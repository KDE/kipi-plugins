/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-09-13
 * Description : a widget to provide options to save image.
 *
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "kpsavesettingswidget.moc"

// Qt includes

#include <QButtonGroup>
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>

// KDE includes

#include <kcombobox.h>
#include <kdialog.h>
#include <klocale.h>

namespace KIPIPlugins
{

class KPSaveSettingsWidget::Private
{
public:

    Private()
    {
        formatLabel         = 0;
        conflictLabel       = 0;
        conflictButtonGroup = 0;
        formatComboBox      = 0;
        overwriteButton     = 0;
        promptButton        = 0;
        grid                = 0;
    }

    QLabel*       formatLabel;
    QLabel*       conflictLabel;

    QGridLayout*  grid;

    QButtonGroup* conflictButtonGroup;

    KComboBox*    formatComboBox;

    QRadioButton* overwriteButton;
    QRadioButton* promptButton;
};

KPSaveSettingsWidget::KPSaveSettingsWidget(QWidget* const parent)
    : QWidget(parent), d(new Private)
{
    setAttribute(Qt::WA_DeleteOnClose);

    d->grid           = new QGridLayout(this);
    d->formatLabel    = new QLabel(i18n("Output file format:"), this);
    d->formatComboBox = new KComboBox( this );
    d->formatComboBox->setWhatsThis(i18n("<p>Set the output file format to use here:</p>"
                                         "<p><b>JPEG</b>: output the processed image in JPEG format. "
                                         "This format will give smaller-sized files.</p>"
                                         "<p><b>Warning: Due to the destructive compression algorithm, "
                                         "JPEG is a lossy quality format.</b></p>"
                                         "<p><b>TIFF</b>: output the processed image in TIFF format. "
                                         "This generates large files, without "
                                         "losing quality. Image is compressed.</p>"
                                         "<p><b>PNG</b>: output the processed image in PNG format. "
                                         "This generates large files, without "
                                         "losing quality. Image is compressed.</p>"
                                         "<p><b>PPM</b>: output the processed image in PPM format. "
                                         "This generates the largest files, without "
                                         "losing quality. Image is not compressed.</p>"));
    slotPopulateImageFormat(false);

    d->conflictLabel       = new QLabel(i18n("If Target File Exists:"), this);
    QWidget* conflictBox   = new QWidget(this);
    QVBoxLayout* vlay      = new QVBoxLayout(conflictBox);
    d->conflictButtonGroup = new QButtonGroup(conflictBox);
    d->overwriteButton     = new QRadioButton(i18n("Overwrite automatically"), conflictBox);
    d->promptButton        = new QRadioButton(i18n("Open rename-file dialog"), conflictBox);
    d->conflictButtonGroup->addButton(d->overwriteButton, OVERWRITE);
    d->conflictButtonGroup->addButton(d->promptButton,    ASKTOUSER);
    d->conflictButtonGroup->setExclusive(true);
    d->overwriteButton->setChecked(true);

    vlay->setMargin(KDialog::spacingHint());
    vlay->setSpacing(KDialog::spacingHint());
    vlay->addWidget(d->overwriteButton);
    vlay->addWidget(d->promptButton);

    d->grid->addWidget(d->formatLabel,    0, 0, 1, 1);
    d->grid->addWidget(d->formatComboBox, 0, 1, 1, 1);
    d->grid->addWidget(d->conflictLabel,  1, 0, 1, 2);
    d->grid->addWidget(conflictBox,       2, 0, 1, 2);
    d->grid->setRowStretch(4, 10);
    d->grid->setMargin(KDialog::spacingHint());
    d->grid->setSpacing(KDialog::spacingHint());

    connect(d->formatComboBox, SIGNAL(activated(int)),
            this, SIGNAL(signalSaveFormatChanged()));

    connect(d->conflictButtonGroup, SIGNAL(buttonClicked(int)),
            this, SIGNAL(signalConflictButtonChanged(int)));
}

KPSaveSettingsWidget::~KPSaveSettingsWidget()
{
    delete d;
}

void KPSaveSettingsWidget::setCustomSettingsWidget(QWidget* const custom)
{
    d->grid->addWidget(custom, 3, 0, 1, 2);
}

void KPSaveSettingsWidget::resetToDefault()
{
    setFileFormat(OUTPUT_PNG);
    setConflictRule(OVERWRITE);
}

KPSaveSettingsWidget::OutputFormat KPSaveSettingsWidget::fileFormat() const
{
    return(OutputFormat)(d->formatComboBox->currentIndex());
}

void KPSaveSettingsWidget::setFileFormat(OutputFormat f)
{
    d->formatComboBox->setCurrentIndex((int)f);
}

void KPSaveSettingsWidget::setPromptButtonText(const QString& str)
{
    d->promptButton->setText(str);
}

KPSaveSettingsWidget::ConflictRule KPSaveSettingsWidget::conflictRule() const
{
    return((ConflictRule)(d->conflictButtonGroup->checkedId()));
}

void KPSaveSettingsWidget::setConflictRule(ConflictRule r)
{
    d->conflictButtonGroup->button((int)r)->setChecked(true);
}

void KPSaveSettingsWidget::readSettings(KConfigGroup& group)
{
    setFileFormat((KPSaveSettingsWidget::OutputFormat)group.readEntry("Output Format", (int)(KPSaveSettingsWidget::OUTPUT_PNG)));
    setConflictRule((KPSaveSettingsWidget::ConflictRule)group.readEntry("Conflict",    (int)(KPSaveSettingsWidget::OVERWRITE)));
}

void KPSaveSettingsWidget::writeSettings(KConfigGroup& group)
{
    group.writeEntry("Output Format", (int)fileFormat());
    group.writeEntry("Conflict",      (int)conflictRule());
}

void KPSaveSettingsWidget::slotPopulateImageFormat(bool sixteenBits)
{
    d->formatComboBox->clear();
    d->formatComboBox->insertItem( OUTPUT_PNG,  "PNG" );
    d->formatComboBox->insertItem( OUTPUT_TIFF, "TIFF" );

    if (!sixteenBits)
    {
        d->formatComboBox->insertItem( OUTPUT_JPEG, "JPEG" );
        d->formatComboBox->insertItem( OUTPUT_PPM,  "PPM" );
    }

    emit signalSaveFormatChanged();
}

QString KPSaveSettingsWidget::extension() const
{
    return extensionForFormat(fileFormat());
}

QString KPSaveSettingsWidget::typeMime() const
{
    QString mime;

    switch(fileFormat())
    {
        case OUTPUT_JPEG:
            mime = "image/jpeg";
            break;
        case OUTPUT_TIFF:
            mime = "image/tiff";
            break;
        case OUTPUT_PPM:
            mime = "image/ppm";
            break;
        case OUTPUT_PNG:
            mime = "image/png";
            break;
    }

    return mime;
}

QString KPSaveSettingsWidget::extensionForFormat(KPSaveSettingsWidget::OutputFormat format)
{
    QString ext;

    switch(format)
    {
        case OUTPUT_JPEG:
            ext = ".jpg";
            break;
        case OUTPUT_TIFF:
            ext = ".tif";
            break;
        case OUTPUT_PPM:
            ext = ".ppm";
            break;
        case OUTPUT_PNG:
            ext = ".png";
            break;
    }

    return ext;
}

} // namespace KIPIPlugins
