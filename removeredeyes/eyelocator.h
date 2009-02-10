/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-06-08
 * Description : the eyelocator detects and removes red eyes
 *               from images
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at gmx dot net>
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

#ifndef EYELOCATOR_H
#define EYELOCATOR_H

class QString;

namespace KIPIRemoveRedEyesPlugin
{

struct EyeLocatorPriv;

class EyeLocator
{

public:

    enum PreviewFileType
    {
        Final = 0,
        OriginalPreview,
        CorrectedPreview,
        MaskPreview
    };

public:

    EyeLocator(const QString& filename, const QString& clsFilename);
    ~EyeLocator();

public:

    void startCorrection(bool scaleDown);

    void saveImage(const QString& path, PreviewFileType type);

    int  redEyes() const;

    double scaleFactor() const;
    void   setScaleFactor(double);

    double minRoundness() const;
    void   setMinRoundness(double);

    int  neighborGroups() const;
    void setNeighborGroups(int);

    int  minBlobsize() const;
    void setMinBlobsize(int);

private:

    EyeLocatorPriv* const d;
};

} // namespace KIPIRemoveRedEyesPlugin

#endif
