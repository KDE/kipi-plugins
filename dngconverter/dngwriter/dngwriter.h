/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG
 *
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DNGWRITTER_H
#define DNGWRITTER_H

// Qt includes

#include <QString>

namespace DNGIface
{

class DNGWriterPrivate;

class DNGWriter
{

public:

    enum JPEGPreview
    {
        NONE = 0,
        MEDIUM,
        FULLSIZE
    };

public:

    DNGWriter();
    ~DNGWriter();

    void setInputFile(const QString& filePath);
    void setOutputFile(const QString& filePath);

    QString inputFile() const;
    QString outputFile() const;

    void setCompressLossLess(bool b);
    bool compressLossLess() const;

    void setUpdateFileDate(bool b);
    bool updateFileDate() const;

    void setBackupOriginalRawFile(bool b);
    bool backupOriginalRawFile() const;

    void setPreviewMode(int mode);
    int  previewMode() const;

    int  convert();
    void cancel();
    void reset();

private:

    DNGWriterPrivate* const d;
};

}  // namespace DNGIface

#endif /* DNGWRITTER_H */
