/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-10-24
 * Description : XMP workflow status properties settings page.
 *
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// QT includes.

#include <QLayout>
#include <QLabel>
#include <QValidator>
#include <QCheckBox>
#include <QPushButton>
#include <QComboBox>

// KDE includes.

#include <kiconloader.h>
#include <klocale.h>
#include <klanguagebutton.h>
#include <kdialog.h>
#include <kglobal.h>
#include <kseparator.h>
#include <ktextedit.h>
#include <klineedit.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// Local includes.

#include "pluginsversion.h"
#include "squeezedcombobox.h"
#include "metadatacheckbox.h"
#include "multivaluesedit.h"
#include "altlangstringedit.h"
#include "objectattributesedit.h"
#include "xmpproperties.h"
#include "xmpproperties.moc"

namespace KIPIMetadataEditPlugin
{

class XMPPropertiesPriv
{
public:

    XMPPropertiesPriv()
    {
        priorityCB              = 0;
        priorityCheck           = 0;
        sceneEdit               = 0;
        objectTypeCB            = 0;
        objectTypeCheck         = 0;
        objectAttribute         = 0;
        languageBtn             = 0;
        languageCheck           = 0;

        sceneCodeMap.insert( "010100", i18n("Headshot") );
        sceneCodeMap.insert( "010200", i18n("Half-length") );
        sceneCodeMap.insert( "010300", i18n("Full-length") );
        sceneCodeMap.insert( "010400", i18n("Profile") );
        sceneCodeMap.insert( "010500", i18n("Rear view") );
        sceneCodeMap.insert( "010600", i18n("Single") );
        sceneCodeMap.insert( "010700", i18n("Couple") );
        sceneCodeMap.insert( "010800", i18n("Two") );
        sceneCodeMap.insert( "010900", i18n("Group") );
        sceneCodeMap.insert( "011000", i18n("General view") );
        sceneCodeMap.insert( "011100", i18n("Panoramic view") );
        sceneCodeMap.insert( "011200", i18n("Aerial view") );
        sceneCodeMap.insert( "011300", i18n("Under-water") );
        sceneCodeMap.insert( "011400", i18n("Night scene") );
        sceneCodeMap.insert( "011500", i18n("Satellite") );
        sceneCodeMap.insert( "011600", i18n("Exterior view") );
        sceneCodeMap.insert( "011700", i18n("Interior view") );
        sceneCodeMap.insert( "011800", i18n("Close-up") );
        sceneCodeMap.insert( "011900", i18n("Action") );
        sceneCodeMap.insert( "012000", i18n("Performing") );
        sceneCodeMap.insert( "012100", i18n("Posing") );
        sceneCodeMap.insert( "012200", i18n("Symbolic") );
        sceneCodeMap.insert( "012300", i18n("Off-beat") );
        sceneCodeMap.insert( "012400", i18n("Movie scene") );
    }

    typedef QMap<QString, QString>  SceneCodeMap; 

    SceneCodeMap                    sceneCodeMap;

    QComboBox                      *priorityCB;
    QComboBox                      *objectTypeCB;

    KLineEdit                      *objectTypeDescEdit;

    KLanguageButton                *languageBtn;

    MetadataCheckBox               *priorityCheck;
    MetadataCheckBox               *objectTypeCheck;
    MetadataCheckBox               *languageCheck;

    MultiValuesEdit                *sceneEdit;

    ObjectAttributesEdit           *objectAttribute;
};

XMPProperties::XMPProperties(QWidget* parent)
         : QWidget(parent)
{
    d = new XMPPropertiesPriv;

    QGridLayout* grid = new QGridLayout(this);

    // --------------------------------------------------------

    d->languageCheck = new MetadataCheckBox(i18n("Language:"), this);
    d->languageBtn   = new KLanguageButton(this);

    QStringList list = KGlobal::locale()->allLanguagesList();
    for (QStringList::Iterator it = list.begin(); it != list.end(); ++it)
    {
        // Only get all ISO 639 language code based on 2 characters
        // http://xml.coverpages.org/iso639a.html
        if ((*it).size() == 2 )
            d->languageBtn->insertLanguage(*it);
    }

    d->languageBtn->setWhatsThis(i18n("<p>Select here the language of content."));

    // --------------------------------------------------------

    d->priorityCheck = new MetadataCheckBox(i18n("Priority:"), this);
    d->priorityCB    = new QComboBox(this);
    d->priorityCB->insertItem(0, i18n("0: None"));
    d->priorityCB->insertItem(1, i18n("1: High"));
    d->priorityCB->insertItem(2, "2");
    d->priorityCB->insertItem(3, "3");
    d->priorityCB->insertItem(4, "4");
    d->priorityCB->insertItem(5, i18n("5: Normal"));
    d->priorityCB->insertItem(6, "6");
    d->priorityCB->insertItem(7, "7");
    d->priorityCB->insertItem(8, i18n("8: Low"));
    d->priorityCB->setWhatsThis(i18n("<p>Select here the editorial urgency of content."));

    // --------------------------------------------------------

    d->sceneEdit = new MultiValuesEdit(this, i18n("Scene:"), 
                       i18n("<p>Select here the scene type of content."));
 
    QStringList list2;
    for (XMPPropertiesPriv::SceneCodeMap::Iterator it = d->sceneCodeMap.begin();
         it != d->sceneCodeMap.end(); ++it)
        list2.append(QString("%1 - %2").arg(it.key()).arg(it.value()));
 
    d->sceneEdit->setData(list2);

    // --------------------------------------------------------

    d->objectTypeCheck    = new MetadataCheckBox(i18n("Type:"), this);
    d->objectTypeCB       = new QComboBox(this);
    d->objectTypeDescEdit = new KLineEdit(this);
    d->objectTypeDescEdit->setClearButtonShown(true);
    d->objectTypeCB->insertItem(0, i18n("News"));
    d->objectTypeCB->insertItem(1, i18n("Data"));
    d->objectTypeCB->insertItem(2, i18n("Advisory"));
    d->objectTypeCB->setWhatsThis(i18n("<p>Select here the editorial type of content."));
    d->objectTypeDescEdit->setWhatsThis(i18n("<p>Set here the editorial type description of content."));

    // --------------------------------------------------------

    d->objectAttribute = new ObjectAttributesEdit(this, false);

    // --------------------------------------------------------

    grid->addWidget(d->languageCheck, 0, 0, 1, 1);
    grid->addWidget(d->languageBtn, 0, 1, 1, 1);
    grid->addWidget(d->priorityCheck, 1, 0, 1, 1);
    grid->addWidget(d->priorityCB, 1, 1, 1, 1);
    grid->addWidget(d->sceneEdit, 2, 0, 1, 5);
    grid->addWidget(d->objectTypeCheck, 3, 0, 1, 1);
    grid->addWidget(d->objectTypeCB, 3, 1, 1, 1);
    grid->addWidget(d->objectTypeDescEdit, 3, 2, 1, 3);
    grid->addWidget(new KSeparator(Qt::Horizontal, this), 4, 0, 1, 5);
    grid->addWidget(d->objectAttribute, 5, 0, 1, 5);
    grid->setRowStretch(6, 10);                     
    grid->setColumnStretch(3, 10);                     
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());

    // --------------------------------------------------------

    connect(d->languageCheck, SIGNAL(toggled(bool)),
            d->languageBtn, SLOT(setEnabled(bool)));

    connect(d->priorityCheck, SIGNAL(toggled(bool)),
            d->priorityCB, SLOT(setEnabled(bool)));

    connect(d->objectTypeCheck, SIGNAL(toggled(bool)),
            d->objectTypeCB, SLOT(setEnabled(bool)));

    connect(d->objectTypeCheck, SIGNAL(toggled(bool)),
            d->objectTypeDescEdit, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->languageCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->priorityCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->sceneEdit, SIGNAL(signalModified()),
            this, SIGNAL(signalModified()));

    connect(d->objectTypeCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->objectAttribute, SIGNAL(signalModified()),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->languageBtn, SIGNAL(activated(const QString&)),
            this, SIGNAL(signalModified()));

    connect(d->priorityCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->objectTypeCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->objectTypeDescEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));
}

XMPProperties::~XMPProperties()
{
    delete d;
}

void XMPProperties::readMetadata(QByteArray& xmpData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setXmp(xmpData);

    int         val;
    QString     data;
    QStringList code, list;
    QString     dateStr, timeStr;
    KExiv2Iface::KExiv2::AltLangMap map;   

    d->languageCheck->setChecked(false);
    data = exiv2Iface.getXmpTagString("Xmp.dc.language", false);    
    if (!data.isNull())
    {
        if (d->languageBtn->contains(data))
        {
            d->languageBtn->setCurrentItem(data);
            d->languageCheck->setChecked(true);
        }
        else
            d->languageCheck->setValid(false);
    }
    d->languageBtn->setEnabled(d->languageCheck->isChecked());
    
    d->priorityCB->setCurrentIndex(0);
    d->priorityCheck->setChecked(false);
    data = exiv2Iface.getXmpTagString("Xmp.photoshop.Urgency", false);    
    if (!data.isNull())
    {
        val = data.toInt(); 
        if (val >= 0 && val <= 8)
        {
            d->priorityCB->setCurrentIndex(val);
            d->priorityCheck->setChecked(true);
        }
        else
            d->priorityCheck->setValid(false);
    }
    d->priorityCB->setEnabled(d->priorityCheck->isChecked());

    code = exiv2Iface.getXmpTagStringBag("Xmp.iptc.Scene", false);
    for (QStringList::Iterator it = code.begin(); it != code.end(); ++it)
    {
        QStringList data = d->sceneEdit->getData();
        QStringList::Iterator it2;
        for (it2 = data.begin(); it2 != data.end(); ++it2)        
        {
            if ((*it2).left(6) == (*it))
            {
                list.append(*it2);
                break;
            }
        }
        if (it2 == data.end())  
            d->sceneEdit->setValid(false);
    }
    d->sceneEdit->setValues(list);
/*
    d->objectTypeCB->setCurrentIndex(0);
    d->objectTypeDescEdit->clear();
    d->objectTypeCheck->setChecked(false);
    data = exiv2Iface.getXmpTagString("Xmp.dc.Type", false);    
    if (!data.isNull())
    {
        QString typeSec = data.section(":", 0, 0);
        if (!typeSec.isEmpty())
        {
            int type = typeSec.toInt()-1;
            if (type >= 0 && type < 3)
            {
                d->objectTypeCB->setCurrentIndex(type);
                d->objectTypeDescEdit->setText(data.section(":", -1));
                d->objectTypeCheck->setChecked(true);
            }
            else
                d->objectTypeCheck->setValid(false);
        }
    }
    d->objectTypeCB->setEnabled(d->objectTypeCheck->isChecked());
    d->objectTypeDescEdit->setEnabled(d->objectTypeCheck->isChecked());

    list = exiv2Iface.getXmpTagsStringList("Xmp.iptc.IntellectualGenre", false);    
    d->objectAttribute->setValues(list);
*/
    blockSignals(false);
}

void XMPProperties::applyMetadata(QByteArray& xmpData)
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setXmp(xmpData);

    if (d->languageCheck->isChecked())
        exiv2Iface.setXmpTagString("Xmp.dc.language", d->languageBtn->current());
    else if (d->languageCheck->isValid())
        exiv2Iface.removeXmpTag("Xmp.dc.language");

    if (d->priorityCheck->isChecked())
        exiv2Iface.setXmpTagString("Xmp.photoshop.Urgency", QString::number(d->priorityCB->currentIndex()));
    else if (d->priorityCheck->isValid())
        exiv2Iface.removeXmpTag("Xmp.photoshop.Urgency");

    QStringList oldList, newList;
    if (d->sceneEdit->getValues(oldList, newList))
    {
        QStringList newCode;

        for (QStringList::Iterator it2 = newList.begin(); it2 != newList.end(); ++it2)
            newCode.append((*it2).left(6));

        exiv2Iface.setXmpTagStringBag("Xmp.iptc.Scene", newCode, false);
    }
    else
    {
        exiv2Iface.removeXmpTag("Xmp.iptc.Scene");
    }
/*
    if (d->objectTypeCheck->isChecked())
    {
        QString objectType;
        objectType.sprintf("%2d", d->objectTypeCB->currentIndex()+1);
        objectType.append(QString(":%1").arg(d->objectTypeDescEdit->text()));
        exiv2Iface.setXmpTagString("Xmp.dc.Type", objectType);
    }
    else if (d->objectTypeCheck->isValid())
        exiv2Iface.removeIptcTag("Xmp.dc.Type");

    QStringList oldList, newList;
    if (d->objectAttribute->getValues(oldList, newList))
        exiv2Iface.setXmpTagsStringList("Xmp.iptc.IntellectualGenre", 64, oldList, newList);
    else if (d->objectAttribute->isValid())
        exiv2Iface.removeXmpTag("Xmp.iptc.IntellectualGenre");
*/
    exiv2Iface.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));

    xmpData = exiv2Iface.getXmp();
}

}  // namespace KIPIMetadataEditPlugin
