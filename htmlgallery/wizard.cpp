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
#include <qspinbox.h>

// KDE
#include <klistbox.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kurlrequester.h>
#include <kwizard.h>

// KIPI
#include <libkipi/imagecollectionselector.h>

// Local
#include "galleryinfo.h"
#include "imagesettingspage.h"
#include "outputpage.h"

namespace KIPIHTMLGallery {

struct Wizard::Private {
	GalleryInfo* mInfo;
	
	KIPI::ImageCollectionSelector* mCollectionSelector;
	KListBox* mAppearancePage;
	ImageSettingsPage* mImageSettingsPage;
	OutputPage* mOutputPage;
	
	void initAppearancePage(QWidget* parent) {
		mAppearancePage=new KListBox(parent);
		QStringList list=KGlobal::instance()->dirs()->findAllResources("data", "kipiplugin_htmlgallery/themes/*/template.xslt");
		QStringList::Iterator it=list.begin();
		QStringList::Iterator end=list.end();
		for (;it!=end; ++it) {
			QFileInfo info(*it);
			QString theme=info.dir().dirName();
			mAppearancePage->insertItem(theme);
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
	d->mOutputPage->destURL->setMode(KFile::Directory);
	addPage(d->mOutputPage, i18n("Output"));

	connect(d->mOutputPage->destURL, SIGNAL(textChanged(const QString&)),
		this, SLOT(updateFinishButton()) );
	
	// Pages can only be disabled after they have *all* been added!
	setNextEnabled(d->mAppearancePage, false);
}


Wizard::~Wizard() {
	delete d;
}


void Wizard::updateFinishButton() {
	setFinishEnabled(d->mOutputPage, !d->mOutputPage->destURL->url().isEmpty());
}


void Wizard::updateAppearancePageNextButton() {
	setNextEnabled(d->mAppearancePage, !d->mAppearancePage->currentText().isEmpty());
}


void Wizard::accept() {
	// FIXME: Handle remote urls
	d->mInfo->mDestURL=d->mOutputPage->destURL->url();
	d->mInfo->mCollectionList=d->mCollectionSelector->selectedImageCollections();
	d->mInfo->mOpenInBrowser=d->mOutputPage->openInBrowser->isChecked();

	d->mInfo->mTheme=d->mAppearancePage->currentText();

	d->mInfo->mFullResize=d->mImageSettingsPage->fullResize->isChecked();
	d->mInfo->mFullSize=d->mImageSettingsPage->fullSize->value();
	d->mInfo->mFullFormat=d->mImageSettingsPage->fullFormat->currentText();
	
	d->mInfo->mThumbnailSize=d->mImageSettingsPage->thumbnailSize->value();
	d->mInfo->mThumbnailFormat=d->mImageSettingsPage->thumbnailFormat->currentText();

	KWizard::accept();
}


} // namespace
