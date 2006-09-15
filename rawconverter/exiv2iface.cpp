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

// KDE includes.

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

namespace KIPIRawConverterPlugin
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

}  // NameSpace KIPIRawConverterPlugin
