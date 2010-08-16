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

#include <limits.h>
#include <math.h>

// Qt includes

#include <QCheckBox>
#include <QDoubleValidator>
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
    : externalEnabledState(true),
      activeState(false),
      haveDelayedState(false)
    {
    }

    KipiImageModel           *imageModel;
    KIPIPlugins::PreviewManager *previewManager;

    QCheckBox *cbCoordinates;
    KLineEdit *leLatitude;
    KLineEdit *leLongitude;
    QCheckBox *cbAltitude;
    KLineEdit *leAltitude;
    QCheckBox *cbSpeed;
    KLineEdit *leSpeed;

    QPushButton *pbApply;

    QPersistentModelIndex imageIndex;
    GPSDataContainer infoOld;
    bool externalEnabledState;
    bool activeState;
    bool haveDelayedState;
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

    d->leLatitude = new KLineEdit(this);
    d->leLatitude->setValidator(new QDoubleValidator(-90.0, 90.0, 12, this));
    d->leLatitude->setClearButtonShown(true);
    formLayout->addRow(i18n("Latitude"), d->leLatitude);
    d->leLongitude = new KLineEdit(this);
    d->leLongitude->setValidator(new QDoubleValidator(-180.0, 180.0, 12, this));
    d->leLongitude->setClearButtonShown(true);
    formLayout->addRow(i18n("Longitude"), d->leLongitude);

    d->cbAltitude = new QCheckBox(i18n("Altitude"), this);
    d->leAltitude = new KLineEdit(this);
    d->leAltitude->setClearButtonShown(true);
    d->leAltitude->setValidator(new QDoubleValidator(this));
    formLayout->addRow(d->cbAltitude, d->leAltitude);

    d->cbSpeed = new QCheckBox(i18n("Speed"), this);
    d->leSpeed = new KLineEdit(this);
    d->leSpeed->setClearButtonShown(true);
    d->leSpeed->setValidator(new QDoubleValidator(0, HUGE_VAL, 12, this));
    formLayout->addRow(d->cbSpeed, d->leSpeed);

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

    connect(d->cbSpeed, SIGNAL(stateChanged(int)),
            this, SLOT(updateUIState()));

    connect(d->imageModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
            this, SLOT(slotModelDataChanged(const QModelIndex&, const QModelIndex&)));

    connect(d->pbApply, SIGNAL(clicked()),
            this, SLOT(slotApply()));

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

    d->leLatitude->setEnabled(haveCoordinates&&externalEnabled);
    d->leLongitude->setEnabled(haveCoordinates&&externalEnabled);

    /* altitude */
    d->cbAltitude->setEnabled(haveCoordinates&&externalEnabled);
    const bool haveAltitude = d->cbAltitude->isChecked();
    d->leAltitude->setEnabled(haveAltitude&&haveCoordinates&&externalEnabled);

    /* speed */
    d->cbSpeed->setEnabled(haveCoordinates&&externalEnabled);
    d->leSpeed->setEnabled(d->cbSpeed->isChecked()&&haveCoordinates&&externalEnabled);

    /* apply */
    d->pbApply->setEnabled(externalEnabled);
}

void GPSImageDetails::displayGPSDataContainer(const GPSDataContainer* const gpsData)
{
    d->cbAltitude->setChecked(false);
    d->cbSpeed->setChecked(false);
    d->leLatitude->clear();
    d->leLongitude->clear();
    d->leAltitude->clear();
    d->leSpeed->clear();

    d->cbCoordinates->setChecked(gpsData->hasCoordinates());
    if (gpsData->hasCoordinates())
    {
        d->leLatitude->setText(KGlobal::locale()->formatNumber(gpsData->getCoordinates().lat(), 12));
        d->leLongitude->setText(KGlobal::locale()->formatNumber(gpsData->getCoordinates().lon(), 12));

        const bool haveAltitude = gpsData->hasAltitude();
        d->cbAltitude->setChecked(haveAltitude);
        if (haveAltitude)
        {
            d->leAltitude->setText(KGlobal::locale()->formatNumber(gpsData->getCoordinates().alt(), 12));
        }

        const bool haveSpeed = gpsData->hasSpeed();
        d->cbSpeed->setChecked(haveSpeed);
        if (haveSpeed)
        {
            d->leSpeed->setText(KGlobal::locale()->formatNumber(gpsData->getSpeed(), 12));
        }
    }

    updateUIState();
}

void GPSImageDetails::slotSetCurrentImage(const QModelIndex& index)
{
    // TODO: slotSetActive may call this function with d->imageIndex as a parameter
    // since we get the index as a reference, we overwrite index when changing d->imageIndex
    QModelIndex indexCopy = index;
    d->imageIndex = indexCopy;

    if (!d->activeState)
    {
        d->haveDelayedState = true;
        return;
    }
    d->haveDelayedState = false;

    GPSDataContainer gpsData;

    if (index.isValid())
    {
        KipiImageItem* const item = d->imageModel->itemFromIndex(index);
        kDebug()<<item;

        if (item)
        {
            d->previewManager->load(item->url().toLocalFile(), true);
            gpsData = item->gpsData();
        }
    }

    /* *** *** *** */

    d->infoOld = gpsData;
    displayGPSDataContainer(&gpsData);
}

void GPSImageDetails::slotModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    if (!d->imageIndex.isValid())
        return;

    if ( (topLeft.row()<=d->imageIndex.row()&&bottomRight.row()>=d->imageIndex.row()) &&
        (topLeft.column()<=d->imageIndex.column()&&bottomRight.column()>=d->imageIndex.column()) )
    {
        if (!d->activeState)
        {
            d->haveDelayedState = true;
            return;
        }

        GPSDataContainer gpsData;
        KipiImageItem* const item = d->imageModel->itemFromIndex(d->imageIndex);
        if (item)
        {
            d->previewManager->load(item->url().toLocalFile(), true);
            gpsData = item->gpsData();
        }

        d->infoOld = gpsData;
        displayGPSDataContainer(&gpsData);
    }
}

void GPSImageDetails::slotApply()
{
    GPSDataContainer newData;

    if (d->cbCoordinates->isChecked())
    {
        const qreal lat = KGlobal::locale()->readNumber(d->leLatitude->text());
        const qreal lon = KGlobal::locale()->readNumber(d->leLongitude->text());
        newData.setCoordinates(KMap::GeoCoordinates(lat, lon));

        if (d->cbAltitude->isChecked())
        {
            const qreal alt = KGlobal::locale()->readNumber(d->leAltitude->text());
            newData.setAltitude(alt);
        }

        if (d->cbSpeed->isChecked())
        {
            const qreal speed = KGlobal::locale()->readNumber(d->leSpeed->text());
            newData.setSpeed(speed);
        }
    }

    KipiImageItem* const gpsItem = d->imageModel->itemFromIndex(d->imageIndex);

    GPSUndoCommand* const undoCommand = new GPSUndoCommand();

    GPSUndoCommand::UndoInfo undoInfo(d->imageIndex);
    undoInfo.readOldDataFromItem(gpsItem);
    gpsItem->setGPSData(newData);
    undoInfo.readNewDataFromItem(gpsItem);
    undoCommand->addUndoInfo(undoInfo);
    undoCommand->setText(i18n("Details changed"));
    emit(signalUndoCommand(undoCommand));
}

void GPSImageDetails::slotSetActive(const bool state)
{
    d->activeState = state;

    if (state&&d->haveDelayedState)
    {
        d->haveDelayedState = false;
        slotSetCurrentImage(d->imageIndex);
    }
}

} /* KIPIGPSSyncPlugin */

