/* ============================================================
 * Author: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date  : 2006-09-15
 * Description : Exiv2 library interface
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

 // C++ includes.

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <string>

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

    Exiv2IfacePriv()
    {
    }

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

}  // NameSpace KIPIPlugins
