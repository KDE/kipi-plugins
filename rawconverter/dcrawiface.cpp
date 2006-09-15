/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net> 
 *          Marcel Wiesweg <marcel.wiesweg@gmx.de>
 * Date   : 2006-12-09
 * Description : dcraw interface (tested with dcraw 8.x releases)
 *
 * Copyright 2006 by Gilles Caulier and Marcel Wiesweg
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
#include <cstring>
#include <iostream>
#include <cstdio>
#include <cstdlib>

// C Ansi includes.

extern "C"
{
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <jpeglib.h>
#include <tiffio.h>
#include <tiffvers.h>
#include "iccjpeg.h"
}

// Qt Includes.

#include <qfile.h>
#include <qtimer.h>
#include <qcstring.h>
#include <qfileinfo.h>
#include <qapplication.h>
#include <qmutex.h>
#include <qwaitcondition.h>

// KDE includes.

#include <kdebug.h>
#include <klocale.h>
#include <kprocess.h>
#include <kstandarddirs.h>

// Local includes.

#include "pluginsversion.h"
#include "rawfiles.h"
#include "exiv2iface.h"
#include "dcrawbinary.h"
#include "dcrawiface.h"
#include "dcrawiface.moc"

namespace KIPIRawConverterPlugin
{

class DcrawIfacePriv
{
public:

    DcrawIfacePriv()
    {
        cancel     = false;
        running    = false;
        normalExit = false;
        process    = 0;
        queryTimer = 0;
        data       = 0;
        dataPos    = 0;
        width      = 0;
        height     = 0;
        rgbmax     = 0;
    }

    bool                cancel;
    bool                running;
    bool                normalExit;

    uchar              *data;
    
    int                 dataPos;
    int                 width;
    int                 height;
    int                 rgbmax;

    QString             filePath;

    QMutex              mutex;
    
    QWaitCondition      condVar;

    QTimer             *queryTimer;
    
    KProcess           *process;

    RawDecodingSettings rawDecodingSettings;
};

DcrawIface::DcrawIface()
{
    d = new DcrawIfacePriv;
}

DcrawIface::~DcrawIface()
{
    cancel();
    delete d;
}

void DcrawIface::cancel()
{
    d->cancel = true;
}

bool DcrawIface::loadDcrawPreview(QImage& image, const QString& path)
{
    FILE       *f=NULL;
    QByteArray  imgData;
    const int   MAX_IPC_SIZE = (1024*32);
    char        buffer[MAX_IPC_SIZE];
    QFile       file;
    Q_LONG      len;
    QCString    command;

    QFileInfo fileInfo(path);
    QString   rawFilesExt(kipi_raw_file_extentions);

    if (!fileInfo.exists() || !rawFilesExt.upper().contains( fileInfo.extension().upper() ))
        return false;

    // Try to extract embedded thumbnail using dcraw with options:
    // -c : write to stdout
    // -e : Extract the camera-generated thumbnail, not the raw image (JPEG or a PPM file).
    // Note : this code require at least dcraw version 8.x

    command  = "dcraw -c -e ";
    command += QFile::encodeName( KProcess::quote( path ) );
    kdDebug( 51000 ) << "Running dcraw command " << command << endl;

    f = popen( command.data(), "r" );

    if ( f == NULL )
        return false;

    file.open( IO_ReadOnly,  f );

    while ((len = file.readBlock(buffer, MAX_IPC_SIZE)) != 0)
    {
        if ( len == -1 )
        {
            file.close();
            return false;
        }
        else
        {
            int oldSize = imgData.size();
            imgData.resize( imgData.size() + len );
            memcpy(imgData.data()+oldSize, buffer, len);
        }
    }

    file.close();
    pclose( f );

    if ( !imgData.isEmpty() )
    {
        if (image.loadFromData( imgData ))
        {
            kdDebug() << "Using embedded RAW preview extraction" << endl;
            return true;
        }
    }

    // In second, try to use simple RAW extraction method in 8 bits ppm output.
    // -c : write to stdout
    // -h : Half-size color image (3x faster than -q)
    // -a : Use automatic white balance
    // -w : Use camera white balance, if possible

    f=NULL;
    command  = "dcraw -c -h -w -a ";
    command += QFile::encodeName( KProcess::quote( path ) );
    kdDebug( 51000 ) << "Running dcraw command " << command << endl;

    f = popen( command.data(), "r" );

    if ( f == NULL )
        return false;

    file.open( IO_ReadOnly,  f );

    while ((len = file.readBlock(buffer, MAX_IPC_SIZE)) != 0)
    {
        if ( len == -1 )
        {
            file.close();
            return false;
        }
        else
        {
            int oldSize = imgData.size();
            imgData.resize( imgData.size() + len );
            memcpy(imgData.data()+oldSize, buffer, len);
        }
    }

    file.close();
    pclose( f );

    if ( !imgData.isEmpty() )
    {
        if (image.loadFromData( imgData ))
        {
            kdDebug() << "Using reduced RAW picture extraction" << endl;
            return true;
        }
    }

    return false;
}

bool DcrawIface::rawFileIdentify(QString& identify, const QString& path)
{
    FILE       *f=NULL;
    QByteArray  txtData;
    const int   MAX_IPC_SIZE = (1024*32);
    char        buffer[MAX_IPC_SIZE];
    QFile       file;
    Q_LONG      len;
    QCString    command;

    QFileInfo fileInfo(path);
    QString   rawFilesExt(kipi_raw_file_extentions);

    if (!fileInfo.exists())
    {
        identify = i18n("Cannot open RAW file");
        return false;
    }

    if (!rawFilesExt.upper().contains( fileInfo.extension().upper() ))
    {
        identify = i18n("Not a RAW file");
        return false;
    }

    // Try to get camera maker/model using dcraw with options:
    // -c : write to stdout
    // -i : identify files without decoding them.

    command  = "dcraw -c -i ";
    command += QFile::encodeName( KProcess::quote( path ) );
    kdDebug( 51000 ) << "Running dcraw command " << command << endl;

    f = popen( command.data(), "r" );

    if ( f == NULL )
    {
        identify = i18n("Cannot start dcraw instance");
        return false;
    }

    file.open( IO_ReadOnly,  f );

    while ((len = file.readBlock(buffer, MAX_IPC_SIZE)) != 0)
    {
        if ( len == -1 )
        {
            identify = i18n("Cannot identify RAW file");
            return false;
        }
        else
        {
            int oldSize = txtData.size();
            txtData.resize( txtData.size() + len );
            memcpy(txtData.data()+oldSize, buffer, len);
        }
    }

    file.close();
    pclose( f );
    identify = QString(txtData);

    if ( identify.isEmpty() )
    {
        identify = i18n("Cannot identify RAW file");
        return false;
    }

    identify.remove(path);
    identify.remove(" is a ");
    identify.remove(" image.");

    return true;
}

QByteArray DcrawIface::getICCProfilFromFile(RawDecodingSettings::OutputColorSpace colorSpace)
{    
    QString filePath;
    KGlobal::dirs()->addResourceType("profiles", KGlobal::dirs()->kde_default("data") + "kipiplugin_rawconverter/profiles");

    switch(colorSpace)
    {
        case RawDecodingSettings::SRGB:
        {
            filePath = KGlobal::dirs()->findResourceDir("profiles", "srgb.icm");
            filePath.append("srgb.icm");
            break;
        }
        case RawDecodingSettings::ADOBERGB:
        {
            filePath = KGlobal::dirs()->findResourceDir("profiles", "adobergb.icm");
            filePath.append("adobergb.icm");
            break;
        }
        case RawDecodingSettings::WIDEGAMMUT:
        {
            filePath = KGlobal::dirs()->findResourceDir("profiles", "widegamut.icm");
            filePath.append("widegamut.icm");
            break;
        }
        case RawDecodingSettings::PROPHOTO:
        {
            filePath = KGlobal::dirs()->findResourceDir("profiles", "prophoto.icm");
            filePath.append("prophoto.icm");
            break;
        }
        default:
            break;
    }

    if ( filePath.isEmpty() ) 
        return QByteArray();

    QFile file(filePath);
    if ( !file.open(IO_ReadOnly) ) 
        return QByteArray();
    
    QByteArray data(file.size());
    QDataStream stream( &file );
    stream.readRawBytes(data.data(), data.size());
    file.close();
    return data;
}

bool DcrawIface::decodeHalfRAWImage(const QString& filePath, QString& destPath,
                                    RawDecodingSettings rawDecodingSettings)
{
    d->rawDecodingSettings = rawDecodingSettings;
    d->rawDecodingSettings.halfSizeColorImage = true;
    d->rawDecodingSettings.outputFileFormat   = RawDecodingSettings::PPM;
    return (loadFromDcraw(filePath, destPath));
}

bool DcrawIface::decodeRAWImage(const QString& filePath, QString& destPath,
                                RawDecodingSettings rawDecodingSettings)
{
    d->rawDecodingSettings = rawDecodingSettings;
    return (loadFromDcraw(filePath, destPath));
}

// ----------------------------------------------------------------------------------

bool DcrawIface::loadFromDcraw(const QString& filePath, QString& destPath)
{
    d->filePath   = filePath;
    d->running    = true;
    d->normalExit = false;
    d->cancel     = false;
    d->process    = 0;
    d->data       = 0;
    d->dataPos    = 0;
    d->width      = 0;
    d->height     = 0;
    d->rgbmax     = 0;

    // trigger startProcess and loop to wait dcraw decoding
    QApplication::postEvent(this, new QCustomEvent(QEvent::User));

    while (d->running && !d->cancel)
    {
        // Waiting for dcraw, is running
        QMutexLocker lock(&d->mutex);
        d->condVar.wait(&d->mutex, 10);
    }

    if (!d->normalExit || d->cancel)
    {
        delete [] d->data;
        d->data = 0;
        return false;
    }

    // -------------------------------------------------------------------
    // Get image data and write it into destination file.

    QByteArray ICCColorProfile = getICCProfilFromFile(d->rawDecodingSettings.outputColorSpace);
    QString soft = QString("Kipi Raw Converter v.%1").arg(kipiplugins_version);
    QFileInfo fi(filePath);
    destPath = fi.dirPath(true) + QString("/") + ".kipi-rawconverter-tmp-" 
                                + QString::number(::time(0));

    switch(d->rawDecodingSettings.outputFileFormat)
    {
        case RawDecodingSettings::JPEG:
        {
            FILE* f = 0;
            f = fopen(QFile::encodeName(destPath), "wb");
    
            if (!f) 
            {
                kdDebug( 51000 ) << "Failed to open JPEG file for writing"
                                 << endl;
                delete [] d->data;
                d->data = 0;
                return false;
            }
    
            struct jpeg_compress_struct cinfo;
            struct jpeg_error_mgr       jerr;
    
            int      row_stride;
            JSAMPROW row_pointer[1];

            // Init JPEG compressor.    
            cinfo.err = jpeg_std_error(&jerr);
            jpeg_create_compress(&cinfo);
            jpeg_stdio_dest(&cinfo, f);
            cinfo.image_width      = d->width;
            cinfo.image_height     = d->height;
            cinfo.input_components = 3;
            cinfo.in_color_space   = JCS_RGB;
            jpeg_set_defaults(&cinfo);
            // B.K.O #130996: set horizontal and vertical Subsampling factor 
            // to 1 for a best quality of color picture compression. 
            cinfo.comp_info[0].h_samp_factor = 1;
            cinfo.comp_info[0].v_samp_factor = 1; 
            jpeg_set_quality(&cinfo, 100, true);
            jpeg_start_compress(&cinfo, true);

            // Write ICC color profil.
            if (!ICCColorProfile.isEmpty())
                write_icc_profile (&cinfo, (JOCTET *)ICCColorProfile.data(), ICCColorProfile.size());

            // Write image data
            row_stride = cinfo.image_width * 3;
            while (!d->cancel && (cinfo.next_scanline < cinfo.image_height))
            {
                row_pointer[0] = d->data + (cinfo.next_scanline * row_stride);
                jpeg_write_scanlines(&cinfo, row_pointer, 1);
            }
            
            jpeg_finish_compress(&cinfo);
            fclose(f);

            // Metadata restoration.
            Exiv2Iface exiv2Ifave;
            exiv2Ifave.load(filePath);
            exiv2Ifave.setImageProgramId(QString("Kipi Raw Converter"), QString(kipiplugins_version));
            exiv2Ifave.save(destPath);
            break;
        }
        case RawDecodingSettings::PNG:
        {
            FILE* f = 0;
            f = fopen(QFile::encodeName(destPath), "wb");
    
            if (!f) 
            {
                kdDebug( 51000 ) << "Failed to open PNG file for writing"
                                 << endl;
                delete [] d->data;
                d->data = 0;
                return false;
            }
    
            png_color_8 sig_bit;
            png_bytep   row_ptr;
            png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
            png_infop info_ptr  = png_create_info_struct(png_ptr);
            png_init_io(png_ptr, f);
            png_set_IHDR(png_ptr, info_ptr, d->width, d->height, 8, 
                        PNG_COLOR_TYPE_RGB,        PNG_INTERLACE_NONE, 
                        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
            sig_bit.red   = 8;
            sig_bit.green = 8;
            sig_bit.blue  = 8;
            sig_bit.alpha = 8;
            png_set_sBIT(png_ptr, info_ptr, &sig_bit);
            png_set_compression_level(png_ptr, 9);

            // Write ICC profil.
            if (!ICCColorProfile.isEmpty())
            {
                png_set_iCCP(png_ptr, info_ptr, "icc", PNG_COMPRESSION_TYPE_BASE, 
                             ICCColorProfile.data(), ICCColorProfile.size());
            }    

            QString libpngver(PNG_HEADER_VERSION_STRING);
            libpngver.replace('\n', ' ');
            soft.append(QString(" (%1)").arg(libpngver));
            png_text text;
            text.key  = "Software";
            text.text = (char *)soft.ascii();
            text.compression = PNG_TEXT_COMPRESSION_zTXt;
            png_set_text(png_ptr, info_ptr, &(text), 1);

            // Metadata restoration.
            Exiv2Iface exiv2Ifave;
            exiv2Ifave.load(filePath);
            exiv2Ifave.setImageProgramId(QString("Kipi Raw Converter"), QString(kipiplugins_version));

            // Store Exif data.
            QByteArray ba = exiv2Ifave.getExif();
            const uchar ExifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};
            QByteArray profile = QByteArray(ba.size() + sizeof(ExifHeader));
            memcpy(profile.data(), ExifHeader, sizeof(ExifHeader));
            memcpy(profile.data()+sizeof(ExifHeader), ba.data(), ba.size());
            writeRawProfile(png_ptr, info_ptr, "exif", profile.data(), (png_uint_32) profile.size());

            // Store Iptc data.
            QByteArray ba2 = exiv2Ifave.getIptc();
            writeRawProfile(png_ptr, info_ptr, "iptc", ba2.data(), (png_uint_32) ba2.size());

            png_write_info(png_ptr, info_ptr);
            png_set_shift(png_ptr, &sig_bit);
            png_set_packing(png_ptr);
            unsigned char* ptr = d->data;
    
            for (int y = 0; !d->cancel && (y < d->height); y++)
            {
                row_ptr = (png_bytep) ptr;
                png_write_rows(png_ptr, &row_ptr, 1);
                ptr += (d->width * 3);
            }
    
            png_write_end(png_ptr, info_ptr);
            png_destroy_write_struct(&png_ptr, (png_infopp) & info_ptr);
            png_destroy_info_struct(png_ptr, (png_infopp) & info_ptr);
            fclose(f);
            break;
        }
        case RawDecodingSettings::TIFF:
        {
            TIFF          *tif=0;
            unsigned char *data=0;
            int            y;
            int            w;
            
            tif = TIFFOpen(QFile::encodeName(destPath), "wb");
    
            if (!tif) 
            {
                kdDebug( 51000 ) << "Failed to open TIFF file for writing"
                                 << endl;
                delete [] d->data;
                d->data = 0;
                return false;
            }
    
            TIFFSetField(tif, TIFFTAG_IMAGEWIDTH,      d->width);
            TIFFSetField(tif, TIFFTAG_IMAGELENGTH,     d->height);
            TIFFSetField(tif, TIFFTAG_ORIENTATION,     ORIENTATION_TOPLEFT);
            TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE,   8);
            TIFFSetField(tif, TIFFTAG_PLANARCONFIG,    PLANARCONFIG_CONTIG);
            TIFFSetField(tif, TIFFTAG_COMPRESSION,     COMPRESSION_ADOBE_DEFLATE);
            TIFFSetField(tif, TIFFTAG_ZIPQUALITY,      9);
            // NOTE : this tag values aren't defined in libtiff 3.6.1. '2' is PREDICTOR_HORIZONTAL.
            //        Use horizontal differencing for images which are
            //        likely to be continuous tone. The TIFF spec says that this
            //        usually leads to better compression.
            //        See this url for more details:
            //        http://www.awaresystems.be/imaging/tiff/tifftags/predictor.html
            TIFFSetField(tif, TIFFTAG_PREDICTOR,       2); 
            TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
            TIFFSetField(tif, TIFFTAG_PHOTOMETRIC,     PHOTOMETRIC_RGB);
            w = TIFFScanlineSize(tif);
            TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,    TIFFDefaultStripSize(tif, 0));


            // Metadata restoration.
            Exiv2Iface exiv2Ifave;
            exiv2Ifave.load(filePath);
            exiv2Ifave.setImageProgramId(QString("Kipi Raw Converter"), QString(kipiplugins_version));

            // Store Exif data.
            // TODO

            // Store Iptc data.
            QByteArray ba2 = exiv2Ifave.getIptc(true);
#if defined(TIFFTAG_PHOTOSHOP)
            TIFFSetField (tif, TIFFTAG_PHOTOSHOP,      (uint32)ba2.size(), (uchar *)ba2.data());
#endif

            QString libtiffver(TIFFLIB_VERSION_STR);
            libtiffver.replace('\n', ' ');
            soft.append(QString(" ( %1 )").arg(libtiffver));
            TIFFSetField(tif, TIFFTAG_SOFTWARE,        (const char*)soft.ascii());

            // Write ICC profil.
            if (!ICCColorProfile.isEmpty())
            {
#if defined(TIFFTAG_ICCPROFILE)    
                TIFFSetField(tif, TIFFTAG_ICCPROFILE, (uint32)ICCColorProfile.size(), 
                             (uchar *)ICCColorProfile.data());
#endif      
            }    

            // Write image data
            for (y = 0; !d->cancel && (y < d->height); y++)
            {
                data = d->data + (y * d->width * 3);
                TIFFWriteScanline(tif, data, y, 0);
            }
    
            TIFFClose(tif);
            break;
        }
        case RawDecodingSettings::PPM:
        {
            FILE* f = fopen(QFile::encodeName(destPath), "wb");
            if (!f) 
            {
                kdDebug( 51000 ) << "Failed to open ppm file for writing"
                                 << endl;
                delete [] d->data;
                d->data = 0;
                return false;
            }
    
            fprintf(f, "P6\n%d %d\n255\n", d->width, d->height);
            fwrite(d->data, 1, d->width*d->height*3, f);
            fclose(f);
            break;
        }
        default:
        {
            kdDebug( 51000 ) << "Invalid output file format"
                             << endl;
            delete [] d->data;
            d->data = 0;
            return false;
        }
    }
    
    delete [] d->data;
    d->data = 0;

    if (d->cancel)
    {
        ::remove(QFile::encodeName(destPath));
        return false;
    }

    return true;
}

void DcrawIface::customEvent(QCustomEvent *)
{
    // KProcess (because of QSocketNotifier) is not reentrant.
    // We must only use it from the main thread.
    startProcess();

    // set up timer to call continueQuery at regular intervals
    if (d->running)
    {
        d->queryTimer = new QTimer;
        connect(d->queryTimer, SIGNAL(timeout()),
                this, SLOT(slotContinueQuery()));
        d->queryTimer->start(30);
    }
}

void DcrawIface::slotContinueQuery()
{
    // this is called from the timer

    if (d->cancel)
    {
        d->process->kill();
        d->process->wait();
        d->normalExit = false;
    }
}

void DcrawIface::startProcess()
{
    if (d->cancel)
    {
        d->running    = false;
        d->normalExit = false;
        return;
    }

    // create KProcess and build argument list

    d->process = new KProcess;

    connect(d->process, SIGNAL(processExited(KProcess *)),
            this, SLOT(slotProcessExited(KProcess *)));
             
    connect(d->process, SIGNAL(receivedStdout(KProcess *, char *, int)),
            this, SLOT(slotReceivedStdout(KProcess *, char *, int)));
             
    connect(d->process, SIGNAL(receivedStderr(KProcess *, char *, int)),
            this, SLOT(slotReceivedStderr(KProcess *, char *, int)));

    // run dcraw with options:
    // -c : write to stdout
    //
    // -4 : 16bit ppm output
    //
    // -f : Interpolate RGB as four colors. This blurs the image a little, but it eliminates false 2x2 mesh patterns.
    // -a : Use automatic white balance
    // -w : Use camera white balance, if possible
    // -n : Don't clip colors
    // -s : Use secondary pixels (Fuji Super CCD SR only)
    // -q : Use simple bilinear interpolation for quick results. Warning: this option require arguments 
    //      depending dcraw version (look below)
    // -B : Use bilateral filter to smooth noise while preserving edges.
    // -p : Use the input ICC profiles to define the camera's raw colorspace.
    // -o : Use ICC profiles to define the output colorspace.
    // -h : Output a half-size color image. Twice as fast as -q 0.

    *d->process << DcrawBinary::path();
    *d->process << "-c";

    if (d->rawDecodingSettings.sixteenBitsImage)
        *d->process << "-4";

    if (d->rawDecodingSettings.halfSizeColorImage)
        *d->process << "-h";

    if (d->rawDecodingSettings.cameraColorBalance)
        *d->process << "-w";

    if (d->rawDecodingSettings.automaticColorBalance)
        *d->process << "-a";

    if (d->rawDecodingSettings.RGBInterpolate4Colors)
        *d->process << "-f";

    if (d->rawDecodingSettings.SuperCCDsecondarySensor)
        *d->process << "-s";

    *d->process << "-H";
    *d->process << QString::number(d->rawDecodingSettings.unclipColors);

    *d->process << "-b";
    *d->process << QString::number(d->rawDecodingSettings.brightness);

    *d->process << "-q";
    *d->process << QString::number(d->rawDecodingSettings.RAWQuality);

    if (d->rawDecodingSettings.enableNoiseReduction)
    {
        *d->process << "-B";
        *d->process << QString::number(d->rawDecodingSettings.NRSigmaDomain);
        *d->process << QString::number(d->rawDecodingSettings.NRSigmaRange);
    }

    *d->process << "-o";
    *d->process << QString::number( d->rawDecodingSettings.outputColorSpace );

    *d->process << QFile::encodeName( d->filePath );
    kdDebug() << "Running dcraw command " << d->process->args() << endl;

    // actually start the process
    if ( !d->process->start(KProcess::NotifyOnExit, 
         KProcess::Communication(KProcess::Stdout | KProcess::Stderr)) )
    {
        kdError() << "Failed to start dcraw" << endl;
        delete d->process;
        d->process    = 0;
        d->running    = false;
        d->normalExit = false;
        return;
    }
}

void DcrawIface::slotProcessExited(KProcess *)
{
    // set variables, clean up, wake up loader thread

    QMutexLocker lock(&d->mutex);
    d->running    = false;
    d->normalExit = d->process->normalExit();
    delete d->process;
    d->process = 0;
    delete d->queryTimer;
    d->queryTimer = 0;
    d->condVar.wakeAll();
}

void DcrawIface::slotReceivedStdout(KProcess *, char *buffer, int buflen)
{
    if (!d->data)
    {
        // first data packet:
        // Parse PPM header to find out size and allocate buffer

        // PPM header is "P6 <width> <height> <maximum rgb value "
        // where the blanks are newline characters

        QString magic = QString::fromAscii(buffer, 2);
        if (magic != "P6") 
        {
            kdError() << "Cannot parse header from dcraw: Magic is " << magic << endl;
            d->process->kill();
            return;
        }

        // Find the third newline that marks the header end in a dcraw generated ppm.
        int i       = 0;
        int counter = 0;

        while (i < buflen) 
        {
            if (counter == 3) break;
            if (buffer[i] == '\n') 
            {
                counter++;
            }
            ++i;
        }

        QStringList splitlist = QStringList::split("\n", QString::fromAscii(buffer, i));
        //kdDebug() << "Header: " << QString::fromAscii(buffer, i) << endl;
        QStringList sizes = QStringList::split(" ", splitlist[1]);
        if (splitlist.size() < 3 || sizes.size() < 2)
        {
            kdError() << "Cannot parse header from dcraw: Could not split" << endl;
            d->process->kill();
            return;
        }

        d->width  = sizes[0].toInt();
        d->height = sizes[1].toInt();
        d->rgbmax = splitlist[2].toInt();

#ifdef ENABLE_DEBUG_MESSAGES
        kdDebug() << "Parsed PPM header: width " << d->width << " height " 
                  << d->height << " rgbmax " << d->rgbmax << endl;
#endif

        // cut header from data for memcpy below
        buffer += i;
        buflen -= i;

        // allocate buffer
        d->data    = new uchar[d->width * d->height * (d->rawDecodingSettings.sixteenBitsImage ? 6 : 3)];
        d->dataPos = 0;
    }

    // copy data to buffer
    memcpy(d->data + d->dataPos, buffer, buflen);
    d->dataPos += buflen;
}

void DcrawIface::slotReceivedStderr(KProcess *, char *buffer, int buflen)
{
    QCString message(buffer, buflen);
    kdDebug() << "Dcraw StdErr: " << message << endl;
}

void DcrawIface::writeRawProfile(png_struct *ping, png_info *ping_info, char *profile_type, 
                                 char *profile_data, png_uint_32 length)
{
    png_textp      text;
    
    register long  i;
    
    uchar         *sp;
    
    png_charp      dp;
    
    png_uint_32    allocated_length, description_length;

    const uchar hex[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
    
    kdDebug() << "Writing Raw profile: type=" << profile_type << ", length=" << length << endl;
    
    text               = (png_textp) png_malloc(ping, (png_uint_32) sizeof(png_text));
    description_length = strlen((const char *) profile_type);
    allocated_length   = (png_uint_32) (length*2 + (length >> 5) + 20 + description_length);
    
    text[0].text   = (png_charp) png_malloc(ping, allocated_length);
    text[0].key    = (png_charp) png_malloc(ping, (png_uint_32) 80);
    text[0].key[0] = '\0';
    
    concatenateString(text[0].key, "Raw profile type ", 4096);
    concatenateString(text[0].key, (const char *) profile_type, 62);
    
    sp = (uchar*)profile_data;
    dp = text[0].text;
    *dp++='\n';
    
    copyString(dp, (const char *) profile_type, allocated_length);
    
    dp += description_length;
    *dp++='\n';
    
    formatString(dp, allocated_length-strlen(text[0].text), "%8lu ", length);
    
    dp += 8;
    
    for (i=0; i < (long) length; i++)
    {
        if (i%36 == 0)
            *dp++='\n';

        *(dp++)=(char) hex[((*sp >> 4) & 0x0f)];
        *(dp++)=(char) hex[((*sp++ ) & 0x0f)]; 
    }

    *dp++='\n';
    *dp='\0';
    text[0].text_length = (png_size_t) (dp-text[0].text);
    text[0].compression = -1;

    if (text[0].text_length <= allocated_length)
        png_set_text(ping, ping_info,text, 1);

    png_free(ping, text[0].text);
    png_free(ping, text[0].key);
    png_free(ping, text);
}

size_t DcrawIface::concatenateString(char *destination, const char *source, const size_t length)
{
    register char       *q;
    
    register const char *p;
    
    register size_t      i;
    
    size_t               count;
  
    if ( !destination || !source || length == 0 )
        return 0;

    p = source;
    q = destination;
    i = length;

    while ((i-- != 0) && (*q != '\0'))
        q++;

    count = (size_t) (q-destination);
    i     = length-count;

    if (i == 0)
        return(count+strlen(p));

    while (*p != '\0')
    {
        if (i != 1)
        {
            *q++=(*p);
            i--;
        }
        p++;
    }

    *q='\0';
    
    return(count+(p-source));
}

size_t DcrawIface::copyString(char *destination, const char *source, const size_t length)
{
    register char       *q;
    
    register const char *p;
    
    register size_t      i;
        
    if ( !destination || !source || length == 0 )
        return 0;

    p = source;
    q = destination;
    i = length;

    if ((i != 0) && (--i != 0))
    {
        do
        {
            if ((*q++=(*p++)) == '\0')
                break;
        } 
        while (--i != 0);
    }

    if (i == 0)
    {
        if (length != 0)
            *q='\0';
  
        while (*p++ != '\0');
    }
    
    return((size_t) (p-source-1));
}

long DcrawIface::formatString(char *string, const size_t length, const char *format,...)
{
    long n;
    
    va_list operands;
    
    va_start(operands,format);
    n = (long) formatStringList(string, length, format, operands);
    va_end(operands);
    return(n);
}

long DcrawIface::formatStringList(char *string, const size_t length, const char *format, va_list operands)
{
    int n = vsnprintf(string, length, format, operands);
    
    if (n < 0)
        string[length-1] = '\0';
    
    return((long) n);
}

}  // namespace KIPIRawConverterPlugin
