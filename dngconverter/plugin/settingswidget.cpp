/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-24
 * Description : save settings widgets
 *
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "settingswidget.moc"

// Qt includes

#include <QButtonGroup>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>

// KDE includes

#include <kcombobox.h>
#include <kdialog.h>
#include <klocale.h>
#include <kseparator.h>
#include <ktoolinvocation.h>
#include <kurllabel.h>
#include <kiconloader.h>

// Libkdcraw includes

#include <libkdcraw/rcombobox.h>

// Libkipi includes

#include <libkipi/interface.h>
#include <libkipi/pluginloader.h>

// Local includes

#include "dngwriter.h"
#include "kphostsettings.h"

using namespace DNGIface;
using namespace KDcrawIface;
using namespace KIPIPlugins;

namespace KIPIDNGConverterPlugin
{

class SettingsWidget::Private
{
public:

    Private()
    {
        conflictLabel          = 0;
        conflictButtonGroup    = 0;
        overwriteButton        = 0;
        promptButton           = 0;
        compressLossLess       = 0;
        previewModeCB          = 0;
        previewModeLabel       = 0;
        backupOriginalRawFile  = 0;
        updateFileDate         = 0;
        iface                  = 0;
        PluginLoader* const pl = PluginLoader::instance();

        if (pl)
        {
            iface = pl->interface();
        }
    }

    QLabel*       conflictLabel;
    QLabel*       previewModeLabel;

    QButtonGroup* conflictButtonGroup;

    QRadioButton* overwriteButton;
    QRadioButton* promptButton;

    QCheckBox*    compressLossLess;
    QCheckBox*    backupOriginalRawFile;
    QCheckBox*    updateFileDate;

    RComboBox*    previewModeCB;

    Interface*    iface;
};

SettingsWidget::SettingsWidget(QWidget* const parent)
    : QWidget(parent), d(new Private)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QGridLayout* const settingsBoxLayout = new QGridLayout(this);

    KUrlLabel* const dngLogoLabel        = new KUrlLabel(this);
    dngLogoLabel->setText(QString());
    dngLogoLabel->setUrl("http://en.wikipedia.org/wiki/Digital_Negative_(file_format)");
    dngLogoLabel->setPixmap(KIconLoader::global()->loadIcon("kipi-dngconverter", KIconLoader::NoGroup, 96));
    dngLogoLabel->setAlignment(Qt::AlignRight);

    // ------------------------------------------------------------------------

    d->backupOriginalRawFile = new QCheckBox(i18n("Embed Original File"), this);

    d->compressLossLess      = new QCheckBox(i18n("Lossless Compression"), this);

    // ------------------------------------------------------------------------

    d->updateFileDate        = new QCheckBox(i18n("Update File Modification Date"), this);
    d->updateFileDate->setWhatsThis(i18n("Sets the file modification date "
                                         "to the creation date provided in the image metadata."));

    if (d->iface)
    {
        // If plugin run from KIPI host, use file time-stamp host settings instead.
        d->updateFileDate->hide();
        KPHostSettings hSettings;
        d->updateFileDate->setChecked(hSettings.metadataSettings().updateFileTimeStamp);
    }

    // ------------------------------------------------------------------------

    d->previewModeLabel = new QLabel(i18n("JPEG Preview:"), this);
    d->previewModeCB    = new RComboBox(this);
    d->previewModeCB->insertItem(DNGWriter::NONE,     i18nc("embedded preview type in dng file", "None"));
    d->previewModeCB->insertItem(DNGWriter::MEDIUM,   i18nc("embedded preview type in dng file", "Medium"));
    d->previewModeCB->insertItem(DNGWriter::FULLSIZE, i18nc("embedded preview type in dng file", "Full size"));
    d->previewModeCB->setDefaultIndex(DNGWriter::MEDIUM);

    // ------------------------------------------------------------------------

    KSeparator* const line     = new KSeparator(Qt::Horizontal, this);
    d->conflictLabel           = new QLabel(i18n("If Target File Exists:"), this);
    QWidget* const conflictBox = new QWidget(this);
    QVBoxLayout* const vlay    = new QVBoxLayout(conflictBox);
    d->conflictButtonGroup     = new QButtonGroup(conflictBox);
    d->overwriteButton         = new QRadioButton(i18n("Overwrite automatically"), conflictBox);
    d->promptButton            = new QRadioButton(i18n("Store as a different name"), conflictBox);
    d->conflictButtonGroup->addButton(d->overwriteButton, OVERWRITE);
    d->conflictButtonGroup->addButton(d->promptButton,    DIFFNAME);
    d->conflictButtonGroup->setExclusive(true);
    d->overwriteButton->setChecked(true);

    vlay->setMargin(KDialog::spacingHint());
    vlay->setSpacing(KDialog::spacingHint());
    vlay->addWidget(d->overwriteButton);
    vlay->addWidget(d->promptButton);

    settingsBoxLayout->addWidget(dngLogoLabel,             0, 0, 1, 1);
    settingsBoxLayout->addWidget(d->backupOriginalRawFile, 1, 0, 1, 1);
    settingsBoxLayout->addWidget(d->compressLossLess,      2, 0, 1, 1);
    settingsBoxLayout->addWidget(d->updateFileDate,        3, 0, 1, 1);
    settingsBoxLayout->addWidget(d->previewModeLabel,      4 ,0, 1, 1);
    settingsBoxLayout->addWidget(d->previewModeCB,         5 ,0 ,1, 1);
    settingsBoxLayout->addWidget(line,                     6, 0, 1, 1);
    settingsBoxLayout->addWidget(d->conflictLabel,         7, 0, 1, 1);
    settingsBoxLayout->addWidget(conflictBox,              8, 0, 1, 1);
    settingsBoxLayout->setRowStretch(9, 10);
    settingsBoxLayout->setMargin(KDialog::spacingHint());
    settingsBoxLayout->setSpacing(KDialog::spacingHint());

    connect(dngLogoLabel, SIGNAL(leftClickedUrl(QString)),
            this, SLOT(processUrl(QString)));

    connect(d->conflictButtonGroup, SIGNAL(buttonClicked(int)),
            this, SIGNAL(buttonChanged(int)));
}

SettingsWidget::~SettingsWidget()
{
    delete d;
}

void SettingsWidget::processUrl(const QString& url)
{
    KToolInvocation::self()->invokeBrowser(url);
}

void SettingsWidget::setDefaultSettings()
{
    d->previewModeCB->slotReset();
    setCompressLossLess(true);
    setBackupOriginalRawFile(false);
    setConflictRule(OVERWRITE);
}

void SettingsWidget::setPreviewMode(int mode)
{
    d->previewModeCB->setCurrentIndex(mode);
}

int SettingsWidget::previewMode() const
{
    return d->previewModeCB->currentIndex();
}

void SettingsWidget::setCompressLossLess(bool b)
{
    d->compressLossLess->setChecked(b);
}

bool SettingsWidget::compressLossLess() const
{
    return d->compressLossLess->isChecked();
}

void SettingsWidget::setUpdateFileDate(bool b)
{
    d->updateFileDate->setChecked(b);
}

bool SettingsWidget::updateFileDate() const
{
    return d->updateFileDate->isChecked();
}

void SettingsWidget::setBackupOriginalRawFile(bool b)
{
    d->backupOriginalRawFile->setChecked(b);
}

bool SettingsWidget::backupOriginalRawFile() const
{
    return d->backupOriginalRawFile->isChecked();
}

SettingsWidget::ConflictRule SettingsWidget::conflictRule()
{
    return((ConflictRule)(d->conflictButtonGroup->checkedId()));
}

void SettingsWidget::setConflictRule(SettingsWidget::ConflictRule r)
{
    d->conflictButtonGroup->button((int)r)->setChecked(true);
}

} // namespace KIPIDNGConverterPlugin
