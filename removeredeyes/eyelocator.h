/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-06-08
 * Description : the eyelocator detects and removes red eyes
 *               from images
 *
 * Copyright 2008 by Andi Clemens <andi dot clemens at gmx dot net>
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

class EyeLocatorPriv;

class EyeLocator
{
public:

    EyeLocator(const char* filename,
               const char* classifierFile,
               double scaleFactor,
               int neighborGroups,
               double minRoundness,
               int minBlobsize);
    ~EyeLocator();

public:

    int     redEyes() const;
    void    saveImage(const char* path);

private:

    EyeLocatorPriv* const d;
};
} // namespace KIPIRemoveRedEyesPlugin

#endif
