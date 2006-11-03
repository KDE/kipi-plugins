/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-15
 * Description : IPTC keywords settings page.
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

// QT includes.

#include <qlayout.h>
#include <qlabel.h>
#include <qwhatsthis.h>
#include <qvalidator.h>
#include <qcheckbox.h>
#include <qpushbutton.h>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <klistbox.h>
#include <klineedit.h>

// Local includes.

#include "exiv2iface.h"
#include "iptckeywords.h"
#include "iptckeywords.moc"

namespace KIPIMetadataEditPlugin
{

class IPTCKeywordsPriv
{
public:

    IPTCKeywordsPriv()
    {
        addKeywordButton = 0;
        delKeywordButton = 0;
        keywordsBox      = 0;
        keywordsCheck    = 0;
        keywordEdit      = 0;
    }

    QStringList  oldKeywords;

    QPushButton *addKeywordButton;
    QPushButton *delKeywordButton;

    QCheckBox   *keywordsCheck;

    KLineEdit   *keywordEdit;

    KListBox    *keywordsBox;
};

IPTCKeywords::IPTCKeywords(QWidget* parent)
            : QWidget(parent)
{
    d = new IPTCKeywordsPriv;
    QGridLayout *grid = new QGridLayout(parent, 5, 2, 0, KDialog::spacingHint());
    grid->setAlignment( Qt::AlignTop );

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    d->keywordsCheck = new QCheckBox(i18n("Use information retrieval words:"), parent);    

    d->keywordEdit   = new KLineEdit(parent);
    d->keywordEdit->setValidator(asciiValidator);
    d->keywordEdit->setMaxLength(64);
    QWhatsThis::add(d->keywordEdit, i18n("<p>Enter here a new keyword. "
                    "This field is limited to 64 ASCII characters."));

    d->keywordsBox   = new KListBox(parent);
    d->keywordsBox->setVScrollBarMode(QScrollView::AlwaysOn);
    
    d->addKeywordButton = new QPushButton( i18n("&Add"), parent);
    d->delKeywordButton = new QPushButton( i18n("&Delete"), parent);
    d->delKeywordButton->setEnabled(false);

    grid->addMultiCellWidget(d->keywordsCheck, 0, 0, 0, 1);
    grid->addMultiCellWidget(d->keywordEdit, 1, 1, 0, 0);
    grid->addMultiCellWidget(d->keywordsBox, 2, 5, 0, 0);
    grid->addMultiCellWidget(d->addKeywordButton, 2, 2, 1, 1);
    grid->addMultiCellWidget(d->delKeywordButton, 3, 3, 1, 1);

    // --------------------------------------------------------

    QLabel *iptcNote = new QLabel(i18n("<b>Note: IPTC text tags only support printable "
                                       "ASCII characters set.</b>"), parent);
    grid->addMultiCellWidget(iptcNote, 4, 4, 1, 1);
    grid->setColStretch(0, 10);                     
    grid->setRowStretch(5, 10);      
                                         
    // --------------------------------------------------------

    connect(d->keywordsBox, SIGNAL(selectionChanged()),
            this, SLOT(slotKeywordSelectionChanged()));
    
    connect(d->addKeywordButton, SIGNAL(clicked()),
            this, SLOT(slotAddKeyword()));
    
    connect(d->delKeywordButton, SIGNAL(clicked()),
            this, SLOT(slotDelKeyword()));

    // --------------------------------------------------------

    connect(d->keywordsCheck, SIGNAL(toggled(bool)),
            d->keywordEdit, SLOT(setEnabled(bool)));

    connect(d->keywordsCheck, SIGNAL(toggled(bool)),
            d->keywordsBox, SLOT(setEnabled(bool)));

    connect(d->keywordsCheck, SIGNAL(toggled(bool)),
            d->addKeywordButton, SLOT(setEnabled(bool)));

    connect(d->keywordsCheck, SIGNAL(toggled(bool)),
            d->delKeywordButton, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->keywordsCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->addKeywordButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));
    
    connect(d->delKeywordButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));
}

IPTCKeywords::~IPTCKeywords()
{
    delete d;
}

void IPTCKeywords::slotDelKeyword()
{
    int index = d->keywordsBox->currentItem();
    if (index == -1)
        return;

    QListBoxItem* item = d->keywordsBox->item(index);
    if (!item) return;
    delete item;
}

void IPTCKeywords::slotKeywordSelectionChanged()
{
    if (d->keywordsBox->currentItem() != -1)
        d->delKeywordButton->setEnabled(true);
    else
        d->delKeywordButton->setEnabled(false);
}

void IPTCKeywords::slotAddKeyword()
{
    QString newKeyword = d->keywordEdit->text();
    if (newKeyword.isEmpty()) return;

    bool found = false;
    for (QListBoxItem *item = d->keywordsBox->firstItem();
         item; item = item->next()) 
    {
        if (newKeyword == item->text()) 
        {
            found = true;
            break;
        }
    }

    if (!found)
        d->keywordsBox->insertItem(newKeyword);
}

void IPTCKeywords::readMetadata(QByteArray& iptcData)
{
    blockSignals(true);
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    d->oldKeywords = exiv2Iface.getImageKeywords();

    if (!d->oldKeywords.isEmpty())
    {
        d->keywordsBox->insertStringList(d->oldKeywords);
        d->keywordsCheck->setChecked(true);
    }
    d->keywordEdit->setEnabled(d->keywordsCheck->isChecked());
    d->keywordsBox->setEnabled(d->keywordsCheck->isChecked());
    d->addKeywordButton->setEnabled(d->keywordsCheck->isChecked());
    d->delKeywordButton->setEnabled(d->keywordsCheck->isChecked());

    blockSignals(false);
}

void IPTCKeywords::applyMetadata(QByteArray& iptcData)
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    QStringList newKeywords;    

    for (QListBoxItem *item = d->keywordsBox->firstItem();
         item; item = item->next()) 
        newKeywords.append(item->text());

    if (d->keywordsCheck->isChecked())
        exiv2Iface.setImageKeywords(d->oldKeywords, newKeywords);
    else
        exiv2Iface.setImageKeywords(d->oldKeywords, QStringList());

    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin

