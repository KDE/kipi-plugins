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

namespace KIPIRemoveRedEyesPlugin
{

struct EyeLocatorPriv;

class EyeLocator
{

public:

    enum FileType
    {
        Final = 0,
        OriginalPreview,
        CorrectedPreview,
        MaskPreview
    };

public:

    EyeLocator(const char* filename, const char* clsFilename);
    ~EyeLocator();

public:

    void startCorrection(bool scaleDown);

    void saveImage(const char* path, FileType type);

    int  redEyes() const;

    void setScaleFactor(double);
    double scaleFactor() const;

    void setMinRoundness(double);
    double minRoundness() const;

    void setNeighborGroups(int);
    int neighborGroups() const;

    void setMinBlobsize(int);
    int minBlobsize() const;

private:

    EyeLocatorPriv* const d;
};

} // namespace KIPIRemoveRedEyesPlugin

#endif
