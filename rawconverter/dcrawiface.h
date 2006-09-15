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

#ifndef DCRAWIFACE_H
#define DCRAWIFACE_H

// C Ansi includes.

extern "C"
{
#include <png.h>
}

// Qt Includes.

#include <qstring.h>
#include <qobject.h>
#include <qimage.h>

// Local includes.

#include "rawdecodingsettings.h"

class QCustomEvent;

class KProcess;

namespace KIPIRawConverterPlugin
{
class DcrawIfacePriv;

class DcrawIface : public QObject
{
    Q_OBJECT

public:  // Fast non cancelable methods.

    /** Get the embedded preview image in RAW file
    */
    bool loadDcrawPreview(QImage& image, const QString& path);

    /** Get the camera model witch have taken RAW file
    */ 
    bool rawFileIdentify(QString& identify, const QString& path);

public:

    DcrawIface();
    ~DcrawIface();

public: // Cancelable methods to extract RAW data. dcraw decoding can take a while.

    /** To cancel 'decodeHalfRAWImage' and 'decodeRAWImage' methods running 
        in a separate thread.
    */
    void cancel();

    /** Extract a small size of decode RAW data in 8 bits/color/pixels 
        using sRGB color space.
    */
    bool decodeHalfRAWImage(const QString& filePath, QString& destPath,
                            RawDecodingSettings rawDecodingSettings);

    /** Extract a full size of RAW data in 8 bits/color/pixels using 
        sRGB color space.
    */
    bool decodeRAWImage(const QString& filePath, QString& destPath,
                        RawDecodingSettings rawDecodingSettings);

private:

    QByteArray getICCProfilFromFile(RawDecodingSettings::OutputColorSpace colorSpace);

    bool loadFromDcraw(const QString& filePath, QString& destPath);
    void startProcess();

    virtual void customEvent(QCustomEvent *);

    void   writeRawProfile(png_struct *ping, png_info *ping_info, char *profile_type, 
                           char *profile_data, png_uint_32 length);

    size_t concatenateString(char *destination, const char *source, const size_t length);
    size_t copyString(char *destination, const char *source, const size_t length);
    long   formatString(char *string, const size_t length, const char *format,...);
    long   formatStringList(char *string, const size_t length, const char *format, va_list operands);

private slots:

    void slotProcessExited(KProcess *);
    void slotReceivedStdout(KProcess *, char *buffer, int buflen);
    void slotReceivedStderr(KProcess *, char *buffer, int buflen);
    void slotContinueQuery();

private:

    DcrawIfacePriv *d;
};

}  // namespace KIPIRawConverterPlugin

#endif /* DCRAWIFACE_H */
