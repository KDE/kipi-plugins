// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/*
A KIPI plugin to generate HTML image galleries
Copyright 2006 by Aurelien Gateau <aurelien dot gateau at free.fr>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.
*/

#include "wizard.moc"

// Qt includes

#include <QCheckBox>
#include <QLabel>

// KDE includes

#include <kapplication.h>
#include <kcombobox.h>
#include <kconfigdialogmanager.h>
#include <kdebug.h>
#include <kdialog.h>
#include <klistwidget.h>
#include <klocale.h>
#include <kmenu.h>
#include <ktextbrowser.h>
#include <kurlrequester.h>
#include <ktoolinvocation.h>
#include <kpushbutton.h>
#include <khelpmenu.h>

// KIPI includes

#include <libkipi/imagecollectionselector.h>
#include <libkipi/interface.h>

// Local includes

#include "abstractthemeparameter.h"
#include "galleryinfo.h"
#include "invisiblebuttongroup.h"
#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "theme.h"
#include "ui_imagesettingspage.h"
#include "ui_outputpage.h"
#include "ui_themepage.h"
#include "ui_themeparameterspage.h"

namespace KIPIHTMLExport {


class ThemeListBoxItem : public QListWidgetItem {
public:
    ThemeListBoxItem(QListWidget* list, Theme::Ptr theme)
    : QListWidgetItem(theme->name(), list)
    , mTheme(theme)
    {}

    Theme::Ptr mTheme;
};


template <class Ui_Class>
class WizardPage : public QWidget, public Ui_Class {
public:
    WizardPage(KAssistantDialog* dialog, const QString& title)
    : QWidget(dialog) {
        setupUi(this);
        layout()->setMargin(0);
        mPage = dialog->addPage(this, title);
    }

    KPageWidgetItem* page() const {
        return mPage;
    }

private:
    KPageWidgetItem* mPage;
};


typedef WizardPage<Ui_ThemePage> ThemePage;
typedef WizardPage<Ui_ThemeParametersPage> ThemeParametersPage;
typedef WizardPage<Ui_OutputPage> OutputPage;

class ImageSettingsPage : public WizardPage<Ui_ImageSettingsPage> {
public:
    ImageSettingsPage(KAssistantDialog* dialog, const QString& title)
    : WizardPage<Ui_ImageSettingsPage>(dialog, title)
    {
        InvisibleButtonGroup* group = new InvisibleButtonGroup(this);
        group->setObjectName("kcfg_useOriginalImageAsFullImage");
        group->addButton(mSaveImageButton, int(false));
        group->addButton(mUseOriginalImageButton, int(true));
    }
};


struct Wizard::Private {
    GalleryInfo* mInfo;
    KConfigDialogManager* mConfigManager;

    KIPI::ImageCollectionSelector* mCollectionSelector;
    KPageWidgetItem* mCollectionSelectorPage;
    ThemePage* mThemePage;
    ThemeParametersPage* mThemeParametersPage;
    ImageSettingsPage* mImageSettingsPage;
    OutputPage* mOutputPage;

    KIPIPlugins::KPAboutData* mAbout;
    QMap<QByteArray, QWidget*> mThemeParameterWidgetFromName;

    void initThemePage() {
        KListWidget* listWidget=mThemePage->mThemeList;
        Theme::List list=Theme::getList();
        Theme::List::ConstIterator it=list.constBegin(), end=list.constEnd();
        for (; it!=end; ++it) {
            Theme::Ptr theme = *it;
            ThemeListBoxItem* item=new ThemeListBoxItem(listWidget, theme);
            if ( theme->internalName()==mInfo->theme() ) {
                listWidget->setCurrentItem(item);
            }
        }
    }

    void fillThemeParametersPage(Theme::Ptr theme) {
        // Create a new content page
        delete mThemeParametersPage->content;
        QWidget* content = new QWidget;
        mThemeParametersPage->content = content;
        mThemeParametersPage->scrollArea->setWidget(mThemeParametersPage->content);
        mThemeParametersPage->scrollArea->viewport()->setAutoFillBackground(false);
        content->setAutoFillBackground(false);
        mThemeParameterWidgetFromName.clear();

        // Create layout. We need to recreate it every time, to get rid of
        // spacers
        QGridLayout* layout = new QGridLayout(content);
        layout->setMargin(0);
        layout->setSpacing(KDialog::spacingHint());

        // Create widgets
        Theme::ParameterList parameterList = theme->parameterList();
        QString themeInternalName = theme->internalName();
        Theme::ParameterList::ConstIterator
            it = parameterList.constBegin(),
            end = parameterList.constEnd();
        for (; it!=end; ++it) {
            AbstractThemeParameter* themeParameter = *it;
            QByteArray internalName = themeParameter->internalName();
            QString value = mInfo->getThemeParameterValue(
                themeInternalName,
                internalName,
                themeParameter->defaultValue());

            QString name = themeParameter->name();
            name = i18nc("'%1' is a label for a theme parameter", "%1:", name);

            QLabel* label = new QLabel(name, content);
            label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            QWidget* widget = themeParameter->createWidget(content, value);
            label->setBuddy(widget);

            int row = layout->rowCount();
            layout->addWidget(label, row, 0);

            if (widget->sizePolicy().expandingDirections() & Qt::Horizontal) {
                // Widget wants full width
                layout->addWidget(widget, row, 1, 1, 2);
            } else {
                // Widget doesn't like to be stretched, add a spacer next to it
                layout->addWidget(widget, row, 1);
                QSpacerItem* spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
                layout->addItem(spacer, row, 2);
            }

            mThemeParameterWidgetFromName[internalName] = widget;
        }

        // Add spacer at the end, so that widgets aren't spread on the whole
        // parent height
        QSpacerItem* spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout->addItem(spacer, layout->rowCount(), 0);
    }
};

Wizard::Wizard(QWidget* parent, GalleryInfo* info, KIPI::Interface* interface)
: KAssistantDialog(parent)
{
    d=new Private;
    d->mInfo=info;

    setCaption(i18n("Export image collections to HTML pages"));

    // ---------------------------------------------------------------
    // About data and help button.

    d->mAbout = new KIPIPlugins::KPAboutData(
        ki18n("HTML Export"),
        QByteArray(),
        KAboutData::License_GPL,
        ki18n("A KIPI plugin to export image collections to HTML pages"),
        ki18n("(c) 2006-2008, Aurelien Gateau"));

    d->mAbout->addAuthor(
        ki18n("Aurelien Gateau"),
        ki18n("Author and Maintainer"),
        "aurelien.gateau@free.fr");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(this, d->mAbout, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    // ---------------------------------------------------------------

    d->mCollectionSelector = interface->imageCollectionSelector(this);
    d->mCollectionSelectorPage = addPage(d->mCollectionSelector, i18n("Collection Selection"));
    updateCollectionSelectorPageValidity();
    connect(d->mCollectionSelector, SIGNAL(selectionChanged()),
        SLOT(updateCollectionSelectorPageValidity()));

    d->mThemePage=new ThemePage(this, i18n("Theme"));
    d->initThemePage();
    connect(d->mThemePage->mThemeList, SIGNAL(itemSelectionChanged()),
        this, SLOT(slotThemeSelectionChanged()) );

    d->mThemeParametersPage = new ThemeParametersPage(this, i18n("Theme Parameters"));

    d->mImageSettingsPage=new ImageSettingsPage(this, i18n("Image Settings"));

    d->mOutputPage=new OutputPage(this, i18n("Output"));
    d->mOutputPage->kcfg_destUrl->setMode(KFile::Directory);

    connect(d->mOutputPage->kcfg_destUrl, SIGNAL(textChanged(const QString&)),
        this, SLOT(updateFinishPageValidity()) );

    d->mConfigManager=new KConfigDialogManager(this, d->mInfo);
    d->mConfigManager->updateWidgets();

    // Set page states
    // Pages can only be disabled after they have *all* been added!
    slotThemeSelectionChanged();
    updateFinishPageValidity();
}


Wizard::~Wizard() {
    delete d->mAbout;
    delete d;
}

void Wizard::slotHelp() {
    KToolInvocation::invokeHelp("htmlexport", "kipi-plugins");
}

void Wizard::updateFinishPageValidity() {
    setValid(d->mOutputPage->page(), !d->mOutputPage->kcfg_destUrl->url().isEmpty());
}


void Wizard::updateCollectionSelectorPageValidity() {
    setValid(d->mCollectionSelectorPage, !d->mCollectionSelector->selectedImageCollections().empty());
}


void Wizard::slotThemeSelectionChanged() {
    KListWidget* listWidget=d->mThemePage->mThemeList;
    KTextBrowser* browser=d->mThemePage->mThemeInfo;
    if (listWidget->currentItem()) {
        Theme::Ptr theme=static_cast<ThemeListBoxItem*>(listWidget->currentItem())->mTheme;

        QString url=theme->authorUrl();
        QString author=theme->authorName();
        if (!url.isEmpty()) {
            author=QString("<a href='%1'>%2</a>").arg(url).arg(author);
        }

        QString txt=
            QString("<b>%1</b><br/><br/>%2<br/><br/>").arg(theme->name(), theme->comment())
            + i18n("Author: %1", author);
        browser->setText(txt);
        setValid(d->mThemePage->page(), true);

        // Enable theme parameter page if there is any parameter
        Theme::ParameterList parameterList = theme->parameterList();
        setAppropriate(d->mThemeParametersPage->page(), parameterList.size() > 0);

        d->fillThemeParametersPage(theme);
    } else {
        browser->clear();
        setValid(d->mThemePage->page(), false);
    }
}


/**
 * Update mInfo
 */
void Wizard::accept() {
    d->mInfo->mCollectionList=d->mCollectionSelector->selectedImageCollections();

    Theme::Ptr theme=static_cast<ThemeListBoxItem*>(d->mThemePage->mThemeList->currentItem())->mTheme;
    QString themeInternalName = theme->internalName();
    d->mInfo->setTheme(themeInternalName);

    Theme::ParameterList parameterList = theme->parameterList();
    Theme::ParameterList::ConstIterator
        it = parameterList.constBegin(),
           end = parameterList.constEnd();
    for (; it!=end; ++it) {
        AbstractThemeParameter* themeParameter = *it;
        QByteArray parameterInternalName = themeParameter->internalName();
        QWidget* widget = d->mThemeParameterWidgetFromName[parameterInternalName];
        QString value = themeParameter->valueFromWidget(widget);

        d->mInfo->setThemeParameterValue(
            themeInternalName,
            parameterInternalName,
            value);
    }

    d->mConfigManager->updateSettings();

    KAssistantDialog::accept();
}


} // namespace KIPIHTMLExport
