/* ============================================================
 *
 * Date        : 2010-08-06
 * Description : A widget to show details about images
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
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

#include "gpsimagedetails.h"
#include "gpsimagedetails.moc"

// Qt includes

#include <QCheckBox>
#include <QFormLayout>
#include <QPushButton>
#include <QVBoxLayout>

// KDE includes

#include <kconfiggroup.h>
#include <klineedit.h>
#include <klocale.h>
#include <kseparator.h>

// local includes

#include "previewmanager.h"
#include "kipiimagemodel.h"
#include "gpsundocommand.h"


namespace KIPIGPSSyncPlugin
{

class GPSImageDetails::GPSImageDetailsPrivate
{
public:
    GPSImageDetailsPrivate()
    : externalEnabledState(true)
    {
    }

    KipiImageModel           *imageModel;
    KIPIPlugins::PreviewManager *previewManager;

    QCheckBox *cbCoordinates;
    QCheckBox *cbAltitude;
    KLineEdit *leAltitude;

    QPushButton *pbApply;

    QPersistentModelIndex imageIndex;
    GPSDataContainer infoOld;
    bool externalEnabledState;
};

GPSImageDetails::GPSImageDetails(QWidget* const parent, KipiImageModel* const imageModel, const int marginHint, const int spacingHint)
: QWidget(parent), d(new GPSImageDetailsPrivate())
{
    d->imageModel = imageModel;

    // TODO: subscribe to changes in the model to update the display

    QVBoxLayout* const layout1 = new QVBoxLayout(this);

    /* *** *** *** */

    QFormLayout* const formLayout = new QFormLayout();
    layout1->addLayout(formLayout);

    d->cbCoordinates = new QCheckBox(i18n("Coordinates"), this);
    formLayout->setWidget(formLayout->rowCount(), QFormLayout::LabelRole, d->cbCoordinates);

    d->cbAltitude = new QCheckBox(i18n("Altitude"), this);
    d->leAltitude = new KLineEdit(this);
    formLayout->addRow(d->cbAltitude, d->leAltitude);

    d->pbApply = new QPushButton(i18n("Apply"), this);
    formLayout->setWidget(formLayout->rowCount(), QFormLayout::SpanningRole, d->pbApply);

    /* *** *** *** */

    layout1->addWidget(new KSeparator(Qt::Horizontal, this));

    /* *** *** *** */

    d->previewManager = new KIPIPlugins::PreviewManager(this);
    d->previewManager->setMinimumSize(QSize(200, 200));
    layout1->addWidget(d->previewManager);

    /* *** *** *** */

    connect(d->cbCoordinates, SIGNAL(stateChanged(int)),
            this, SLOT(updateUIState()));

    connect(d->cbAltitude, SIGNAL(stateChanged(int)),
            this, SLOT(updateUIState()));

    updateUIState();
}

GPSImageDetails::~GPSImageDetails()
{
    delete d;
}

void GPSImageDetails::setUIEnabledExternal(const bool state)
{
    d->externalEnabledState = state;
    updateUIState();
}

void GPSImageDetails::saveSettingsToGroup(KConfigGroup* const group)
{
}

void GPSImageDetails::readSettingsFromGroup(const KConfigGroup* const group)
{
}

void GPSImageDetails::updateUIState()
{
    const bool externalEnabled = d->externalEnabledState && d->imageIndex.isValid();
    const bool haveCoordinates = d->cbCoordinates->isChecked();

    d->cbCoordinates->setEnabled(externalEnabled);

    /* altitude */
    d->cbAltitude->setEnabled(haveCoordinates&&externalEnabled);
    const bool haveAltitude = d->cbAltitude->isChecked();
    d->leAltitude->setEnabled(haveAltitude&&haveCoordinates&&externalEnabled);

    /* apply */
    d->pbApply->setEnabled(/*externalEnabled*/false);
}

void GPSImageDetails::slotSetCurrentImage(const QModelIndex& index)
{
    d->imageIndex = index;

    GPSDataContainer gpsData;

    if (index.isValid())
    {
        KipiImageItem* const item = d->imageModel->itemFromIndex(index);

        if (item)
        {
            d->previewManager->load(item->url().toLocalFile(), true);
            gpsData = item->gpsData();
        }
    }

    /* *** *** *** */

    d->cbCoordinates->setChecked(gpsData.hasCoordinates());
    if (gpsData.hasCoordinates())
    {
        const bool haveAltitude = gpsData.hasAltitude();
        d->cbAltitude->setChecked(haveAltitude);
        if (haveAltitude)
        {
            d->leAltitude->setText(gpsData.getCoordinates().altString());
        }
    }

    updateUIState();
}

} /* KIPIGPSSyncPlugin */

