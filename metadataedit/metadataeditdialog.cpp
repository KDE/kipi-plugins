/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-11
 * Description : a plugin to edit pictures metadata
 *
 * Copyright 2006 by Gilles Caulier
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes.

#include <qcombobox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qvgroupbox.h>
#include <qgrid.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>

// KDE includes.

#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <ksqueezedtextlabel.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kstandarddirs.h>
#include <klistview.h>
#include <kfiledialog.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <kglobalsettings.h>
#include <knuminput.h>
#include <kseparator.h>

// Local includes.

#include "exiv2iface.h"
#include "pluginsversion.h"
#include "exifeditdialog.h"
#include "iptceditdialog.h"
#include "metadataitem.h"
#include "metadataeditdialog.h"
#include "metadataeditdialog.moc"

namespace KIPIMetadataEditPlugin
{

class MetadataEditDialogPriv
{
public:

    MetadataEditDialogPriv()
    {
        listView  = 0;
        interface = 0;
    }

    KListView       *listView;

    KIPI::Interface *interface;
};

MetadataEditDialog::MetadataEditDialog( KIPI::Interface* interface, QWidget* parent)
                  : KDialogBase(Plain, i18n("Edit Metadata"), 
                                Help|Apply|Close, Close, 
                                parent, 0, true, true )
{
    d = new MetadataEditDialogPriv;
    d->interface = interface;

    QGridLayout *mainLayout = new QGridLayout(plainPage(), 3, 1, 0, marginHint());

    //---------------------------------------------

    QFrame *headerFrame = new QFrame( plainPage() );
    headerFrame->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    QHBoxLayout* layout = new QHBoxLayout( headerFrame );
    layout->setMargin( 2 ); // to make sure the frame gets displayed
    layout->setSpacing( 0 );
    QLabel *pixmapLabelLeft = new QLabel( headerFrame, "pixmapLabelLeft" );
    pixmapLabelLeft->setScaledContents( false );
    layout->addWidget( pixmapLabelLeft );
    QLabel *labelTitle = new QLabel( i18n("Edit Pictures Metadata"),
                                     headerFrame, "labelTitle" );
    layout->addWidget( labelTitle );
    layout->setStretchFactor( labelTitle, 1 );

    QString directory;
    KGlobal::dirs()->addResourceType("kipi_banner_left", KGlobal::dirs()->kde_default("data") + "kipi/data");
    directory = KGlobal::dirs()->findResourceDir("kipi_banner_left", "banner_left.png");

    pixmapLabelLeft->setPaletteBackgroundColor( QColor(201, 208, 255) );
    pixmapLabelLeft->setPixmap( QPixmap( directory + "banner_left.png" ) );
    labelTitle->setPaletteBackgroundColor( QColor(201, 208, 255) );

    // --------------------------------------------------------------

    d->listView = new KListView(plainPage());
    d->listView->addColumn( i18n("Thumbnail") );
    d->listView->addColumn( i18n("File Name") );
    d->listView->addColumn( i18n("Has EXIF") );
    d->listView->addColumn( i18n("Has IPTC") );
    d->listView->addColumn( i18n("Status") );
    d->listView->setResizeMode(QListView::AllColumns);
    d->listView->setAllColumnsShowFocus(true);
    d->listView->setSorting(-1);
    d->listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->listView->setSelectionMode(QListView::Extended);
    d->listView->setMinimumWidth(450);

    // --------------------------------------------------------------

    QGroupBox   *exifBox       = new QGroupBox(0, Qt::Vertical, i18n("EXIF Actions"), plainPage());
    QGridLayout *exifBoxLayout = new QGridLayout(exifBox->layout(), 2, 1,
                                                 KDialog::spacingHint());

    QPushButton *editExifButton   = new QPushButton(i18n("Edit..."), exifBox);
    QPushButton *loadExifButton   = new QPushButton(i18n("Load..."), exifBox);
    QPushButton *removeExifButton = new QPushButton(i18n("Remove"), exifBox);

    QWhatsThis::add(editExifButton, i18n("<p>Edit EXIF data of selected pictures from the list."));
    QWhatsThis::add(loadExifButton, i18n("<p>Load EXIF data from a picture and set it to selected "
                                         "pictures from the list."));
    QWhatsThis::add(removeExifButton, i18n("<p>Remove EXIF data of selected pictures from the list."));

    exifBoxLayout->addMultiCellWidget(editExifButton, 0, 0, 0, 1);     
    exifBoxLayout->addMultiCellWidget(loadExifButton, 1, 1, 0, 1);     
    exifBoxLayout->addMultiCellWidget(removeExifButton, 2, 2, 0, 1);     

    // --------------------------------------------------------------

    QGroupBox   *iptcBox       = new QGroupBox(0, Qt::Vertical, i18n("IPTC Actions"), plainPage());
    QGridLayout *iptcBoxLayout = new QGridLayout(iptcBox->layout(), 2, 1,
                                                 KDialog::spacingHint());

    QPushButton *editIptcButton   = new QPushButton(i18n("Edit..."), iptcBox);
    QPushButton *loadIptcButton   = new QPushButton(i18n("Load..."), iptcBox);
    QPushButton *removeIptcButton = new QPushButton(i18n("Remove"), iptcBox);

    QWhatsThis::add(editIptcButton, i18n("<p>Edit IPTC data of selected pictures from the list."));
    QWhatsThis::add(loadIptcButton, i18n("<p>Load IPTC data from a picture and set it to selected "
                                         "pictures from the list."));
    QWhatsThis::add(removeIptcButton, i18n("<p>Remove IPTC data of selected pictures from the list."));

    iptcBoxLayout->addMultiCellWidget(editIptcButton, 0, 0, 0, 1);     
    iptcBoxLayout->addMultiCellWidget(loadIptcButton, 1, 1, 0, 1);     
    iptcBoxLayout->addMultiCellWidget(removeIptcButton, 2, 2, 0, 1);     

    // ---------------------------------------------------------------

    mainLayout->addMultiCellWidget(headerFrame, 0, 0, 0, 2);
    mainLayout->addMultiCellWidget(d->listView, 1, 3, 0, 1);
    mainLayout->addMultiCellWidget(exifBox, 1, 1, 2, 2);
    mainLayout->addMultiCellWidget(iptcBox, 2, 2, 2, 2);
    mainLayout->setColStretch(1, 10);
    mainLayout->setRowStretch(3, 10);

    // ---------------------------------------------------------------
    // About data and help button.

    KAboutData* about = new KAboutData("kipiplugins",
                                       I18N_NOOP("Edit Metadata"),
                                       kipiplugins_version,
                                       I18N_NOOP("A Plugin to edit pictures metadata"),
                                       KAboutData::License_GPL,
                                       "(c) 2006, Gilles Caulier",
                                       0,
                                       "http://extragear.kde.org/apps/kipi");

    about->addAuthor("Gilles Caulier", I18N_NOOP("Author and Maintainer"),
                     "caulier dot gilles at kdemail dot net");

    KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Edit Metadata Handbook"),
                                 this, SLOT(slotHelp()), 0, -1, 0);
    actionButton(Help)->setPopup( helpMenu->menu() );

    // ---------------------------------------------------------------

    connect(editExifButton, SIGNAL(released()),
            this, SLOT(slotEditExif()));

    connect(loadExifButton, SIGNAL(released()),
            this, SLOT(slotLoadExif()));

    connect(removeExifButton, SIGNAL(released()),
            this, SLOT(slotRemoveExif()));

    connect(editIptcButton, SIGNAL(released()),
            this, SLOT(slotEditIptc()));

    connect(loadIptcButton, SIGNAL(released()),
            this, SLOT(slotLoadIptc()));

    connect(removeIptcButton, SIGNAL(released()),
            this, SLOT(slotRemoveIptc()));

    readSettings();
}

MetadataEditDialog::~MetadataEditDialog()
{
    delete d;
}

void MetadataEditDialog::slotHelp()
{
    KApplication::kApplication()->invokeHelp("metadataedit", "kipi-plugins");
}

void MetadataEditDialog::setImages( const KURL::List& images )
{
    for( KURL::List::ConstIterator it = images.begin(); it != images.end(); ++it )
        new MetadataItem(d->listView, d->listView->lastItem(), *it);
}

void MetadataEditDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    if (!promptUserClose()) 
    {
        e->ignore();
        return;
    }

    saveSettings();
    e->accept();
}

void MetadataEditDialog::slotClose()
{
    if (!promptUserClose()) return;
    saveSettings();
    KDialogBase::slotClose();
}

bool MetadataEditDialog::promptUserClose()
{
    // Check if one item is dirty in the list.

    QListViewItemIterator it( d->listView );
    int dirty = 0;

    while ( it.current() ) 
    {
        MetadataItem *item = (MetadataItem*) it.current();
        if (item->isDirty())
            dirty++;

        ++it;
    }

    if (dirty > 0)
    {
        QString msg = i18n("1 picture from the list isn't updated.",
                           "%n pictures from the list isn't updated.", dirty);

        if (KMessageBox::No == KMessageBox::warningYesNo(this,
                     i18n("<p>%1\n"
                          "Do you want really to close this window without applying changes?</p>")
                          .arg(msg)))
            return false;
    }

    return true;
}

void MetadataEditDialog::readSettings()
{
    KConfig config("kipirc");
    config.setGroup("Metadata Edit Settings");
    resize(configDialogSize(config, QString("Metadata Edit Dialog")));
}

void MetadataEditDialog::saveSettings()
{
    KConfig config("kipirc");
    config.setGroup("Metadata Edit Settings");
    saveDialogSize(config, QString("Metadata Edit Dialog"));
    config.sync();
}

void MetadataEditDialog::slotEditExif()
{
    if (!d->listView->currentItem())
    {
        KMessageBox::information(this, i18n("Please, select at least one picture from "
                     "the list to edit EXIF metadata manually."), i18n("Edit Metadata"));    
        return;
    }

    MetadataItem* item = (MetadataItem*)d->listView->currentItem();

    EXIFEditDialog dlg(this, item->getExif(), item->getUrl().fileName());

    if (dlg.exec() == KDialogBase::Accepted)
    {
        QListViewItemIterator it(d->listView);

        while (it.current())
        {
            if (it.current()->isSelected())
            {
                MetadataItem *selItem = (MetadataItem*)it.current();
                selItem->setExif(dlg.getEXIFInfo(), true);
            }
            ++it;
        }
    }
}

void MetadataEditDialog::slotEditIptc()
{
    if (!d->listView->currentItem())
    {
        KMessageBox::information(this, i18n("Please, select at least one picture from "
                     "the list to edit IPTC metadata manually."), i18n("Edit Metadata"));    
        return;
    }

    MetadataItem* item = (MetadataItem*)d->listView->currentItem();

    IPTCEditDialog dlg(this, item->getIptc(), item->getUrl().fileName());

    if (dlg.exec() == KDialogBase::Accepted)
    {
        QListViewItemIterator it(d->listView);

        while (it.current())
        {
            if (it.current()->isSelected())
            {
                MetadataItem *selItem = (MetadataItem*)it.current();
                selItem->setIptc(dlg.getIPTCInfo(), true);
            }
            ++it;
        }
    }
}

void MetadataEditDialog::slotLoadExif()
{
    // TODO
}

void MetadataEditDialog::slotLoadIptc()
{
    if (!d->listView->currentItem())
    {
        KMessageBox::information(this, i18n("Please, select at least one picture from "
                     "the list to apply IPTC metadata from a specific picture."), i18n("Edit Metadata"));    
        return;
    }

    KURL loadIPTCFile = KFileDialog::getOpenURL(KGlobalSettings::documentPath(),
                                                QString::null, this,
                                                i18n("Select File to Load IPTC data") );
    if( loadIPTCFile.isEmpty() )
       return;
    
    KIPIPlugins::Exiv2Iface exiv2Iface;
    if (!exiv2Iface.load(loadIPTCFile.path()))
    {
        KMessageBox::error(this, i18n("Cannot load metadata from %1!").arg(loadIPTCFile.fileName()), 
                           i18n("Edit Metadata"));    
        return;
    }
    
    QByteArray iptcData = exiv2Iface.getIptc();
    if (iptcData.isEmpty())
    {
        KMessageBox::error(this, i18n("%1 do not have IPTC metadata!").arg(loadIPTCFile.fileName()), 
                           i18n("Edit Metadata"));    
        return;
    }        

    IPTCEditDialog dlg(this, iptcData, loadIPTCFile.fileName());

    if (dlg.exec() == KDialogBase::Accepted)
    {
        QListViewItemIterator it(d->listView);

        while (it.current())
        {
            if (it.current()->isSelected())
            {
                MetadataItem *selItem = (MetadataItem*)it.current();
                selItem->setIptc(dlg.getIPTCInfo(), true);
            }
            ++it;
        }
    }
}

void MetadataEditDialog::slotRemoveExif()
{
    if (!d->listView->currentItem())
    {
        KMessageBox::information(this, i18n("Please, select at least one picture from "
                     "the list to remove EXIF metadata."), i18n("Edit Metadata"));    
        return;
    }

    QListViewItemIterator it(d->listView);

    while (it.current())
    {
        if (it.current()->isSelected())
        {
            MetadataItem *selItem = (MetadataItem*)it.current();
            selItem->eraseExif();
        }
        ++it;
    }
}

void MetadataEditDialog::slotRemoveIptc()
{
    if (!d->listView->currentItem())
    {
        KMessageBox::information(this, i18n("Please, select at least one picture from "
                     "the list to remove IPTC metadata."), i18n("Metadata Edit"));    
        return;
    }

    QListViewItemIterator it(d->listView);

    while (it.current())
    {
        if (it.current()->isSelected())
        {
            MetadataItem *selItem = (MetadataItem*)it.current();
            selItem->eraseIptc();
        }
        ++it;
    }
}

void MetadataEditDialog::slotApply()
{
    KURL::List images;

    QListViewItemIterator it( d->listView );
    while ( it.current() ) 
    {
        MetadataItem *item = (MetadataItem*)it.current();
        d->listView->setSelected(item, true);
        d->listView->ensureItemVisible(item);
        item->writeMetadataToFile();
        images.append(item->getUrl());

        // TODO : add libkipi method call to please the host to re-read metadata from pictures.

        ++it;
        kapp->processEvents();
    }
    
    d->interface->refreshImages(images);
}

}  // NameSpace KIPIMetadataEditPlugin
