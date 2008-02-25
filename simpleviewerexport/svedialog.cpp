/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-12-19
 * Description : a plugin to export image collections using SimpleViewer.
 *
 * Copyright (C) 2005-2006 by Joern Ahrens <joern dot ahrens at kdemail dot net>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

// QT include files

#include <qframe.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>
#include <qhgroupbox.h>
#include <qvgroupbox.h>
#include <qcombobox.h>
#include <qhbox.h>
#include <qvbox.h>

// KDE include files

#include <klineedit.h>
#include <klocale.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kcolorbutton.h>
#include <kurlrequester.h>
#include <kglobalsettings.h>
#include <kconfig.h>
#include <kapplication.h>

// KIPI include files

#include <libkipi/imagecollectionselector.h>

// Local include files

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "svedialog.h"
#include "svedialog.moc"

namespace KIPISimpleViewerExportPlugin
{

SVEDialog::SVEDialog(KIPI::Interface* interface, QWidget *parent)
          : KDialogBase( IconList, i18n("Flash Export"), Help|Ok|Cancel, Ok,
                         parent, "SimpleViewerExportDialog", true, true ),
            m_interface( interface )
{
    selectionPage();
    generalPage();
    lookPage();

    readConfig();

    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(I18N_NOOP("Flash Export"),
                                           kipiplugins_version,
                                           KAboutData::License_GPL,
                                           I18N_NOOP("A Kipi plugin to export images to Flash using Simple Viewer component"),
                                           "(c) 2005-2006, Joern Ahrens\n"
                                           "(c) 2008, Gilles Caulier");

    m_about->addAuthor("Joern Ahrens", 
                       I18N_NOOP("Author and maintainer"),
                       "joern dot ahrens at kdemail dot net");

    m_about->addAuthor("Gilles Caulier", 
                       I18N_NOOP("Developer and maintainer"),
                       "caulier dot gilles at gmail dot com");

    m_about->addCredit("Felix Turner",
                       "Author of the SimpleViewer flash application",
                       0,
                       "http://www.airtightinteractive.com/simpleviewer");

    m_about->addCredit("Mikkel B. Stegmann",
                       "Basis for the index.html template",
                       0,
                       "http://www.stegmann.dk/mikkel/porta");

    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Plugin Handbook"),
                                 this, SLOT(slotHelp()), 0, -1, 0);
    actionButton(Help)->setPopup( helpMenu->menu() );
}

SVEDialog::~SVEDialog()
{
    delete m_about;
}

void SVEDialog::slotHelp()
{
    KApplication::kApplication()->invokeHelp("simpleviewerexport", "kipi-plugins");
}

void SVEDialog::readConfig()
{
    KConfig config("kipirc");

    setThumbnailRows(config.readNumEntry("thumbnailRows", 3));
    setThumbnailColumns(config.readNumEntry("thumbnailColumns", 3));
    m_navPosition->setCurrentItem(config.readNumEntry("navPosition", 1));
    m_navDirection->setCurrentItem(config.readNumEntry("navDirection", 1));
    setTextColor(QColor(config.readEntry("textColor", "#ffffff")));
    setBackgroundColor(QColor(config.readEntry("backgroundColor", "#181818")));
    setFrameColor(QColor(config.readEntry("frameColor", "#ffffff")));
    setFrameWidth(config.readNumEntry("frameWidth", 1));
    setStagePadding(config.readNumEntry("stagePadding", 20));
    setTitle(config.readEntry("title", QString()));
    m_exportURL->setURL(config.readPathEntry("exporturl",  KGlobalSettings::documentPath() + "simpleviewer"));
    setResizeExportImages(config.readBoolEntry("resizeExportImages", true));
    setImagesExportSize(config.readNumEntry("imagesExportSize", 640));
    setMaxImagesDimension(config.readNumEntry("maxImageDimension", 640));
    setShowExifComments(config.readBoolEntry("showExifComments", true));

    resize(configDialogSize(config, QString("SimpleViewerExport Dialog")));
}

void SVEDialog::writeConfig()
{
    KConfig config("kipirc");
    config.writeEntry("thumbnailRows", thumbnailRows());
    config.writeEntry("thumbnailColumns", thumbnailColumns());
    config.writeEntry("navPosition", m_navPosition->currentItem());
    config.writeEntry("navDirection", m_navDirection->currentItem());
    config.writeEntry("textColor", textColor().name());
    config.writeEntry("backgroundColor", backgroundColor().name());
    config.writeEntry("frameColor", frameColor().name());
    config.writeEntry("frameWidth", frameWidth());
    config.writeEntry("stagePadding", stagePadding());
    config.writePathEntry("exporturl", exportURL());
    config.writeEntry("title", title());
    config.writeEntry("resizeExportImages", resizeExportImages());
    config.writeEntry("imagesExportSize", imagesExportSize());
    config.writeEntry("maxImageDimension", maxImageDimension());
    config.writeEntry("showExifComments", showExifComments());

    saveDialogSize(config, QString("GPS Sync Dialog"));
    config.sync();
}

void SVEDialog::selectionPage()
{
    m_selectionPage = addPage(i18n("Selection"), i18n("Album Selection"),
                              BarIcon("folder_image", KIcon::SizeMedium));

    QVBoxLayout *layout       = new QVBoxLayout( m_selectionPage, 0, spacingHint() );
    m_imageCollectionSelector = new KIPI::ImageCollectionSelector( m_selectionPage, m_interface );
    layout->addWidget(m_imageCollectionSelector);
}

void SVEDialog::lookPage()
{
    m_lookPage = addPage(i18n("Look"), i18n("Page Look"),
                              BarIcon("html", KIcon::SizeMedium));

    QVBoxLayout *mainLayout = new QVBoxLayout(m_lookPage, 0, spacingHint());
    QVGroupBox *vgroupbox;
    QHBox *hbox;
    QLabel *label;

    // ------------------------------------------------------------------------

    vgroupbox = new QVGroupBox(i18n("Navigation"), m_lookPage);
    mainLayout->addWidget(vgroupbox);

    m_thumbnailRows = new KIntNumInput(3, vgroupbox);
    m_thumbnailRows->setRange(1, 10, 1, true);
    m_thumbnailRows->setLabel(i18n("Thumbnail &Rows:"), AlignVCenter);
    QWhatsThis::add(m_thumbnailRows, i18n("<p>Number of thumbnails rows"));

    // ------------------------------------------------------------------------

    m_thumbnailColumns = new KIntNumInput(3, vgroupbox);
    m_thumbnailColumns->setRange(1, 10, 1, true);
    m_thumbnailColumns->setLabel(i18n("Thumbnail &Columns:"), AlignVCenter);
    QWhatsThis::add(m_thumbnailColumns, i18n("<p>Number of thumbnails columns"));

    // ------------------------------------------------------------------------

    hbox  = new QHBox(vgroupbox);
    label = new QLabel(i18n("Thumbnail &Position:"), hbox);
    m_navPosition = new QComboBox(false, hbox);
    m_navPosition->insertItem(i18n("Right"));
    m_navPosition->insertItem(i18n("Left"));
    m_navPosition->insertItem(i18n("Top"));
    m_navPosition->insertItem(i18n("Bottom"));
    m_navPosition->setCurrentText(i18n("Right"));
    label->setBuddy(m_navPosition);

    // ------------------------------------------------------------------------

    hbox  = new QHBox(vgroupbox);
    label = new QLabel(i18n("&Direction of Navigation:"), hbox);
    m_navDirection = new QComboBox(false, hbox);
    m_navDirection->insertItem(i18n("Left to Right"));
    m_navDirection->insertItem(i18n("Right to Left"));
    m_navDirection->setCurrentText(i18n("Left to Right"));
    label->setBuddy(m_navDirection);

    // ------------------------------------------------------------------------

    vgroupbox = new QVGroupBox(i18n("Colors"), m_lookPage);
    mainLayout->addWidget(vgroupbox);

    hbox  = new QHBox(vgroupbox);
    label = new QLabel(i18n("&Text Color:"), hbox);
    m_textColor = new KColorButton(QColor("#ffffff"), hbox);
    label->setBuddy(m_textColor);

    // ------------------------------------------------------------------------

    hbox  = new QHBox(vgroupbox);
    label = new QLabel(i18n("&Background Color:"), hbox);
    m_backgroundColor = new KColorButton(QColor("#181818"), hbox);
    label->setBuddy(m_backgroundColor);

    // ------------------------------------------------------------------------

    hbox  = new QHBox(vgroupbox);
    label = new QLabel(i18n("&Frame Color:"), hbox);
    m_frameColor = new KColorButton(QColor("#ffffff"), hbox);
    label->setBuddy(m_frameColor);

    // ------------------------------------------------------------------------

    vgroupbox = new QVGroupBox(i18n("Style"), m_lookPage);
    mainLayout->addWidget(vgroupbox);

    m_frameWidth = new KIntNumInput(3, vgroupbox);
    m_frameWidth->setRange(0, 10, 1, true);
    m_frameWidth->setLabel(i18n("Frame &Width:"), AlignVCenter);
    QWhatsThis::add(m_frameWidth, i18n("<p>Width of image frame in pixels."));

    // ------------------------------------------------------------------------

    m_stagePadding = new KIntNumInput(20, vgroupbox);
    m_stagePadding->setRange(1, 100, 1, true);
    m_stagePadding->setLabel(i18n("Stage &Padding:"), AlignVCenter);
    QWhatsThis::add(m_stagePadding, i18n("<p>tagePadding: Distance between image and thumbnails in pixels."));

    mainLayout->addStretch(1);
}

void SVEDialog::generalPage()
{
    m_generalPage = addPage(i18n("General"), i18n("General Setup"),
                            BarIcon("html", KIcon::SizeMedium));

    QVBoxLayout *mainLayout = new QVBoxLayout(m_generalPage, 0, spacingHint());

    QHGroupBox *hgroupbox;
    QVGroupBox *vgroupbox;

    // ------------------------------------------------------------------------

    hgroupbox = new QHGroupBox(i18n("Gallery &Title"), m_generalPage);
    mainLayout->addWidget(hgroupbox);

    m_title   = new KLineEdit(hgroupbox);
    QWhatsThis::add(m_title, i18n("<p>Enter here the gallery title"));

    // ------------------------------------------------------------------------

    vgroupbox   = new QVGroupBox(i18n("Save Gallery To"), m_generalPage);
    mainLayout->addWidget(vgroupbox);
    m_exportURL = new KURLRequester(KGlobalSettings::documentPath() + "simpleviewer", vgroupbox);
    m_exportURL->setMode(KFile::Directory | KFile::LocalOnly);

    // ------------------------------------------------------------------------

    vgroupbox = new QVGroupBox(i18n("Image Size"), m_generalPage);
    mainLayout->addWidget(vgroupbox);

    m_resizeExportImages = new QCheckBox(i18n("Resize Target Images"), vgroupbox);
    m_resizeExportImages->setChecked(true);
    QWhatsThis::add(m_resizeExportImages, i18n("<p>If you enable this option, "
                                               "all target images can be resized.") );

    m_imagesExportSize = new KIntNumInput(640, vgroupbox);
    m_imagesExportSize->setRange(200, 2000, 1, true );
    m_imagesExportSize->setLabel(i18n("&Target Images Size:"), AlignVCenter);
    QWhatsThis::add(m_imagesExportSize, i18n("<p>The new size of the exported images in pixels"
                                             "SimpleViewer resizes the images as well, but this"
                                             "resizes your images before they are uploaded to your server"));
    connect(m_resizeExportImages, SIGNAL(toggled(bool)),
            m_imagesExportSize, SLOT(setEnabled(bool)));

    // ------------------------------------------------------------------------

    m_maxImageDimension = new KIntNumInput(m_imagesExportSize, 640, vgroupbox);
    m_maxImageDimension->setRange(200, 2000, 1, true );
    m_maxImageDimension->setLabel(i18n("&Displayed Images Size:"), AlignVCenter);
    QWhatsThis::add(m_maxImageDimension, i18n("<p>scales the displayed images to this size. "
                                              "Largest height or width of your largest image (in pixels). "
                                              "Images will not be scaled up above this size, to ensure best image quality."));

    // ------------------------------------------------------------------------

    vgroupbox = new QVGroupBox(i18n("Misc"), m_generalPage);
    mainLayout->addWidget(vgroupbox);

    m_showExifComments = new QCheckBox(i18n("Display Captions"), vgroupbox);
    m_showExifComments->setChecked(true);
    QWhatsThis::add(m_showExifComments, i18n("<p>If you enable this option, "
                                             "the images caption will be shown"));

    mainLayout->addStretch(1);
}


void SVEDialog::slotOk()
{
    m_selectedAlbums = m_imageCollectionSelector->selectedImageCollections();

    if (m_selectedAlbums.size() == 0)
    {
        KMessageBox::sorry(this, i18n("You must select at least one album."));
        return;
    }
    writeConfig();
    accept();
}

QValueList<KIPI::ImageCollection> SVEDialog::getSelectedAlbums() const
{
    return m_selectedAlbums;
}

void SVEDialog::setTitle(const QString &title)
{
    m_title->setText(title);
}

QString SVEDialog::title() const
{
    return m_title->text();
}

bool SVEDialog::resizeExportImages() const
{
    return m_resizeExportImages->isChecked();
}

void SVEDialog::setResizeExportImages(bool resize)
{
    m_resizeExportImages->setChecked(resize);
}

int SVEDialog::imagesExportSize() const
{
    return m_imagesExportSize->value();
}

void SVEDialog::setImagesExportSize(int size)
{
    m_imagesExportSize->setValue(size);
}

int SVEDialog::maxImageDimension() const
{
    return m_maxImageDimension->value();
}

void SVEDialog::setMaxImagesDimension(int size)
{
    m_maxImageDimension->setValue(size);
}

bool SVEDialog::showExifComments() const
{
    return m_showExifComments->isChecked();
}

void SVEDialog::setShowExifComments(bool show)
{
    m_showExifComments->setChecked(show);
}

QString SVEDialog::exportURL() const
{
    return m_exportURL->url();
}

int SVEDialog::thumbnailRows() const
{
    return m_thumbnailRows->value();
}

void SVEDialog::setThumbnailRows(int rows)
{
    m_thumbnailRows->setValue(rows);
}

int SVEDialog::thumbnailColumns() const
{
    return m_thumbnailColumns->value();
}

void SVEDialog::setThumbnailColumns(int columns)
{
    m_thumbnailColumns->setValue(columns);
}

int SVEDialog::frameWidth() const
{
    return m_frameWidth->value();
}

void SVEDialog::setFrameWidth(int width)
{
    m_frameWidth->setValue(width);
}

int SVEDialog::stagePadding() const
{
    return m_stagePadding->value();
}

void SVEDialog::setStagePadding(int stagePadding)
{
    m_stagePadding->setValue(stagePadding);
}

QColor SVEDialog::textColor() const
{
    return m_textColor->color();
}

void SVEDialog::setTextColor(const QColor &color)
{
    m_textColor->setColor(color);
}

QColor SVEDialog::frameColor() const
{
    return m_frameColor->color();
}

void SVEDialog::setFrameColor(const QColor &color)
{
    m_frameColor->setColor(color);
}

QColor SVEDialog::backgroundColor() const
{
    return m_backgroundColor->color();
}

void SVEDialog::setBackgroundColor(const QColor &color)
{
    m_backgroundColor->setColor(color);
}

QString SVEDialog::navDirection() const
{
    if(m_navDirection->currentText() == i18n("Left to Right"))
        return "LTR";
    else
        return "RTL";
}

void SVEDialog::setNavDirection(const QString &direction)
{
    if(direction == "LTR")
        m_navDirection->setCurrentText(i18n("Left to Right"));
    else
        m_navDirection->setCurrentText(i18n("Right to Left"));
}

QString SVEDialog::navPosition() const
{
    QString pos = m_navPosition->currentText();

    if(pos == i18n("Top"))
        return "top";
    else if(pos == i18n("Bottom"))
        return "bottom";
    else if(pos == i18n("Left"))
        return "left";
    else
        return "right";
}

void SVEDialog::setNavPosition(const QString &pos)
{
    m_navPosition->setCurrentText(pos);
}

} //  namespace KIPISimpleViewerExportPlugin
