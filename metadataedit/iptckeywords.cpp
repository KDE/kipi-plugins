/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-15
 * Description : IPTC keywords settings page.
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "iptckeywords.h"
#include "iptckeywords.moc"

// Qt includes

#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QValidator>
#include <QGridLayout>

// KDE includes

#include <kdialog.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klistwidget.h>
#include <klocale.h>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

namespace KIPIMetadataEditPlugin
{

class IPTCKeywordsPriv
{
public:

    IPTCKeywordsPriv()
    {
        addKeywordButton = 0;
        delKeywordButton = 0;
        repKeywordButton = 0;
        keywordsBox      = 0;
        keywordsCheck    = 0;
        keywordEdit      = 0;
    }

    QStringList  oldKeywords;

    QPushButton *addKeywordButton;
    QPushButton *delKeywordButton;
    QPushButton *repKeywordButton;

    QCheckBox   *keywordsCheck;

    KLineEdit   *keywordEdit;

    KListWidget *keywordsBox;
};

IPTCKeywords::IPTCKeywords(QWidget* parent)
            : QWidget(parent), d(new IPTCKeywordsPriv)
{
    QGridLayout *grid = new QGridLayout(this);

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    d->keywordsCheck = new QCheckBox(i18n("Use information retrieval words:"), this);

    d->keywordEdit   = new KLineEdit(this);
    d->keywordEdit->setClearButtonShown(true);
    d->keywordEdit->setValidator(asciiValidator);
    d->keywordEdit->setMaxLength(64);
    d->keywordEdit->setWhatsThis(i18n("Enter here a new keyword. "
                                      "This field is limited to 64 ASCII characters."));

    d->keywordsBox   = new KListWidget(this);
    d->keywordsBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    d->addKeywordButton = new QPushButton( i18n("&Add"), this);
    d->delKeywordButton = new QPushButton( i18n("&Delete"), this);
    d->repKeywordButton = new QPushButton( i18n("&Replace"), this);
    d->addKeywordButton->setIcon(SmallIcon("list-add"));
    d->delKeywordButton->setIcon(SmallIcon("edit-delete"));
    d->repKeywordButton->setIcon(SmallIcon("view-refresh"));
    d->delKeywordButton->setEnabled(false);
    d->repKeywordButton->setEnabled(false);

    // --------------------------------------------------------

    QLabel *note = new QLabel(i18n("<b>Note: "
                 "<b><a href='http://en.wikipedia.org/wiki/IPTC'>IPTC</a></b> "
                 "text tags only support the printable "
                 "<b><a href='http://en.wikipedia.org/wiki/Ascii'>ASCII</a></b> "
                 "characters and limit string sizes. "
                 "Use contextual help for details.</b>"), this);
    note->setMaximumWidth(150);
    note->setOpenExternalLinks(true);
    note->setWordWrap(true);
    note->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    // --------------------------------------------------------

    grid->setAlignment( Qt::AlignTop );
    grid->addWidget(d->keywordsCheck,       0, 0, 1, 2 );
    grid->addWidget(d->keywordEdit,         1, 0, 1, 1);
    grid->addWidget(d->keywordsBox,         2, 0, 5, 1);
    grid->addWidget(d->addKeywordButton,    2, 1, 1, 1);
    grid->addWidget(d->delKeywordButton,    3, 1, 1, 1);
    grid->addWidget(d->repKeywordButton,    4, 1, 1, 1);
    grid->addWidget(note,                   5, 1, 1, 1);
    grid->setColumnStretch(0, 10);
    grid->setRowStretch(6, 10);
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());

    // --------------------------------------------------------

    connect(d->keywordsBox, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotKeywordSelectionChanged()));

    connect(d->addKeywordButton, SIGNAL(clicked()),
            this, SLOT(slotAddKeyword()));

    connect(d->delKeywordButton, SIGNAL(clicked()),
            this, SLOT(slotDelKeyword()));

    connect(d->repKeywordButton, SIGNAL(clicked()),
            this, SLOT(slotRepKeyword()));

    // --------------------------------------------------------

    connect(d->keywordsCheck, SIGNAL(toggled(bool)),
            d->keywordEdit, SLOT(setEnabled(bool)));

    connect(d->keywordsCheck, SIGNAL(toggled(bool)),
            d->addKeywordButton, SLOT(setEnabled(bool)));

    connect(d->keywordsCheck, SIGNAL(toggled(bool)),
            d->delKeywordButton, SLOT(setEnabled(bool)));

    connect(d->keywordsCheck, SIGNAL(toggled(bool)),
            d->repKeywordButton, SLOT(setEnabled(bool)));

    connect(d->keywordsCheck, SIGNAL(toggled(bool)),
            d->keywordsBox, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->keywordsCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->addKeywordButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));

    connect(d->delKeywordButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));

    connect(d->repKeywordButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));
}

IPTCKeywords::~IPTCKeywords()
{
    delete d;
}

void IPTCKeywords::slotDelKeyword()
{
    QListWidgetItem *item = d->keywordsBox->currentItem();
    if (!item) return;
    d->keywordsBox->takeItem(d->keywordsBox->row(item));
    delete item;
}

void IPTCKeywords::slotRepKeyword()
{
    QString newKeyword = d->keywordEdit->text();
    if (newKeyword.isEmpty()) return;

    if (!d->keywordsBox->selectedItems().isEmpty())
    {
        d->keywordsBox->selectedItems()[0]->setText(newKeyword);
        d->keywordEdit->clear();
    }
}

void IPTCKeywords::slotKeywordSelectionChanged()
{
    if (!d->keywordsBox->selectedItems().isEmpty())
    {
        d->keywordEdit->setText(d->keywordsBox->selectedItems()[0]->text());
        d->delKeywordButton->setEnabled(true);
        d->repKeywordButton->setEnabled(true);
    }
    else
    {
        d->delKeywordButton->setEnabled(false);
        d->repKeywordButton->setEnabled(false);
    }
}

void IPTCKeywords::slotAddKeyword()
{
    QString newKeyword = d->keywordEdit->text();
    if (newKeyword.isEmpty()) return;

    bool found = false;
    for (int i = 0 ; i < d->keywordsBox->count(); i++)
    {
        QListWidgetItem *item = d->keywordsBox->item(i);
        if (newKeyword == item->text())
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        d->keywordsBox->insertItem(d->keywordsBox->count(), newKeyword);
        d->keywordEdit->clear();
    }
}

void IPTCKeywords::readMetadata(QByteArray& iptcData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    d->oldKeywords = exiv2Iface.getIptcKeywords();

    d->keywordsBox->clear();
    d->keywordsCheck->setChecked(false);
    if (!d->oldKeywords.isEmpty())
    {
        d->keywordsBox->insertItems(0, d->oldKeywords);
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
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    QStringList newKeywords;

    for (int i = 0 ; i < d->keywordsBox->count(); i++)
    {
        QListWidgetItem *item = d->keywordsBox->item(i);
        newKeywords.append(item->text());
    }

    if (d->keywordsCheck->isChecked())
        exiv2Iface.setIptcKeywords(d->oldKeywords, newKeywords);
    else
        exiv2Iface.setIptcKeywords(d->oldKeywords, QStringList());

    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin
