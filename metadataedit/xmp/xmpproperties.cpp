/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-10-24
 * Description : XMP workflow status properties settings page.
 *
 * Copyright (C) 2007-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "xmpproperties.moc"

// Qt includes

#include <QCheckBox>
#include <QPushButton>
#include <QGridLayout>

// KDE includes

#include <kcombobox.h>
#include <kdialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <kseparator.h>
#include <ktextedit.h>
#include <kdebug.h>
#include <kconfiggroup.h>

// LibKDcraw includes

#include <libkdcraw/squeezedcombobox.h>

// Local includes

#include "altlangstringedit.h"
#include "metadatacheckbox.h"
#include "multivaluesedit.h"
#include "objectattributesedit.h"
#include "kpversion.h"
#include "kpmetadata.h"

using namespace KIPIPlugins;
using namespace KDcrawIface;

namespace KIPIMetadataEditPlugin
{

class XMPProperties::XMPPropertiesPriv
{
public:

    XMPPropertiesPriv()
    {
        priorityCB           = 0;
        objectTypeCB         = 0;
        priorityCheck        = 0;
        objectAttributeCheck = 0;
        sceneEdit            = 0;
        objectTypeEdit       = 0;
        objectAttributeEdit  = 0;
        objectAttributeCB    = 0;
        languageEdit         = 0;
        originalTransEdit    = 0;
        originalTransCheck   = 0;


        sceneCodeMap.insert( "010100", i18n("Headshot") );
        sceneCodeMap.insert( "010200", i18n("Half-length") );
        sceneCodeMap.insert( "010300", i18n("Full-length") );
        sceneCodeMap.insert( "010400", i18nc("Side view of a person", "Profile") );
        sceneCodeMap.insert( "010500", i18n("Rear view") );
        sceneCodeMap.insert( "010600", i18n("Single") );
        sceneCodeMap.insert( "010700", i18n("Couple") );
        sceneCodeMap.insert( "010800", i18n("Two") );
        sceneCodeMap.insert( "010900", i18nc("group of people", "Group") );
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

        typeCodeMap.insert( "Advisory",           i18n("Advisory") );
        typeCodeMap.insert( "Alert",              i18n("Alert") );
        typeCodeMap.insert( "Catalog",            i18n("Catalog") );
        typeCodeMap.insert( "Data",               i18n("Data") );
        typeCodeMap.insert( "Document",           i18nc("type is a document", "Document") );
        typeCodeMap.insert( "DTD",                i18n("DTD") );
        typeCodeMap.insert( "Maintenance",        i18n("Maintenance") );
        typeCodeMap.insert( "News",               i18n("News") );
        typeCodeMap.insert( "NewsManagementMode", i18n("News Management Mode") );
        typeCodeMap.insert( "Package",            i18n("Package") );
        typeCodeMap.insert( "Schema",             i18n("Schema") );
        typeCodeMap.insert( "Topic",              i18n("Topic") );
        typeCodeMap.insert( "TopicSet",           i18n("Topic Set") );

        // Fill language code map. inspired from KLanguageButton::loadAllLanguages()
        QStringList list = KGlobal::locale()->allLanguagesList();
        list.sort();
        for (QStringList::Iterator it = list.begin(); it != list.end(); ++it)
        {
            QString code = *it;

            // Only get all ISO 639 language code based on 2 characters
            // http://xml.coverpages.org/iso639a.html
            if (code.size() == 2 )
            {
                QString name = KGlobal::locale()->languageCodeToName(code);
                languageCodeMap.insert(code, name);
            }
        }
    }

    typedef QMap<QString, QString>  SceneCodeMap;
    typedef QMap<QString, QString>  TypeCodeMap;
    typedef QMap<QString, QString>  LanguageCodeMap;

    SceneCodeMap                    sceneCodeMap;
    TypeCodeMap                     typeCodeMap;
    LanguageCodeMap                 languageCodeMap;

    QCheckBox*                      originalTransCheck;

    KComboBox*                      priorityCB;
    KComboBox*                      objectTypeCB;

    KLineEdit*                      objectAttributeEdit;
    KLineEdit*                      originalTransEdit;

    MetadataCheckBox*               priorityCheck;
    MetadataCheckBox*               objectAttributeCheck;

    MultiValuesEdit*                sceneEdit;
    MultiValuesEdit*                objectTypeEdit;
    MultiValuesEdit*                languageEdit;

    SqueezedComboBox*               objectAttributeCB;
};

XMPProperties::XMPProperties(QWidget* const parent)
    : QWidget(parent), d(new XMPPropertiesPriv)
{
    QGridLayout* grid = new QGridLayout(this);

    // --------------------------------------------------------

    d->languageEdit = new MultiValuesEdit(this, i18n("Language:"),
                          i18n("Select here the language of content."));

    QStringList list;
    for (XMPPropertiesPriv::LanguageCodeMap::Iterator it = d->languageCodeMap.begin();
         it != d->languageCodeMap.end(); ++it)
        list.append(QString("%1 - %2").arg(it.key()).arg(it.value()));

    d->languageEdit->setData(list);

    // --------------------------------------------------------

    d->priorityCheck = new MetadataCheckBox(i18n("Priority:"), this);
    d->priorityCB    = new KComboBox(this);
    d->priorityCB->insertItem(0, i18nc("editorial urgency of content", "0: None"));
    d->priorityCB->insertItem(1, i18nc("editorial urgency of content", "1: High"));
    d->priorityCB->insertItem(2, "2");
    d->priorityCB->insertItem(3, "3");
    d->priorityCB->insertItem(4, "4");
    d->priorityCB->insertItem(5, i18nc("editorial urgency of content", "5: Normal"));
    d->priorityCB->insertItem(6, "6");
    d->priorityCB->insertItem(7, "7");
    d->priorityCB->insertItem(8, i18nc("editorial urgency of content", "8: Low"));
    d->priorityCB->insertItem(9, i18nc("editorial urgency of content", "9: User-defined"));
    d->priorityCB->setWhatsThis(i18n("Select here the editorial urgency of content."));

    // --------------------------------------------------------

    d->sceneEdit = new MultiValuesEdit(this, i18n("Scene:"),
                       i18n("Select here the scene type of the content."));

    QStringList list2;
    for (XMPPropertiesPriv::SceneCodeMap::Iterator it = d->sceneCodeMap.begin();
         it != d->sceneCodeMap.end(); ++it)
        list2.append(QString("%1 - %2").arg(it.key()).arg(it.value()));

    d->sceneEdit->setData(list2);

    // --------------------------------------------------------

    d->objectTypeEdit = new MultiValuesEdit(this, i18n("Type:"),
                            i18n("Select here the editorial type of the content."));

    QStringList list3;
    for (XMPPropertiesPriv::TypeCodeMap::Iterator it = d->typeCodeMap.begin();
         it != d->typeCodeMap.end(); ++it)
        list3.append(it.value());

    d->objectTypeEdit->setData(list3);

    // --------------------------------------------------------

    d->objectAttributeCheck = new MetadataCheckBox(i18n("Attribute:"), this);
    d->objectAttributeCB    = new SqueezedComboBox(this);
    d->objectAttributeEdit  = new KLineEdit(this);
    d->objectAttributeEdit->setClearButtonShown(true);
    d->objectAttributeEdit->setWhatsThis(i18n("Set here the editorial attribute description of the content."));

    d->objectAttributeCB->setWhatsThis(i18n("Select here the editorial attribute of the content."));
    d->objectAttributeCB->addSqueezedItem(QString("001 - ") + i18nc("editorial content attribute", "Current"));
    d->objectAttributeCB->addSqueezedItem(QString("002 - ") + i18nc("editorial content attribute", "Analysis"));
    d->objectAttributeCB->addSqueezedItem(QString("003 - ") + i18nc("editorial content attribute", "Archive material"));
    d->objectAttributeCB->addSqueezedItem(QString("004 - ") + i18nc("editorial content attribute", "Background"));
    d->objectAttributeCB->addSqueezedItem(QString("005 - ") + i18nc("editorial content attribute", "Feature"));
    d->objectAttributeCB->addSqueezedItem(QString("006 - ") + i18nc("editorial content attribute", "Forecast"));
    d->objectAttributeCB->addSqueezedItem(QString("007 - ") + i18nc("editorial content attribute", "History"));
    d->objectAttributeCB->addSqueezedItem(QString("008 - ") + i18nc("editorial content attribute", "Obituary"));
    d->objectAttributeCB->addSqueezedItem(QString("009 - ") + i18nc("editorial content attribute", "Opinion"));
    d->objectAttributeCB->addSqueezedItem(QString("010 - ") + i18nc("editorial content attribute", "Polls & Surveys"));
    d->objectAttributeCB->addSqueezedItem(QString("011 - ") + i18nc("editorial content attribute", "Profile"));
    d->objectAttributeCB->addSqueezedItem(QString("012 - ") + i18nc("editorial content attribute", "Results Listings & Table"));
    d->objectAttributeCB->addSqueezedItem(QString("013 - ") + i18nc("editorial content attribute", "Side bar & Supporting information"));
    d->objectAttributeCB->addSqueezedItem(QString("014 - ") + i18nc("editorial content attribute", "Summary"));
    d->objectAttributeCB->addSqueezedItem(QString("015 - ") + i18nc("editorial content attribute", "Transcript & Verbatim"));
    d->objectAttributeCB->addSqueezedItem(QString("016 - ") + i18nc("editorial content attribute", "Interview"));
    d->objectAttributeCB->addSqueezedItem(QString("017 - ") + i18nc("editorial content attribute", "From the Scene"));
    d->objectAttributeCB->addSqueezedItem(QString("018 - ") + i18nc("editorial content attribute", "Retrospective"));
    d->objectAttributeCB->addSqueezedItem(QString("019 - ") + i18nc("editorial content attribute", "Statistics"));
    d->objectAttributeCB->addSqueezedItem(QString("020 - ") + i18nc("editorial content attribute", "Update"));
    d->objectAttributeCB->addSqueezedItem(QString("021 - ") + i18nc("editorial content attribute", "Wrap-up"));
    d->objectAttributeCB->addSqueezedItem(QString("022 - ") + i18nc("editorial content attribute", "Press Release"));

    // --------------------------------------------------------

    d->originalTransCheck = new QCheckBox(i18n("Reference:"), this);
    d->originalTransEdit  = new KLineEdit(this);
    d->originalTransEdit->setClearButtonShown(true);
    d->originalTransEdit->setWhatsThis(i18n("Set here the original content transmission reference."));

    // --------------------------------------------------------

    grid->addWidget(d->languageEdit,                        0, 0, 1, 5);
    grid->addWidget(d->priorityCheck,                       1, 0, 1, 1);
    grid->addWidget(d->priorityCB,                          1, 1, 1, 1);
    grid->addWidget(d->sceneEdit,                           2, 0, 1, 5);
    grid->addWidget(d->objectTypeEdit,                      3, 0, 1, 5);
    grid->addWidget(new KSeparator(Qt::Horizontal, this),   4, 0, 1, 5);
    grid->addWidget(d->objectAttributeCheck,                5, 0, 1, 1);
    grid->addWidget(d->objectAttributeCB,                   5, 1, 1, 2);
    grid->addWidget(d->objectAttributeEdit,                 5, 3, 1, 2);
    grid->addWidget(new KSeparator(Qt::Horizontal, this),   6, 0, 1, 5);
    grid->addWidget(d->originalTransCheck,                  7, 0, 1, 1);
    grid->addWidget(d->originalTransEdit,                   7, 1, 1, 4);

    grid->setRowStretch(8, 10);
    grid->setColumnStretch(4, 10);
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());

    // --------------------------------------------------------

    connect(d->priorityCheck, SIGNAL(toggled(bool)),
            d->priorityCB, SLOT(setEnabled(bool)));

    connect(d->objectAttributeCheck, SIGNAL(toggled(bool)),
            d->objectAttributeCB, SLOT(setEnabled(bool)));

    connect(d->objectAttributeCheck, SIGNAL(toggled(bool)),
            d->objectAttributeEdit, SLOT(setEnabled(bool)));

    connect(d->originalTransCheck, SIGNAL(toggled(bool)),
            d->originalTransEdit, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->languageEdit, SIGNAL(signalModified()),
            this, SIGNAL(signalModified()));

    connect(d->priorityCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->sceneEdit, SIGNAL(signalModified()),
            this, SIGNAL(signalModified()));

    connect(d->objectTypeEdit, SIGNAL(signalModified()),
            this, SIGNAL(signalModified()));

    connect(d->objectAttributeCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->originalTransCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->priorityCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->objectAttributeCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->objectAttributeEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(signalModified()));

    connect(d->originalTransEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(signalModified()));
}

XMPProperties::~XMPProperties()
{
    delete d;
}

void XMPProperties::readMetadata(QByteArray& xmpData)
{
    blockSignals(true);
    KPMetadata meta;
    meta.setXmp(xmpData);

    int         val;
    QString     data;
    QStringList code, list, list2;
    QString     dateStr, timeStr;
    KPMetadata::AltLangMap map;

    // ---------------------------------------------------------------

    code = meta.getXmpTagStringBag("Xmp.dc.language", false);
    for (QStringList::Iterator it = code.begin(); it != code.end(); ++it)
    {
        QStringList data = d->languageEdit->getData();
        QStringList::Iterator it2;
        for (it2 = data.begin(); it2 != data.end(); ++it2)
        {
            if ((*it2).left(2) == (*it))
            {
                list.append(*it2);
                break;
            }
        }
        if (it2 == data.end())
            d->languageEdit->setValid(false);
    }
    d->languageEdit->setValues(list);

    // ---------------------------------------------------------------

    d->priorityCB->setCurrentIndex(0);
    d->priorityCheck->setChecked(false);
    data = meta.getXmpTagString("Xmp.photoshop.Urgency", false);
    if (!data.isNull())
    {
        val = data.toInt();
        if (val >= 0 && val <= 9)
        {
            d->priorityCB->setCurrentIndex(val);
            d->priorityCheck->setChecked(true);
        }
        else
            d->priorityCheck->setValid(false);
    }
    d->priorityCB->setEnabled(d->priorityCheck->isChecked());

    // ---------------------------------------------------------------

    code = meta.getXmpTagStringBag("Xmp.iptc.Scene", false);
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

    // ---------------------------------------------------------------

    code = meta.getXmpTagStringBag("Xmp.dc.type", false);
    for (QStringList::Iterator it3 = code.begin(); it3 != code.end(); ++it3)
    {
        QStringList data = d->objectTypeEdit->getData();
        QStringList::Iterator it4;
        for (it4 = data.begin(); it4 != data.end(); ++it4)
        {
            if ((*it4) == (*it3))
            {
                list2.append(*it4);
                break;
            }
        }
        if (it4 == data.end())
            d->objectTypeEdit->setValid(false);
    }
    d->objectTypeEdit->setValues(list2);

    // ---------------------------------------------------------------

    d->objectAttributeCB->setCurrentIndex(0);
    d->objectAttributeEdit->clear();
    d->objectAttributeCheck->setChecked(false);
    data = meta.getXmpTagString("Xmp.iptc.IntellectualGenre", false);
    if (!data.isNull())
    {
        QString attrSec = data.section(':', 0, 0);
        if (!attrSec.isEmpty())
        {
            int attr = attrSec.toInt()-1;
            if (attr >= 0 && attr < 23)
            {
                d->objectAttributeCB->setCurrentIndex(attr);
                d->objectAttributeEdit->setText(data.section(':', -1));
                d->objectAttributeCheck->setChecked(true);
            }
            else
                d->objectAttributeCheck->setValid(false);
        }
    }
    d->objectAttributeCB->setEnabled(d->objectAttributeCheck->isChecked());
    d->objectAttributeEdit->setEnabled(d->objectAttributeCheck->isChecked());

    // ---------------------------------------------------------------

    d->originalTransEdit->clear();
    d->originalTransCheck->setChecked(false);
    data = meta.getXmpTagString("Xmp.photoshop.TransmissionReference", false);
    if (!data.isNull())
    {
        d->originalTransEdit->setText(data);
        d->originalTransCheck->setChecked(true);
    }
    d->originalTransEdit->setEnabled(d->originalTransCheck->isChecked());

    // ---------------------------------------------------------------

    blockSignals(false);
}

void XMPProperties::applyMetadata(QByteArray& xmpData)
{
    QStringList oldList, newList;
    KPMetadata meta;
    meta.setXmp(xmpData);

    // ---------------------------------------------------------------

    if (d->languageEdit->getValues(oldList, newList))
    {
        QStringList newCode;

        for (QStringList::Iterator it2 = newList.begin(); it2 != newList.end(); ++it2)
            newCode.append((*it2).left(2));

        meta.setXmpTagStringBag("Xmp.dc.language", newCode, false);
    }
    else
    {
        meta.removeXmpTag("Xmp.dc.language");
    }

    // ---------------------------------------------------------------

    if (d->priorityCheck->isChecked())
        meta.setXmpTagString("Xmp.photoshop.Urgency", QString::number(d->priorityCB->currentIndex()));
    else if (d->priorityCheck->isValid())
        meta.removeXmpTag("Xmp.photoshop.Urgency");

    // ---------------------------------------------------------------

    if (d->sceneEdit->getValues(oldList, newList))
    {
        QStringList newCode;

        for (QStringList::Iterator it2 = newList.begin(); it2 != newList.end(); ++it2)
            newCode.append((*it2).left(6));

        meta.setXmpTagStringBag("Xmp.iptc.Scene", newCode, false);
    }
    else
    {
        meta.removeXmpTag("Xmp.iptc.Scene");
    }

    // ---------------------------------------------------------------

    if (d->objectTypeEdit->getValues(oldList, newList))
        meta.setXmpTagStringBag("Xmp.dc.type", newList, false);
    else
        meta.removeXmpTag("Xmp.dc.type");

    // ---------------------------------------------------------------

    if (d->objectAttributeCheck->isChecked())
    {
        QString objectAttribute;
        objectAttribute.sprintf("%3d", d->objectAttributeCB->currentIndex()+1);
        objectAttribute.append(QString(":%1").arg(d->objectAttributeEdit->text()));
        meta.setXmpTagString("Xmp.iptc.IntellectualGenre", objectAttribute);
    }
    else if (d->objectAttributeCheck->isValid())
    {
        meta.removeXmpTag("Xmp.iptc.IntellectualGenre");
    }

    // ---------------------------------------------------------------

    if (d->originalTransCheck->isChecked())
        meta.setXmpTagString("Xmp.photoshop.TransmissionReference", d->originalTransEdit->text());
    else
        meta.removeXmpTag("Xmp.photoshop.TransmissionReference");

    // ---------------------------------------------------------------

    meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));

    xmpData = meta.getXmp();
}

}  // namespace KIPIMetadataEditPlugin
