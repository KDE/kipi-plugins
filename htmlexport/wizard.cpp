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
#include <kurlrequester.h>
#include <kwizard.h>

// KIPI
#include <libkipi/imagecollectionselector.h>

// Local
#include "galleryinfo.h"
#include "imagesettingspage.h"
#include "theme.h"
#include "outputpage.h"

namespace KIPIHTMLExport {


class ThemeListBoxItem : public QListBoxText {
public:
	ThemeListBoxItem(QListBox* list, Theme::Ptr theme)
	: QListBoxText(list)
	, mTheme(theme)
	{}

	virtual int height(const QListBox* lb) const {
		return lb->fontMetrics().height()*3;	
	}

	Theme::Ptr mTheme;

protected:
	virtual void paint(QPainter* painter) {
		QListBox* lb=listBox();
		QFontMetrics fm=lb->fontMetrics();
		QFont font=lb->font();
		int top=fm.height()/2;
		
		font.setBold(true);
		painter->setFont(font);
		painter->drawText(0, top + fm.ascent(), mTheme->name());
		top+=fm.height();
		
		font.setBold(false);
		painter->setFont(font);
		painter->drawText(0, top + fm.ascent(), mTheme->comment());
	}
};


struct Wizard::Private {
	GalleryInfo* mInfo;
	KConfigDialogManager* mConfigManager;
	
	KIPI::ImageCollectionSelector* mCollectionSelector;
	KListBox* mAppearancePage;
	ImageSettingsPage* mImageSettingsPage;
	OutputPage* mOutputPage;
	
	void initAppearancePage(QWidget* parent) {
		mAppearancePage=new KListBox(parent);
		Theme::List list=Theme::getList();
		Theme::List::Iterator it=list.begin(), end=list.end();
		for (; it!=end; ++it) {
			ThemeListBoxItem* item=new ThemeListBoxItem(mAppearancePage, *it);
			if ( (*it)->path()==mInfo->theme() ) {
				mAppearancePage->setCurrentItem(item);
			}
		}
	}
};

Wizard::Wizard(QWidget* parent, KIPI::Interface* interface, GalleryInfo* info)
: KWizard(parent)
{
	d=new Private;
	d->mInfo=info;
	
	d->mCollectionSelector=new KIPI::ImageCollectionSelector(this, interface);
	addPage(d->mCollectionSelector, i18n("Collection Selection"));

	d->initAppearancePage(this);
	addPage(d->mAppearancePage, i18n("Gallery Appearance"));
	connect(d->mAppearancePage, SIGNAL(selectionChanged()),
		this, SLOT(updateAppearancePageNextButton()) );

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
	setNextEnabled(d->mAppearancePage, d->mAppearancePage->currentItem()!=-1);
	updateFinishButton();
}


Wizard::~Wizard() {
	delete d;
}


void Wizard::updateFinishButton() {
	setFinishEnabled(d->mOutputPage, !d->mOutputPage->kcfg_destURL->url().isEmpty());
}


void Wizard::updateAppearancePageNextButton() {
	setNextEnabled(d->mAppearancePage, d->mAppearancePage->selectedItem());
}


/**
 * Update mInfo
 */
void Wizard::accept() {
	d->mInfo->mCollectionList=d->mCollectionSelector->selectedImageCollections();

	Theme::Ptr theme=static_cast<ThemeListBoxItem*>(d->mAppearancePage->selectedItem())->mTheme;
	d->mInfo->setTheme(theme->path());

	d->mConfigManager->updateSettings();

	KWizard::accept();
}


} // namespace
