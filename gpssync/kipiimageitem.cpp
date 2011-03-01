/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.kipi-plugins.org">http://www.kipi-plugins.org</a>
 *
 * @date   2010-03-21
 * @brief  An item to hold information about an image.
 *
 * @author Copyright (C) 2010 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "kipiimageitem.h"

// Qt includes

#include <QBrush>
#include <QScopedPointer>

// KDE includes

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>

// LibKExiv2 includes

#include <libkexiv2/version.h>
#include <libkexiv2/kexiv2.h>

// local includes

#include "kipiimagemodel.h"

using namespace KExiv2Iface;

namespace KIPIGPSSyncPlugin
{

bool KExiv2SetExifXmpTagDataVariant(KExiv2Iface::KExiv2* const exiv2Iface, const char* const exifTagName, const char* const xmpTagName, const QVariant& value)
{
    bool success = exiv2Iface->setExifTagVariant(exifTagName, value);

    if (success)
    {
        /** @todo Here we save all data types as XMP Strings. Is that okay or do we have to store them as some other type?
         */
        switch (value.type())
        {
            case QVariant::Int:
            case QVariant::UInt:
            case QVariant::Bool:
            case QVariant::LongLong:
            case QVariant::ULongLong:
                success = exiv2Iface->setXmpTagString(xmpTagName, QString::number(value.toInt()));
                break;

            case QVariant::Double:
            {
                long num, den;
                exiv2Iface->convertToRationalSmallDenominator(value.toDouble(), &num, &den);
                success = exiv2Iface->setXmpTagString(xmpTagName, QString("%1/%2").arg(num).arg(den));
                break;
            }
            case QVariant::List:
            {
                long num = 0, den = 1;
                QList<QVariant> list = value.toList();
                if (list.size() >= 1)
                    num = list[0].toInt();
                if (list.size() >= 2)
                    den = list[1].toInt();
                success = exiv2Iface->setXmpTagString(xmpTagName, QString("%1/%2").arg(num).arg(den));
                break;
            }

            case QVariant::Date:
            case QVariant::DateTime:
            {
                QDateTime dateTime = value.toDateTime();
                if(!dateTime.isValid())
                {
                    success = false;
                    break;
                }

                success = exiv2Iface->setXmpTagString(xmpTagName, dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")));
                break;
            }

            case QVariant::String:
            case QVariant::Char:
                success = exiv2Iface->setXmpTagString(xmpTagName, value.toString());
                break;

            case QVariant::ByteArray:
                /// @todo I don't know a straightforward way to convert a byte array to XMP
                success = false;
                break;
                
            default:
                success = false;
                break;
        }
    }

    return success;
}

KipiImageItem::KipiImageItem(KIPI::Interface* const interface, const KUrl& url)
             : m_interface(interface),
               m_model(0),
               m_url(url),
               m_dateTime(),
               m_dirty(false),
               m_gpsData(),
               m_savedState(),
               m_tagListDirty(false),
               m_tagList(),
               m_savedTagList(),
               m_writeXmpTags(true)
{
}

KipiImageItem::~KipiImageItem()
{
}

KExiv2Iface::KExiv2* KipiImageItem::getExiv2ForFile()
{
    QScopedPointer<KExiv2Iface::KExiv2> exiv2Iface(new KExiv2Iface::KExiv2);

    if (m_interface)
    {
        exiv2Iface->setWriteRawFiles(m_interface->hostSetting("WriteMetadataToRAW").toBool());
        exiv2Iface->setUpdateFileTimeStamp(m_interface->hostSetting("WriteMetadataUpdateFiletimeStamp").toBool());
        exiv2Iface->setUseXMPSidecar4Reading(m_interface->hostSetting("UseXMPSidecar4Reading").toBool());
        exiv2Iface->setMetadataWritingMode(m_interface->hostSetting("MetadataWritingMode").toInt());
    }
    else
    {
        exiv2Iface->setUseXMPSidecar4Reading(true);
        exiv2Iface->setMetadataWritingMode(KExiv2::WRITETOSIDECARONLY4READONLYFILES);
    }

    if (!exiv2Iface->load(m_url.path()))
    {
        return 0;
    }

    return exiv2Iface.take();
}

int getWarningLevelFromGPSDataContainer(const GPSDataContainer& data)
{
    if (data.hasDop())
    {
        const int dopValue = data.getDop();
        if (dopValue<2)
            return 1;
        if (dopValue<4)
            return 2;
        if (dopValue<10)
            return 3;
        return 4;
    }
    else if (data.hasFixType())
    {
        if (data.getFixType()<3)
            return 4;
    }
    else if (data.hasNSatellites())
    {
        if (data.getNSatellites()<4)
            return 4;
    }

    // no warning level
    return -1;
}

bool KipiImageItem::loadImageData(const bool fromInterface, const bool fromFile)
{
    if (fromInterface && m_interface)
    {
        // try to load the GPS data from the KIPI interface:
        QMap<QString, QVariant> attributes;
        KIPI::ImageInfo info = m_interface->info(m_url);
        attributes = info.attributes();

        if (attributes.contains("latitude") &&
            attributes.contains("longitude"))
        {
            m_gpsData.setLatLon(attributes["latitude"].toDouble(), attributes["longitude"].toDouble());
            if (attributes.contains("altitude"))
            {
                m_gpsData.setAltitude(attributes["altitude"].toDouble());
            }
        }

        m_dateTime = info.time(KIPI::FromInfo);
    }

    if (fromFile)
    {
        QScopedPointer<KExiv2Iface::KExiv2> exiv2Iface(getExiv2ForFile());

        if (!exiv2Iface)
            return false;

        if (!m_dateTime.isValid())
        {
            m_dateTime = exiv2Iface->getImageDateTime();
        }

        m_gpsData.clear();

        if (!m_gpsData.hasCoordinates())
        {
            // could not load the coordinates from the interface,
            // read them directly from the file

            double lat, lng;
            bool haveCoordinates = exiv2Iface->getGPSLatitudeNumber(&lat) && exiv2Iface->getGPSLongitudeNumber(&lng);
            if (haveCoordinates)
            {
                KMap::GeoCoordinates coordinates(lat, lng);
                double alt;
                if (exiv2Iface->getGPSAltitude(&alt))
                {
                    coordinates.setAlt(alt);
                }
                m_gpsData.setCoordinates(coordinates);
            }
        }

        /** @todo It seems that exiv2 provides EXIF entries if XMP sidecar entries exist,
         *  therefore no need to read XMP as well?
         */
        // read the remaining GPS information from the file:
        const QByteArray speedRef = exiv2Iface->getExifTagData("Exif.GPSInfo.GPSSpeedRef");
        bool success = !speedRef.isEmpty();
        long num, den;
        success&= exiv2Iface->getExifTagRational("Exif.GPSInfo.GPSSpeed", num, den);
        if (success)
        {
            // be relaxed about 0/0
            if ((num==0.0)&&(den==0.0))
                den = 1.0;

            const qreal speedInRef = qreal(num)/qreal(den);

            qreal FactorToMetersPerSecond;
            if (speedRef.startsWith('K'))
            {
                // km/h = 1000 * 3600
                FactorToMetersPerSecond = 1.0/3.6;
            }
            else if (speedRef.startsWith('M'))
            {
                // TODO: someone please check that this is the 'right' mile
                // miles/hour = 1609.344 meters / hour = 1609.344 meters / 3600 seconds
                FactorToMetersPerSecond = 1.0 / (1609.344 / 3600.0);
            }
            else if (speedRef.startsWith('N'))
            {
                // speed is in knots.
                // knot = one nautic mile / hour = 1852 meters / hour = 1852 meters / 3600 seconds
                FactorToMetersPerSecond = 1.0 / (1852.0 / 3600.0);
            }
            else
            {
                success = false;
            }

            if (success)
            {
                const qreal speedInMetersPerSecond = speedInRef * FactorToMetersPerSecond;
                m_gpsData.setSpeed(speedInMetersPerSecond);
            }
        }

        // number of satellites
        const QString gpsSatellitesString = exiv2Iface->getExifTagString("Exif.GPSInfo.GPSSatellites");
        bool satellitesOkay = !gpsSatellitesString.isEmpty();
        if (satellitesOkay)
        {
            /**
             * @todo Here we only accept a single integer denoting the number of satellites used
             *       but not detailed information about all satellites.
             */
            const int nSatellites = gpsSatellitesString.toInt(&satellitesOkay);
            if (satellitesOkay)
            {
                m_gpsData.setNSatellites(nSatellites);
            }
        }

        // fix type / measure mode
        const QByteArray gpsMeasureModeByteArray = exiv2Iface->getExifTagData("Exif.GPSInfo.GPSMeasureMode");
        bool measureModeOkay = !gpsMeasureModeByteArray.isEmpty();
        if (measureModeOkay)
        {
            const int measureMode = gpsMeasureModeByteArray.toInt(&measureModeOkay);
            if (measureModeOkay)
            {
                if ((measureMode==2)||(measureMode==3))
                {
                    m_gpsData.setFixType(measureMode);
                }
            }
        }

        // read the DOP value:
        success= exiv2Iface->getExifTagRational("Exif.GPSInfo.GPSDOP", num, den);
        if (success)
        {
            // be relaxed about 0/0
            if ((num==0.0)&&(den==0.0))
                den = 1.0;

            const qreal dop = qreal(num)/qreal(den);

            m_gpsData.setDop(dop);
        }

    }

    // mark us as not-dirty, because the data was just loaded:
    m_dirty = false;
    m_savedState = m_gpsData;

    emitDataChanged();

    return true;
}

QVariant KipiImageItem::data(const int column, const int role) const
{
    if ((column==ColumnFilename)&&(role==Qt::DisplayRole))
    {
        return m_url.fileName();
    }
    else if ((column==ColumnDateTime)&&(role==Qt::DisplayRole))
    {
        if (m_dateTime.isValid())
        {
            return m_dateTime.toString(Qt::LocalDate);
        }
        return i18n("Not available");
    }
    else if (role==RoleCoordinates)
    {
        return QVariant::fromValue(m_gpsData.getCoordinates());
    }
    else if ((column==ColumnLatitude)&&(role==Qt::DisplayRole))
    {
        if (!m_gpsData.getCoordinates().hasLatitude())
            return QString();

        return KGlobal::locale()->formatNumber(m_gpsData.getCoordinates().lat(), 7);
    }
    else if ((column==ColumnLongitude)&&(role==Qt::DisplayRole))
    {
        if (!m_gpsData.getCoordinates().hasLongitude())
            return QString();

        return KGlobal::locale()->formatNumber(m_gpsData.getCoordinates().lon(), 7);
    }
    else if ((column==ColumnAltitude)&&(role==Qt::DisplayRole))
    {
        if (!m_gpsData.getCoordinates().hasAltitude())
            return QString();

        return KGlobal::locale()->formatNumber(m_gpsData.getCoordinates().alt());
    }
    else if (column==ColumnAccuracy)
    {
        if (role==Qt::DisplayRole)
        {
            if (m_gpsData.hasDop())
            {
                return i18n("DOP: %1", m_gpsData.getDop());
            }

            if (m_gpsData.hasFixType())
            {
                return i18n("Fix: %1d", m_gpsData.getFixType());
            }

            if (m_gpsData.hasNSatellites())
            {
                return i18n("#Sat: %1", m_gpsData.getNSatellites());
            }
        }
        else if (role==Qt::BackgroundRole)
        {
            const int warningLevel = getWarningLevelFromGPSDataContainer(m_gpsData);
            switch (warningLevel)
            {
            case 1:
                return QBrush(Qt::green);
            case 2:
                return QBrush(Qt::yellow);
            case 3:
                // orange
                return QBrush(QColor(0xff, 0x80, 0x00));
            case 4:
                return QBrush(Qt::red);
            default:
                break;
            }
        }
    }
    else if ((column==ColumnDOP)&&(role==Qt::DisplayRole))
    {
        if (!m_gpsData.hasDop())
            return QString();

        return KGlobal::locale()->formatNumber(m_gpsData.getDop());
    }
    else if ((column==ColumnFixType)&&(role==Qt::DisplayRole))
    {
        if (!m_gpsData.hasFixType())
            return QString();

        return i18n("%1d", m_gpsData.getFixType());
    }
    else if ((column==ColumnNSatellites)&&(role==Qt::DisplayRole))
    {
        if (!m_gpsData.hasNSatellites())
            return QString();

        return KGlobal::locale()->formatNumber(m_gpsData.getNSatellites(), 0);
    }
    else if ((column==ColumnSpeed)&&(role==Qt::DisplayRole))
    {
        if (!m_gpsData.hasSpeed())
            return QString();

        return KGlobal::locale()->formatNumber(m_gpsData.getSpeed());
    }
    else if ((column==ColumnStatus)&&(role==Qt::DisplayRole))
    {
        if (m_dirty || m_tagListDirty)
        {
            return i18n("Modified");
        }

        return QString();
    }
    else if ((column==ColumnTags)&&(role==Qt::DisplayRole))
    {
        if (!m_tagList.isEmpty())
        {

            QString myTagsList;
            for (int i=0; i<m_tagList.count(); ++i)
            {
                QString myTag;
                for (int j=0; j<m_tagList[i].count(); ++j)
                {
                    myTag.append(QString("/") + m_tagList[i].at(j).tagName);
                    if (j == 0)
                        myTag.remove(0,1);
                }

                if (!myTagsList.isEmpty())
                    myTagsList.append(", ");
                myTagsList.append(myTag);
            }

            return myTagsList;
        }

        return QString();
    }


    return QVariant();
}

void KipiImageItem::setCoordinates(const KMap::GeoCoordinates& newCoordinates)
{
    m_gpsData.setCoordinates(newCoordinates);
    m_dirty = true;
    emitDataChanged();
}

void KipiImageItem::setModel(KipiImageModel* const model)
{
    m_model = model;
}

void KipiImageItem::emitDataChanged()
{
    if (m_model)
    {
        m_model->itemChanged(this);
    }
}

void KipiImageItem::setHeaderData(KipiImageModel* const model)
{
    model->setColumnCount(ColumnGPSImageItemCount);
    model->setHeaderData(ColumnThumbnail, Qt::Horizontal, i18n("Thumbnail"), Qt::DisplayRole);
    model->setHeaderData(ColumnFilename, Qt::Horizontal, i18n("Filename"), Qt::DisplayRole);
    model->setHeaderData(ColumnDateTime, Qt::Horizontal, i18n("Date and time"), Qt::DisplayRole);
    model->setHeaderData(ColumnLatitude, Qt::Horizontal, i18n("Latitude"), Qt::DisplayRole);
    model->setHeaderData(ColumnLongitude, Qt::Horizontal, i18n("Longitude"), Qt::DisplayRole);
    model->setHeaderData(ColumnAltitude, Qt::Horizontal, i18n("Altitude"), Qt::DisplayRole);
    model->setHeaderData(ColumnAccuracy, Qt::Horizontal, i18n("Accuracy"), Qt::DisplayRole);
    model->setHeaderData(ColumnDOP, Qt::Horizontal, i18n("DOP"), Qt::DisplayRole);
    model->setHeaderData(ColumnFixType, Qt::Horizontal, i18n("Fix type"), Qt::DisplayRole);
    model->setHeaderData(ColumnNSatellites, Qt::Horizontal, i18n("# satellites"), Qt::DisplayRole);
    model->setHeaderData(ColumnSpeed, Qt::Horizontal, i18n("Speed"), Qt::DisplayRole);
    model->setHeaderData(ColumnStatus, Qt::Horizontal, i18n("Status"), Qt::DisplayRole);
    model->setHeaderData(ColumnTags, Qt::Horizontal, i18n("Tags"), Qt::DisplayRole);
}

bool KipiImageItem::lessThan(const KipiImageItem* const otherItem, const int column) const
{
    switch (column)
    {
    case ColumnThumbnail:
        return false;

    case ColumnFilename:
        return m_url < otherItem->m_url;

    case ColumnDateTime:
        return m_dateTime < otherItem->m_dateTime;

        case ColumnAltitude:
    {
        if (!m_gpsData.hasAltitude())
            return false;

        if (!otherItem->m_gpsData.hasAltitude())
            return true;

        return m_gpsData.getCoordinates().alt() < otherItem->m_gpsData.getCoordinates().alt();
    }

    case ColumnNSatellites:
    {
        if (!m_gpsData.hasNSatellites())
            return false;

        if (!otherItem->m_gpsData.hasNSatellites())
            return true;

        return m_gpsData.getNSatellites() < otherItem->m_gpsData.getNSatellites();
    }

    case ColumnAccuracy:
    {
        const int myWarning = getWarningLevelFromGPSDataContainer(m_gpsData);
        const int otherWarning = getWarningLevelFromGPSDataContainer(otherItem->m_gpsData);

        if (myWarning<0)
            return false;

        if (otherWarning<0)
            return true;

        if (myWarning!=otherWarning)
            return myWarning < otherWarning;

        // TODO: this may not be the best way to sort images with equal warning levels
        //       but it works for now

        if (m_gpsData.hasDop()!=otherItem->m_gpsData.hasDop())
            return !m_gpsData.hasDop();
        if (m_gpsData.hasDop()&&otherItem->m_gpsData.hasDop())
        {
            return m_gpsData.getDop()<otherItem->m_gpsData.getDop();
        }

        if (m_gpsData.hasFixType()!=otherItem->m_gpsData.hasFixType())
            return m_gpsData.hasFixType();
        if (m_gpsData.hasFixType()&&otherItem->m_gpsData.hasFixType())
        {
            return m_gpsData.getFixType()>otherItem->m_gpsData.getFixType();
        }

        if (m_gpsData.hasNSatellites()!=otherItem->m_gpsData.hasNSatellites())
            return m_gpsData.hasNSatellites();
        if (m_gpsData.hasNSatellites()&&otherItem->m_gpsData.hasNSatellites())
        {
            return m_gpsData.getNSatellites()>otherItem->m_gpsData.getNSatellites();
        }

        return false;
    }

    case ColumnDOP:
    {
        if (!m_gpsData.hasDop())
            return false;

        if (!otherItem->m_gpsData.hasDop())
            return true;

        return m_gpsData.getDop() < otherItem->m_gpsData.getDop();
    }

    case ColumnFixType:
    {
        if (!m_gpsData.hasFixType())
            return false;

        if (!otherItem->m_gpsData.hasFixType())
            return true;

        return m_gpsData.getFixType() < otherItem->m_gpsData.getFixType();
    }

    case ColumnSpeed:
    {
        if (!m_gpsData.hasSpeed())
            return false;

        if (!otherItem->m_gpsData.hasSpeed())
            return true;

        return m_gpsData.getSpeed() < otherItem->m_gpsData.getSpeed();
    }

    case ColumnLatitude:
    {
        if (!m_gpsData.hasCoordinates())
            return false;

        if (!otherItem->m_gpsData.hasCoordinates())
            return true;

        return m_gpsData.getCoordinates().lat() < otherItem->m_gpsData.getCoordinates().lat();
    }

    case ColumnLongitude:
    {
        if (!m_gpsData.hasCoordinates())
            return false;

        if (!otherItem->m_gpsData.hasCoordinates())
            return true;

        return m_gpsData.getCoordinates().lon() < otherItem->m_gpsData.getCoordinates().lon();
    }

    case ColumnStatus:
    {
        return m_dirty && !otherItem->m_dirty;
    }

    default:
        return false;
    }
}

QString KipiImageItem::saveChanges(const bool toInterface, const bool toFile)
{
    // determine what is to be done first
    bool shouldRemoveCoordinates = false;
    bool shouldRemoveAltitude = false;
    bool shouldWriteCoordinates = false;
    bool shouldWriteAltitude = false;
    qreal altitude = 0;
    qreal latitude = 0;
    qreal longitude = 0;

    // do we have gps information?
    if (m_gpsData.hasCoordinates())
    {
        shouldWriteCoordinates = true;
        latitude = m_gpsData.getCoordinates().lat();
        longitude = m_gpsData.getCoordinates().lon();

        if (m_gpsData.hasAltitude())
        {
            shouldWriteAltitude = true;
            altitude = m_gpsData.getCoordinates().alt();
        }
        else
        {
            shouldRemoveAltitude = true;
        }
    }
    else
    {
        shouldRemoveCoordinates = true;
        shouldRemoveAltitude = true;
    }

    QString returnString;

    // first try to write the information to the image file
    bool success = false;
    QScopedPointer<KExiv2Iface::KExiv2> exiv2Iface(getExiv2ForFile());
    if (!exiv2Iface)
    {
        // TODO: more verbosity!
        returnString = i18n("Failed to open file.");
    }
    else
    {
        if (shouldWriteCoordinates)
        {
            if (shouldWriteAltitude)
            {
                success = exiv2Iface->setGPSInfo(altitude, latitude, longitude);
            }
            else
            {
                success = exiv2Iface->setGPSInfo(static_cast<const double* const>(0), latitude, longitude);
            }

            // write all other GPS information here too
            if (success && m_gpsData.hasSpeed())
            {
                success = KExiv2SetExifXmpTagDataVariant(exiv2Iface.data(), "Exif.GPSInfo.GPSSpeedRef", "Xmp.exif.GPSSpeedRef", QVariant(QString("K")));

                if (success)
                {
                    const qreal speedInMetersPerSecond = m_gpsData.getSpeed();

                    // km/h = 0.001 * m / ( s * 1/(60*60) ) = 3.6 * m/s
                    const qreal speedInKilometersPerHour = 3.6 * speedInMetersPerSecond;
                    success = KExiv2SetExifXmpTagDataVariant(exiv2Iface.data(), "Exif.GPSInfo.GPSSpeed", "Xmp.exif.GPSSpeed", QVariant(speedInKilometersPerHour));
                }
            }

            if (success && m_gpsData.hasNSatellites())
            {
                /**
                 * @todo According to the EXIF 2.2 spec, GPSSatellites is a free form field which can either hold only the
                 * number of satellites or more details about each satellite used. For now, we just write
                 * the number of satellites. Are we using the correct format for the number of satellites here?
                 */
                success = KExiv2SetExifXmpTagDataVariant(exiv2Iface.data(),
                                                         "Exif.GPSInfo.GPSSatellites", "Xmp.exif.GPSSatellites",
                                                         QVariant(QString::number(m_gpsData.getNSatellites())));
            }

            if (success && m_gpsData.hasFixType())
            {
                success = KExiv2SetExifXmpTagDataVariant(exiv2Iface.data(),
                                                         "Exif.GPSInfo.GPSMeasureMode", "Xmp.exif.GPSMeasureMode",
                                                         QVariant(QString::number(m_gpsData.getFixType())));
            }

            // write DOP
            if (success && m_gpsData.hasDop())
            {
                success = KExiv2SetExifXmpTagDataVariant(
                        exiv2Iface.data(),
                        "Exif.GPSInfo.GPSDOP",
                        "Xmp.exif.GPSDOP",
                        QVariant(m_gpsData.getDop())
                    );
            }


            if (!success)
            {
                returnString = i18n("Failed to add GPS info to image.");
            }
        }
        if (shouldRemoveCoordinates)
        {
            // TODO: remove only the altitude if requested
            success = exiv2Iface->removeGPSInfo();
            if (!success)
            {
                returnString = i18n("Failed to remove GPS info from image");
            }
        }

        if (!m_tagList.isEmpty() && m_writeXmpTags)
        {

            QStringList tagSeq;

            for (int i=0; i<m_tagList.count(); ++i)
            {
                QList<TagData> currentTagList = m_tagList[i];
                QString tag;

                for (int j=0; j<currentTagList.count(); ++j)
                {
                    tag.append(QString("/") + currentTagList[j].tagName);
                }
                tag.remove(0,1);

                tagSeq.append(tag);
            }

            bool success = exiv2Iface->setXmpTagStringSeq("Xmp.digiKam.TagsList", tagSeq, true);
            if (!success)
            {
                returnString = i18n("Failed to save tags to file.");
            }
            success = exiv2Iface->setXmpTagStringSeq("Xmp.dc.subject", tagSeq, true);
            if (!success)
            {
                returnString = i18n("Failed to save tags to file.");
            }
        }
    }

    if (success)
    {
        success = exiv2Iface->save(m_url.path());
        if (!success)
        {
            returnString = i18n("Unable to save changes to file");
        }
        else
        {
            m_dirty = false;
            m_savedState = m_gpsData;
            m_tagListDirty = false;
            m_savedTagList = m_tagList;
        }
    }

    // now tell the interface about the changes
    // TODO: remove the altitude if it is not available
    if (m_interface)
    {
        if (shouldWriteCoordinates)
        {
            QMap<QString, QVariant> attributes;
            attributes.insert("latitude", latitude);
            attributes.insert("longitude", longitude);
            if (shouldWriteAltitude)
            {
                attributes.insert("altitude", altitude);
            }

            KIPI::ImageInfo info = m_interface->info(m_url);
            info.addAttributes(attributes);
        }

        if (shouldRemoveCoordinates)
        {
            QStringList listToRemove;
            listToRemove << "gpslocation";
            KIPI::ImageInfo info = m_interface->info(m_url);
            info.delAttributes(listToRemove);
        }

        if (!m_tagList.isEmpty())
        {
            QMap<QString, QVariant> attributes;
            QStringList tagsPath;

            for (int i=0; i<m_tagList.count(); ++i)
            {

                QString singleTagPath;
                QList<TagData> currentTagPath = m_tagList[i];
                for (int j=0; j<currentTagPath.count(); ++j)
                {
                    singleTagPath.append(QString("%1").arg("/") + currentTagPath[j].tagName);
                    if (j == 0)
                    {
                        singleTagPath.remove(0,1);
                    }
                }

                tagsPath.append(singleTagPath);
            }

            attributes.insert("tagspath", tagsPath);
            KIPI::ImageInfo info = m_interface->info(m_url);
            info.addAttributes(attributes);
        }
    }

    if (returnString.isEmpty())
    {
        // mark all changes as not dirty and tell the model:
        emitDataChanged();
    }

    return returnString;
}

/**
 * @brief Restore the gps data to @p container. Sets m_dirty to false if container equals savedState.
 */
void KipiImageItem::restoreGPSData(const GPSDataContainer& container)
{
    m_dirty = !(container == m_savedState);
    m_gpsData = container;
    emitDataChanged();
}

void KipiImageItem::restoreRGTagList(const QList<QList<TagData> >& tagList)
{
    //TODO: override == operator

    if (tagList.count() != m_savedTagList.count())
        m_tagListDirty = true;
    else
    {
        for (int i=0; i<tagList.count(); ++i)
        {
            bool foundNotEqual = false;

            if (tagList[i].count() != m_savedTagList[i].count())
            {
                m_tagListDirty = true;
                break;
            }

            for (int j=0; j<tagList[i].count(); ++j)
            {
                if (tagList[i].at(j).tagName != m_savedTagList[i].at(j).tagName)
                {
                    foundNotEqual = true;
                    break;
                }
            }

            if (foundNotEqual)
            {
                m_tagListDirty = true;
                break;
            }
        }
    }

    m_tagList = tagList;
    emitDataChanged();
}

} /* KIPIGPSSyncPlugin */
