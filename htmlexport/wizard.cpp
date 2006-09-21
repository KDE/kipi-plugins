// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/*
A KIPI plugin to generate HTML image galleries
Copyright 2006 Aurelien Gateau

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// Self
#include "wizard.moc"

// Qt
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qpainter.h>
#include <qspinbox.h>

// KDE
#include <kconfigdialogmanager.h>
#include <kdebug.h>
#include <klistbox.h>
#include <klocale.h>
#include <ktextbrowser.h>
#include <kurlrequester.h>
#include <kwizard.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>

// KIPI
#include <libkipi/imagecollectionselector.h>

// Local
#include "pluginsversion.h"
#include "galleryinfo.h"
#include "imagesettingspage.h"
#include "theme.h"
#include "themepage.h"
#include "outputpage.h"

namespace KIPIHTMLExport {


class ThemeListBoxItem : public QListBoxText {
public:
	ThemeListBoxItem(QListBox* list, Theme::Ptr theme)
	: QListBoxText(list, theme->name())
	, mTheme(theme)
	{}

	Theme::Ptr mTheme;
};


struct Wizard::Private {
	GalleryInfo* mInfo;
	KConfigDialogManager* mConfigManager;
	
	KIPI::ImageCollectionSelector* mCollectionSelector;
	ThemePage* mThemePage;
	ImageSettingsPage* mImageSettingsPage;
	OutputPage* mOutputPage;
	
	void initThemePage() {
		KListBox* listBox=mThemePage->mThemeList;
		Theme::List list=Theme::getList();
		Theme::List::Iterator it=list.begin(), end=list.end();
		for (; it!=end; ++it) {
			ThemeListBoxItem* item=new ThemeListBoxItem(listBox, *it);
			if ( (*it)->path()==mInfo->theme() ) {
				listBox->setCurrentItem(item);
			}
		}
	}
};

Wizard::Wizard(QWidget* parent, KIPI::Interface* interface, GalleryInfo* info)
: KWizard(parent)
{
	d=new Private;
	d->mInfo=info;

    // About data and help button.
        
    KAboutData* about = new KAboutData("kipiplugins",
                                        I18N_NOOP("HTML Export"), 
                                        kipiplugins_version,
                                        I18N_NOOP("A KIPI plugin to export image collections to HTML pages"),
                                        KAboutData::License_GPL,
                                        "(c) 2006, Aurelien Gateau", 
                                        0,
                                        "http://extragear.kde.org/apps/kipi");
        
    about->addAuthor("Aurelien Gateau", I18N_NOOP("Author and Maintainer"),
                    "aurelien.gateau@free.fr");
    
    KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("HTML Export Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    helpButton()->setPopup( helpMenu->menu() );
	
	d->mCollectionSelector=new KIPI::ImageCollectionSelector(this, interface);
	addPage(d->mCollectionSelector, i18n("Collection Selection"));

	d->mThemePage=new ThemePage(this);
	d->initThemePage();
	addPage(d->mThemePage, i18n("Theme"));
	connect(d->mThemePage->mThemeList, SIGNAL(selectionChanged()),
		this, SLOT(slotThemeSelectionChanged()) );

	d->mImageSettingsPage=new ImageSettingsPage(this);
	addPage(d->mImageSettingsPage, i18n("Image Settings"));
	
	d->mOutputPage=new OutputPage(this);
	d->mOutputPage->kcfg_destURL->setMode(KFile::Directory);
	addPage(d->mOutputPage, i18n("Output"));

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
	delete d;
}

void Wizard::slotHelp() {
  KApplication::kApplication()->invokeHelp("htmlexport", "kipi-plugins");
}

void Wizard::updateFinishButton() {
	setFinishEnabled(d->mOutputPage, !d->mOutputPage->kcfg_destURL->url().isEmpty());
}


void Wizard::slotThemeSelectionChanged() {
	KListBox* listBox=d->mThemePage->mThemeList;
	KTextBrowser* browser=d->mThemePage->mThemeInfo;
	if (listBox->selectedItem()) {
		Theme::Ptr theme=static_cast<ThemeListBoxItem*>(listBox->selectedItem())->mTheme;
		
		QString url=theme->authorUrl();
		QString author=theme->authorName();
		if (!url.isEmpty()) {
			author=QString("<a href='%1'>%2</a>").arg(url).arg(author);
		}
		
		QString txt=
			QString("<b>%1</b><br><br>%2<br><br>").arg(theme->name(), theme->comment())
			+ i18n("Author: %1").arg(author);
		browser->setText(txt);
		setNextEnabled(d->mThemePage, true);
	} else {
		browser->clear();
		setNextEnabled(d->mThemePage, false);
	}
}


/**
 * Update mInfo
 */
void Wizard::accept() {
	d->mInfo->mCollectionList=d->mCollectionSelector->selectedImageCollections();

	Theme::Ptr theme=static_cast<ThemeListBoxItem*>(d->mThemePage->mThemeList->selectedItem())->mTheme;
	d->mInfo->setTheme(theme->path());

	d->mConfigManager->updateSettings();

	KWizard::accept();
}


} // namespace
