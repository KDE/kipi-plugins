/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
            Alexios Beveratos <alexios.beveratos@lpn.cnrs.fr>
 * Date   : 2006-09-15
 * Description : Exiv2 library interface
 *
 * Copyright 2006 by Gilles Caulier
 * Copyright 2006 by Alexios Beveratos
 *
 *
 * NOTE: This class is a simplified version of Digikam::DMetadata
 *       class from digiKam core. Please contact digiKam team 
 *       before to change/fix/improve this implementation.
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

 // C++ includes.

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <string>
#include <cmath>
#include <iostream>
#include <iomanip>

// Qt includes.

#include <qfile.h>
#include <qimage.h>
#include <qsize.h>

// KDE includes.

#include <ktempfile.h>
#include <kdebug.h>

// Exiv2 includes.

#include <exiv2/types.hpp>
#include <exiv2/exif.hpp>
#include <exiv2/image.hpp>
#include <exiv2/jpgimage.hpp>
#include <exiv2/datasets.hpp>
#include <exiv2/tags.hpp>

// Local includes.

#include "exiv2iface.h"

namespace KIPIPlugins
{

class Exiv2IfacePriv
{
public:

    Exiv2IfacePriv(){}

    std::string     imageComments;  

    Exiv2::ExifData exifMetadata;

    Exiv2::IptcData iptcMetadata;
};

Exiv2Iface::Exiv2Iface()
{
    d = new Exiv2IfacePriv;
}

Exiv2Iface::~Exiv2Iface()
{
    delete d;
}

QByteArray Exiv2Iface::getComments() const
{
    QByteArray data(d->imageComments.size());
    memcpy(data.data(), d->imageComments.c_str(), d->imageComments.size());
    return data;
}

void Exiv2Iface::setComments(const QByteArray& data)
{
    QString string(data);
    const std::string str(string.utf8());
    d->imageComments = str;
}

QByteArray Exiv2Iface::getExif() const
{
    try
    {    
        if (!d->exifMetadata.empty())
        {

            Exiv2::ExifData& exif = d->exifMetadata;
            Exiv2::DataBuf c2 = exif.copy();
            QByteArray data(c2.size_);
            memcpy(data.data(), c2.pData_, c2.size_);
            return data;
        }
    }
    catch( Exiv2::Error &e )
    {
        kdDebug() << "Cannot get Exif data using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }       
    
    return QByteArray();
}

QByteArray Exiv2Iface::getIptc(bool addIrbHeader) const
{
    try
    {    
        if (!d->iptcMetadata.empty())
        {                
            Exiv2::IptcData& iptc = d->iptcMetadata;
            Exiv2::DataBuf c2;

            if (addIrbHeader) 
            {
#ifdef EXIV2_CHECK_VERSION 
                if (EXIV2_CHECK_VERSION(0,10,0))
                    c2 = Exiv2::Photoshop::setIptcIrb(0, 0, iptc);
                else
                {
                    kdDebug() << "Exiv2 version is to old. Cannot add Irb header to IPTC metadata" << endl;
                    return QByteArray();
                }
#endif
            }
            else 
                c2 = iptc.copy();

            QByteArray data(c2.size_);
            memcpy(data.data(), c2.pData_, c2.size_);
            return data;
        }
    }
    catch( Exiv2::Error &e )
    {
        kdDebug() << "Cannot get Iptc data using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }       
    
    return QByteArray();
}

void Exiv2Iface::setExif(const QByteArray& data)
{
    try
    {    
        if (!data.isEmpty())
            d->exifMetadata.load((const Exiv2::byte*)data.data(), data.size());
    }
    catch( Exiv2::Error &e )
    {
        kdDebug() << "Cannot set Exif data using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
}

void Exiv2Iface::setIptc(const QByteArray& data)
{
    try
    {    
        if (!data.isEmpty())
            d->iptcMetadata.load((const Exiv2::byte*)data.data(), data.size());
    }
    catch( Exiv2::Error &e )
    {
        kdDebug() << "Cannot set Iptc data using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
}

bool Exiv2Iface::load(const QString& filePath)
{
    try
    {    
        if (filePath.isEmpty())
            return false;

        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((const char*)
                                      (QFile::encodeName(filePath)));
        image->readMetadata();

        // Image comments ---------------------------------

        d->imageComments = image->comment();

        // Exif metadata ----------------------------------
        
        d->exifMetadata = image->exifData();

        // Iptc metadata ----------------------------------
        
        d->iptcMetadata = image->iptcData();

        return true;
    }
    catch( Exiv2::Error &e )
    {
        kdDebug() << "Cannot load metadata using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
        return false;
    }
}

bool Exiv2Iface::save(const QString& filePath)
{
    try
    {    
        if (filePath.isEmpty())
            return false;

        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((const char*)
                                      (QFile::encodeName(filePath)));
        
        // Image Comments ---------------------------------
        
        if (!d->imageComments.empty())
        {
            image->setComment(d->imageComments);
        }

        // Exif metadata ----------------------------------
        
        if (!d->exifMetadata.empty())
        {
            image->setExifData(d->exifMetadata);
        }

        // Iptc metadata ----------------------------------
        
        if (!d->iptcMetadata.empty())
        {
            image->setIptcData(d->iptcMetadata);
        }
    
        image->writeMetadata();

        return true;
    }
    catch( Exiv2::Error &e )
    {
        kdDebug() << "Cannot save metadata using Exiv2 ("
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
        return false;
    }
}

bool Exiv2Iface::setImageProgramId(const QString& program, const QString& version)
{
    try
    {
        QString software(program);
        software.append("-");
        software.append(version);
        d->exifMetadata["Exif.Image.Software"]              = software.ascii();

        d->iptcMetadata["Iptc.Application2.Program"]        = program.ascii();
        d->iptcMetadata["Iptc.Application2.ProgramVersion"] = version.ascii();
        return true;
    }
    catch( Exiv2::Error &e )
    {
        kdDebug() << "Cannot set Program identity into image using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }

    return false;
}

bool Exiv2Iface::setImageDimensions(const QSize& size)
{
    try
    {    
        d->exifMetadata["Exif.Image.ImageWidth"]      = size.width();
        d->exifMetadata["Exif.Image.ImageLength"]     = size.height();
        d->exifMetadata["Exif.Photo.PixelXDimension"] = size.width();
        d->exifMetadata["Exif.Photo.PixelYDimension"] = size.height();
        return true;
    }
    catch( Exiv2::Error &e )
    {
        kdDebug() << "Cannot set Date & Time into image using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return false;
}

bool Exiv2Iface::setExifThumbnail(const QImage& thumb)
{
    try
    {   
        KTempFile thumbFile(QString::null, "DigikamDMetadataThumb");
        thumbFile.setAutoDelete(true);
        thumb.save(thumbFile.name(), "JPEG");

        const std::string &fileName( (const char*)(QFile::encodeName(thumbFile.name())) );
        d->exifMetadata.setJpegThumbnail( fileName );
        return true;
    }
    catch( Exiv2::Error &e )
    {
        kdDebug() << "Cannot set Exif Thumbnail using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return false;
}

QString Exiv2Iface::getExifTagString(const char* exifTagName) const
{
    try
    {
        Exiv2::ExifKey exifKey(exifTagName);
        Exiv2::ExifData exifData(d->exifMetadata);
        Exiv2::ExifData::iterator it = exifData.findKey(exifKey);
        if (it != exifData.end())
        {
            std::ostringstream os;
            os << *it;
            QString tagValue = QString::fromLocal8Bit(os.str().c_str());
            tagValue.replace("\n", " ");
            return tagValue;
        }
    }
    catch( Exiv2::Error &e )
    {
        kdDebug() << "Cannot find Exif key '"
                  << exifTagName << "' into image using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }

    return QString();
}

bool Exiv2Iface::setExifTagString(const char *exifTagName, const QString& value)
{
    try
    {
        d->exifMetadata[exifTagName] = value.ascii();
        return true;
    }
    catch( Exiv2::Error &e )
    {
        kdDebug() << "Cannot set Exif tag string into image using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }

    return false;
}

bool Exiv2Iface::setImagePreview(const QImage& preview)
{
    try
    {
        KTempFile previewFile(QString::null, "DigikamDMetadataPreview");
        previewFile.setAutoDelete(true);
        // A little bit compressed preview jpeg image to limit IPTC size.
        preview.save(previewFile.name(), "JPEG");

        QFile file(previewFile.name());
        if ( !file.open(IO_ReadOnly) ) 
            return false;

        kdDebug() << "(" << preview.width() << "x" << preview.height() 
                  << ") JPEG image preview size: " << file.size() 
                  << " bytes" << endl;
        
        QByteArray data(file.size());
        QDataStream stream( &file );
        stream.readRawBytes(data.data(), data.size());
        file.close();
        
        Exiv2::DataValue val;
        val.read((Exiv2::byte *)data.data(), data.size());
        d->iptcMetadata["Iptc.Application2.Preview"] = val;
        
        // See http://www.iptc.org/std/IIM/4.1/specification/IIMV4.1.pdf Appendix A for details.
        d->iptcMetadata["Iptc.Application2.PreviewFormat"]  = 11;  // JPEG 
        d->iptcMetadata["Iptc.Application2.PreviewVersion"] = 1;
        
        return true;
    }
    catch( Exiv2::Error &e )
    {
        kdDebug() << "Cannot get image preview using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }

    return false;
}

QDateTime Exiv2Iface::getImageDateTime() const
{
    try
    {    
        // In first, trying to get Date & time from Exif tags.
        
        if (!d->exifMetadata.empty())
        {        
            // Try standard Exif date time entry.
    
            Exiv2::ExifKey key("Exif.Image.DateTime");
            Exiv2::ExifData exifData(d->exifMetadata);
            Exiv2::ExifData::iterator it = exifData.findKey(key);
            
            if (it != exifData.end())
            {
                QDateTime dateTime = QDateTime::fromString(it->toString().c_str(), Qt::ISODate);
    
                if (dateTime.isValid())
                {
                    // kdDebug() << "DateTime (Exif standard): " << dateTime << endl;
                    return dateTime;
                }
            }
    
            // Bogus standard Exif date time entry. Try Exif date time original.
    
            Exiv2::ExifKey key2("Exif.Photo.DateTimeOriginal");
            Exiv2::ExifData::iterator it2 = exifData.findKey(key2);
            
            if (it2 != exifData.end())
            {
                QDateTime dateTime = QDateTime::fromString(it2->toString().c_str(), Qt::ISODate);
    
                if (dateTime.isValid())
                {
                    // kdDebug() << "DateTime (Exif original): " << dateTime << endl;
                    return dateTime;
                }
            }
    
            // Bogus Exif date time original entry. Try Exif date time digitized.
    
            Exiv2::ExifKey key3("Exif.Photo.DateTimeDigitized");
            Exiv2::ExifData::iterator it3 = exifData.findKey(key3);
            
            if (it3 != exifData.end())
            {
                QDateTime dateTime = QDateTime::fromString(it3->toString().c_str(), Qt::ISODate);
    
                if (dateTime.isValid())
                {
                    // kdDebug() << "DateTime (Exif digitalized): " << dateTime << endl;
                    return dateTime;
                }
            }
        }
        
        // In second, trying to get Date & time from Iptc tags.
            
        if (!d->iptcMetadata.empty())
        {        
            // Try creation Iptc date time entries.

            Exiv2::IptcKey keyDateCreated("Iptc.Application2.DateCreated");
            Exiv2::IptcData iptcData(d->iptcMetadata);
            Exiv2::IptcData::iterator it = iptcData.findKey(keyDateCreated);
                        
            if (it != iptcData.end())
            {
                QString IptcDateCreated(it->toString().c_str());
    
                Exiv2::IptcKey keyTimeCreated("Iptc.Application2.TimeCreated");
                Exiv2::IptcData::iterator it2 = iptcData.findKey(keyTimeCreated);
                
                if (it2 != iptcData.end())
                {
                    QString IptcTimeCreated(it2->toString().c_str());
                    
                    QDate date = QDate::fromString(IptcDateCreated, Qt::ISODate);
                    QTime time = QTime::fromString(IptcTimeCreated, Qt::ISODate);
                    QDateTime dateTime = QDateTime(date, time);
                    
                    if (dateTime.isValid())
                    {
                        // kdDebug() << "Date (IPTC created): " << dateTime << endl;
                        return dateTime;
                    }                    
                }
            }                        
            
            // Try digitization Iptc date time entries.
    
            Exiv2::IptcKey keyDigitizationDate("Iptc.Application2.DigitizationDate");
            Exiv2::IptcData::iterator it3 = iptcData.findKey(keyDigitizationDate);
                        
            if (it3 != iptcData.end())
            {
                QString IptcDateDigitization(it3->toString().c_str());
    
                Exiv2::IptcKey keyDigitizationTime("Iptc.Application2.DigitizationTime");
                Exiv2::IptcData::iterator it4 = iptcData.findKey(keyDigitizationTime);
                
                if (it4 != iptcData.end())
                {
                    QString IptcTimeDigitization(it4->toString().c_str());
                    
                    QDate date = QDate::fromString(IptcDateDigitization, Qt::ISODate);
                    QTime time = QTime::fromString(IptcTimeDigitization, Qt::ISODate);
                    QDateTime dateTime = QDateTime(date, time);
                    
                    if (dateTime.isValid())
                    {
                        // kdDebug() << "Date (IPTC digitalized): " << dateTime << endl;
                        return dateTime;
                    }                    
                }
            }                       
        }
    }
    catch( Exiv2::Error &e )
    {
        kdDebug() << "Cannot parse Exif date & time tag using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return QDateTime();
}

Exiv2Iface::ImageOrientation Exiv2Iface::getImageOrientation()
{
    if (d->exifMetadata.empty())
       return ORIENTATION_UNSPECIFIED;

    // Workaround for older Exiv2 versions which do not support
    // Minolta Makernotes and throw an error for such keys.
    bool supportMinolta = true;
    try
    {
        Exiv2::ExifKey minoltaKey1("Exif.MinoltaCs7D.Rotation");
        Exiv2::ExifKey minoltaKey2("Exif.MinoltaCs5D.Rotation");
    }
    catch( Exiv2::Error &e )
    {
        supportMinolta = false;
    }

    try
    {
        Exiv2::ExifData exifData(d->exifMetadata);
        Exiv2::ExifData::iterator it;
        long orientation;
        ImageOrientation imageOrient = ORIENTATION_NORMAL;

        // Because some camera set a wrong standard exif orientation tag, 
        // We need to check makernote tags in first!

        // -- Minolta Cameras ----------------------------------

        if (supportMinolta)
        {
            Exiv2::ExifKey minoltaKey1("Exif.MinoltaCs7D.Rotation");
            it = exifData.findKey(minoltaKey1);

            if (it != exifData.end())
            {
                orientation = it->toLong();
                kdDebug() << "Minolta Makernote Orientation: " << orientation << endl;
                switch(orientation)
                {
                    case 76:
                        imageOrient = ORIENTATION_ROT_90;
                        break;
                    case 82:
                        imageOrient = ORIENTATION_ROT_270;
                        break;
                }
                return imageOrient;
            }

            Exiv2::ExifKey minoltaKey2("Exif.MinoltaCs5D.Rotation");
            it = exifData.findKey(minoltaKey2);

            if (it != exifData.end())
            {
                orientation = it->toLong();
                kdDebug() << "Minolta Makernote Orientation: " << orientation << endl;
                switch(orientation)
                {
                    case 76:
                        imageOrient = ORIENTATION_ROT_90;
                        break;
                    case 82:
                        imageOrient = ORIENTATION_ROT_270;
                        break;
                }
                return imageOrient;
            }
        }

        // -- Standard Exif tag --------------------------------

        Exiv2::ExifKey keyStd("Exif.Image.Orientation");
        it = exifData.findKey(keyStd);

        if (it != exifData.end())
        {
            orientation = it->toLong();
            kdDebug() << "Exif Orientation: " << orientation << endl;
            return (ImageOrientation)orientation;
        }
    }
    catch( Exiv2::Error &e )
    {
        kdDebug() << "Cannot parse Exif Orientation tag using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }

    return ORIENTATION_UNSPECIFIED;
}

bool Exiv2Iface::setImageOrientation(ImageOrientation orientation)
{
    if (d->exifMetadata.empty())
       return false;

    // Workaround for older Exiv2 versions which do not support
    // Minolta Makernotes and throw an error for such keys.
    bool supportMinolta = true;
    try
    {
        Exiv2::ExifKey minoltaKey1("Exif.MinoltaCs7D.Rotation");
        Exiv2::ExifKey minoltaKey2("Exif.MinoltaCs5D.Rotation");
    }
    catch( Exiv2::Error &e )
    {
        supportMinolta = false;
    }

    try
    {    
        if (orientation < ORIENTATION_UNSPECIFIED || orientation > ORIENTATION_ROT_270)
        {
            kdDebug() << k_funcinfo << "Exif orientation tag value is not correct!" << endl;
            return false;
        }
        
        d->exifMetadata["Exif.Image.Orientation"] = (uint16_t)orientation;
        kdDebug() << "Exif orientation tag set to: " << orientation << endl;

        // -- Minolta Cameras ----------------------------------

        if (supportMinolta)
        {
            // Minolta camera store image rotation in Makernote.
            // We remove these informations to prevent duplicate values. 
    
            Exiv2::ExifData::iterator it;

            Exiv2::ExifKey minoltaKey1("Exif.MinoltaCs7D.Rotation");
            it = d->exifMetadata.findKey(minoltaKey1);
            if (it != d->exifMetadata.end())
            {
                d->exifMetadata.erase(it);
                kdDebug() << "Removing Exif.MinoltaCs7D.Rotation tag" << endl;
            }
        
            Exiv2::ExifKey minoltaKey2("Exif.MinoltaCs5D.Rotation");
            it = d->exifMetadata.findKey(minoltaKey2);
            if (it != d->exifMetadata.end())
            {
                d->exifMetadata.erase(it);
                kdDebug() << "Removing Exif.MinoltaCs5D.Rotation tag" << endl;
            }
        }

        return true;
    }
    catch( Exiv2::Error &e )
    {
        kdDebug() << "Cannot set Exif Orientation tag using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return false;
}

bool Exiv2Iface::getGPSInfo(double& altitude, double& latitude, double& longitude)
{
    try
    {    
        QString rational, num, den;
        altitude = 0.0, latitude=0.0, longitude=0.0;
        
        // Latitude decoding.
        
        QString latRef = getExifTagString("Exif.GPSInfo.GPSLatitudeRef");
        if (latRef.isEmpty()) return false;
            
        QString lat = getExifTagString("Exif.GPSInfo.GPSLatitude");
        if (lat.isEmpty()) return false;
        rational = lat.section(" ", 0, 0);
        num      = rational.section("/", 0, 0);
        den      = rational.section("/", 1, 1);
        latitude = num.toDouble()/den.toDouble();
        rational = lat.section(" ", 1, 1);
        num      = rational.section("/", 0, 0);
        den      = rational.section("/", 1, 1);
        latitude = latitude + (num.toDouble()/den.toDouble())/60.0;
        rational = lat.section(" ", 2, 2);
        num      = rational.section("/", 0, 0);
        den      = rational.section("/", 1, 1);
        latitude = latitude + (num.toDouble()/den.toDouble())/3600.0;
        
        if (latRef == "S") latitude *= -1.0;
    
        // Longitude decoding.
        
        QString lngRef = getExifTagString("Exif.GPSInfo.GPSLongitudeRef");
        if (lngRef.isEmpty()) return false;
    
        QString lng = getExifTagString("Exif.GPSInfo.GPSLongitude");
        if (lng.isEmpty()) return false;
        rational  = lng.section(" ", 0, 0);
        num       = rational.section("/", 0, 0);
        den       = rational.section("/", 1, 1);
        longitude = num.toDouble()/den.toDouble();
        rational  = lng.section(" ", 1, 1);
        num       = rational.section("/", 0, 0);
        den       = rational.section("/", 1, 1);
        longitude = longitude + (num.toDouble()/den.toDouble())/60.0;
        rational  = lng.section(" ", 2, 2);
        num       = rational.section("/", 0, 0);
        den       = rational.section("/", 1, 1);
        longitude = longitude + (num.toDouble()/den.toDouble())/3600.0;
        
        if (lngRef == "W") longitude *= -1.0;

        // Altitude decoding.

        QString altRef = getExifTagString("Exif.GPSInfo.GPSAltitudeRef");
        if (altRef.isEmpty()) return false;
        QString alt = getExifTagString("Exif.GPSInfo.GPSAltitude");
        if (alt.isEmpty()) return false;
        num       = rational.section("/", 0, 0);
        den       = rational.section("/", 1, 1);
        altitude  = num.toDouble()/den.toDouble();
        
        if (altRef == "1") altitude *= -1.0;

        return true;
    }
    catch( Exiv2::Error &e )
    {
        kdDebug() << "Cannot get Exif GPS tag using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return false;
}

bool Exiv2Iface::setGPSInfo(double altitude, double latitude, double longitude)
{
    try
    {    
        char scratchBuf[100];
        long int nom, denom;
        long int deg, min;
        
        // Do all the easy constant ones first.
        // GPSVersionID tag: standard says is should be four bytes: 02 00 00 00
        // (and, must be present).
        Exiv2::Value::AutoPtr value = Exiv2::Value::create(Exiv2::unsignedByte);
        value->read("2 0 0 0");
        d->exifMetadata.add(Exiv2::ExifKey("Exif.GPSInfo.GPSVersionID"), value.get());

        // Datum: the datum of the measured data. If not given, we insert WGS-84.
        d->exifMetadata["Exif.GPSInfo.GPSMapDatum"] = "WGS-84";
        
        // Now start adding data.

        // ALTITUDE.
        // Altitude reference: byte "00" meaning "sea level".
        value = Exiv2::Value::create(Exiv2::unsignedByte);
        value->read("0");
        d->exifMetadata.add(Exiv2::ExifKey("Exif.GPSInfo.GPSAltitudeRef"), value.get());
        
        // And the actual altitude.
        value = Exiv2::Value::create(Exiv2::signedRational);
        convertToRational(altitude, &nom, &denom, 4);
        snprintf(scratchBuf, 100, "%ld/%ld", nom, denom);
        value->read(scratchBuf);
        d->exifMetadata.add(Exiv2::ExifKey("Exif.GPSInfo.GPSAltitude"), value.get());

        // LATTITUDE
        // Latitude reference: "N" or "S".
        if (latitude < 0)
        {
            // Less than Zero: ie, minus: means
            // Southern hemisphere. Where I live.
            d->exifMetadata["Exif.GPSInfo.GPSLatitudeRef"] = "S";
        } 
        else 
        {
            // More than Zero: ie, plus: means
            // Northern hemisphere.
            d->exifMetadata["Exif.GPSInfo.GPSLatitudeRef"] = "N";
        }
        
        // Now the actual lattitude itself.
        // This is done as three rationals.
        // I choose to do it as:
        //   dd/1 - degrees.
        //   mmmm/100 - minutes
        //   0/1 - seconds
        // Exif standard says you can do it with minutes
        // as mm/1 and then seconds as ss/1, but its
        // (slightly) more accurate to do it as
        //  mmmm/100 than to split it.
        // We also absolute the value (with fabs())
        // as the sign is encoded in LatRef.
        // Further note: original code did not translate between
        //   dd.dddddd to dd mm.mm - that's why we now multiply
        //   by 6000 - x60 to get minutes, x100 to get to mmmm/100.
        value = Exiv2::Value::create(Exiv2::signedRational);
        deg   = (int)floor(fabs(latitude)); // Slice off after decimal.
        min   = (int)floor((fabs(latitude) - floor(fabs(latitude))) * 6000);
        snprintf(scratchBuf, 100, "%ld/1 %ld/100 0/1", deg, min);
        value->read(scratchBuf);
        d->exifMetadata.add(Exiv2::ExifKey("Exif.GPSInfo.GPSLatitude"), value.get());
        
        // LONGITUDE
        // Longitude reference: "E" or "W".
        if (longitude < 0)
        {
            // Less than Zero: ie, minus: means
            // Western hemisphere.
            d->exifMetadata["Exif.GPSInfo.GPSLongitudeRef"] = "W";
        } 
        else 
        {
            // More than Zero: ie, plus: means
            // Eastern hemisphere. Where I live.
            d->exifMetadata["Exif.GPSInfo.GPSLongitudeRef"] = "E";
        }

        // Now the actual longitude itself.
        // This is done as three rationals.
        // I choose to do it as:
        //   dd/1 - degrees.
        //   mmmm/100 - minutes
        //   0/1 - seconds
        // Exif standard says you can do it with minutes
        // as mm/1 and then seconds as ss/1, but its
        // (slightly) more accurate to do it as
        //  mmmm/100 than to split it.
        // We also absolute the value (with fabs())
        // as the sign is encoded in LongRef.
        // Further note: original code did not translate between
        //   dd.dddddd to dd mm.mm - that's why we now multiply
        //   by 6000 - x60 to get minutes, x100 to get to mmmm/100.
        value = Exiv2::Value::create(Exiv2::signedRational);
        deg   = (int)floor(fabs(longitude)); // Slice off after decimal.
        min   = (int)floor((fabs(longitude) - floor(fabs(longitude))) * 6000);
        snprintf(scratchBuf, 100, "%ld/1 %ld/100 0/1", deg, min);
        value->read(scratchBuf);
        d->exifMetadata.add(Exiv2::ExifKey("Exif.GPSInfo.GPSLongitude"), value.get());
    
        return true;
    }
    catch( Exiv2::Error &e )
    {
        kdDebug() << "Cannot set Exif GPS tag using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return false;
}

bool Exiv2Iface::removeGPSInfo()
{
    try
    {  
        QStringList gpsTagsKeys;
  
        for (Exiv2::ExifData::iterator it = d->exifMetadata.begin();
             it != d->exifMetadata.end(); ++it)
        {
            QString key = QString::fromLocal8Bit(it->key().c_str());

            if (key.section(".", 1, 1) == QString("GPSInfo"))
                gpsTagsKeys.append(key);
        }

        for(QStringList::Iterator it2 = gpsTagsKeys.begin(); it2 != gpsTagsKeys.end(); ++it2)             
        {
            Exiv2::ExifKey gpsKey((*it2).ascii());
            Exiv2::ExifData::iterator it3 = d->exifMetadata.findKey(gpsKey);
            if (it3 != d->exifMetadata.end())
                d->exifMetadata.erase(it3);
        }
        
        return true;
    }
    catch( Exiv2::Error &e )
    {
        kdDebug() << "Cannot remove Exif GPS tag using Exiv2 (" 
                  << QString::fromLocal8Bit(e.what().c_str())
                  << ")" << endl;
    }        
    
    return false;
}

void Exiv2Iface::convertToRational(double number, long int* numerator, 
                                   long int* denominator, int rounding)
{
    // This function converts the given decimal number
    // to a rational (fractional) number.
    //
    // Examples in comments use Number as 25.12345, Rounding as 4.
    
    // Split up the number.
    double whole      = trunc(number);
    double fractional = number - whole;

    // Calculate the "number" used for rounding.
    // This is 10^Digits - ie, 4 places gives us 10000.
    double rounder = pow(10, rounding);

    // Round the fractional part, and leave the number
    // as greater than 1.
    // To do this we: (for example)
    //  0.12345 * 10000 = 1234.5
    //  floor(1234.5) = 1234 - now bigger than 1 - ready...
    fractional = trunc(fractional * rounder);

    // Convert the whole thing to a fraction.
    // Fraction is:
    //     (25 * 10000) + 1234   251234
    //     ------------------- = ------ = 25.1234
    //           10000            10000
    double numTemp = (whole * rounder) + fractional;
    double denTemp = rounder;

    // Now we should reduce until we can reduce no more.
    
    // Try simple reduction...
    // if   Num
    //     ----- = integer out then....
    //      Den
    if (trunc(numTemp / denTemp) == (numTemp / denTemp))
    {
        // Divide both by Denominator.
        numTemp /= denTemp;
        denTemp /= denTemp;
    }
    
    // And, if that fails, brute force it.
    while (1)
    {
        // Jump out if we can't integer divide one.
        if ((numTemp / 2) != trunc(numTemp / 2)) break;
        if ((denTemp / 2) != trunc(denTemp / 2)) break;
        // Otherwise, divide away.
        numTemp /= 2;
        denTemp /= 2;
    }

    // Copy out the numbers.
    *numerator   = (int)numTemp;
    *denominator = (int)denTemp;
}

}  // NameSpace KIPIPlugins
