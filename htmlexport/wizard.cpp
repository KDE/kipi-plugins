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
// Self
#include "wizard.moc"

// Qt
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qspinbox.h>

// KDE
#include <kconfigdialogmanager.h>
#include <kdebug.h>
#include <kdialog.h>
#include <klistwidget.h>
#include <klocale.h>
#include <ktextbrowser.h>
#include <kurlrequester.h>
#include <kapplication.h>
#include <kmenu.h>

// KIPI
#include <libkipi/imagecollectionselector.h>

// Local
#include "abstractthemeparameter.h"
#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "galleryinfo.h"
#include "ui_imagesettingspage.h"
#include "theme.h"
#include "ui_themepage.h"
#include "ui_themeparameterspage.h"
#include "ui_outputpage.h"
#include "kpaboutdata.h"

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
		mPage = dialog->addPage(this, title);
	}

	KPageWidgetItem* page() const {
		return mPage;
	}

private:
	KPageWidgetItem* mPage;
};


typedef WizardPage<Ui_ThemePage> ThemePage;
typedef WizardPage<Ui_ImageSettingsPage> ImageSettingsPage;
typedef WizardPage<Ui_ThemeParametersPage> ThemeParametersPage;
typedef WizardPage<Ui_OutputPage> OutputPage;


struct Wizard::Private {
	GalleryInfo* mInfo;
	KConfigDialogManager* mConfigManager;
	
	KIPI::ImageCollectionSelector* mCollectionSelector;
	ThemePage* mThemePage;
	ThemeParametersPage* mThemeParametersPage;
	ImageSettingsPage* mImageSettingsPage;
	OutputPage* mOutputPage;

	KIPIPlugins::KPAboutData* mAbout;
	QMap<QByteArray, QWidget*> mThemeParameterWidgetFromName;
	
	void initThemePage() {
		KListWidget* listWidget=mThemePage->mThemeList;
		Theme::List list=Theme::getList();
		Theme::List::Iterator it=list.begin(), end=list.end();
		for (; it!=end; ++it) {
			Theme::Ptr theme = *it;
			ThemeListBoxItem* item=new ThemeListBoxItem(listWidget, theme);
			if ( theme->internalName()==mInfo->theme() ) {
				listWidget->setCurrentItem(item);
			}
		}
	}

	void fillThemeParametersPage(Theme::Ptr theme) {
		// Delete any previous widgets
		QFrame* content = mThemeParametersPage->content;
		if (content->layout()) {
			Q_FOREACH(QObject* object, content->children()) {
				if (object->isWidgetType()) {
					delete object;
				}
			}
			delete content->layout();
		}
		mThemeParameterWidgetFromName.clear();

		// Create layout. We need to recreate it everytime, to get rid of
		// spacers
		QGridLayout* layout = new QGridLayout(content);
		layout->setSpacing(KDialog::spacingHint());

		// Create widgets
		Theme::ParameterList parameterList = theme->parameterList();
		QString themeInternalName = theme->internalName();
		Theme::ParameterList::ConstIterator
			it = parameterList.begin(),
			end = parameterList.end();
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
			QWidget* widget = themeParameter->createWidget(content, value);
			label->setBuddy(widget);

			int row = layout->rowCount();
			layout->addWidget(label, row, 0);

			if (widget->sizePolicy().expandingDirections() & Qt::Horizontal) {
				// Widget wants full width
				layout->addWidget(widget, row, row, 1, 2);
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

Wizard::Wizard(QWidget* parent, GalleryInfo* info)
: KAssistantDialog(parent)
{
	d=new Private;
	d->mInfo=info;

	// About data
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

	d->mCollectionSelector=new KIPI::ImageCollectionSelector(this);
	addPage(d->mCollectionSelector, i18n("Collection Selection"));

	d->mThemePage=new ThemePage(this, i18n("Theme"));
	d->initThemePage();
	connect(d->mThemePage->mThemeList, SIGNAL(itemSelectionChanged()),
		this, SLOT(slotThemeSelectionChanged()) );

	d->mThemeParametersPage = new ThemeParametersPage(this, i18n("Theme Parameters"));

	d->mImageSettingsPage=new ImageSettingsPage(this, i18n("Image Settings"));
	
	d->mOutputPage=new OutputPage(this, i18n("Output"));
	d->mOutputPage->kcfg_destURL->setMode(KFile::Directory);

	connect(d->mOutputPage->kcfg_destURL, SIGNAL(textChanged(const QString&)),
		this, SLOT(updateFinishButton()) );

	d->mConfigManager=new KConfigDialogManager(this, d->mInfo);
	d->mConfigManager->updateWidgets();

	// Set page states
	// Pages can only be disabled after they have *all* been added!
	slotThemeSelectionChanged();
	updateFinishButton();
}


Wizard::~Wizard() {
	delete d->mAbout;
	delete d;
}


void Wizard::updateFinishButton() {
	setValid(d->mOutputPage->page(), !d->mOutputPage->kcfg_destURL->url().isEmpty());
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
			QString("<b>%1</b><br><br>%2<br><br>").arg(theme->name(), theme->comment())
			+ i18n("Author: %1").arg(author);
		browser->setText(txt);
		setValid(currentPage(), true);

		// Enable theme parameter page if there is any parameter
		Theme::ParameterList parameterList = theme->parameterList();
		setAppropriate(d->mThemeParametersPage->page(), parameterList.size() > 0);

		d->fillThemeParametersPage(theme);
	} else {
		browser->clear();
		setValid(currentPage(), false);
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
		it = parameterList.begin(),
		   end = parameterList.end();
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


} // namespace
