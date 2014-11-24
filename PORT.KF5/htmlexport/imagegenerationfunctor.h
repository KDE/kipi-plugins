/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-04-04
 * Description : A KIPI plugin to generate HTML image galleries
 *
 * Copyright (C) 2006-2010 by Aurelien Gateau <aurelien dot gateau at free.fr>
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

// vim: set tabstop=4 shiftwidth=4 noexpandtab:

#ifndef IMAGEGENERATIONFUNCTOR_H
#define IMAGEGENERATIONFUNCTOR_H

#include "uniquenamehelper.h"

namespace KIPIHTMLExport
{

class GalleryInfo;
class Generator;
class ImageElement;

/**
 * This functor generates images (full and thumbnail) for an url and returns an
 * ImageElement initialized to fill the xml writer.
 * It is used as an argument to QtConcurrent::mapped().
 */
class ImageGenerationFunctor
{
public:

    typedef ImageElement result_type;

public:

    ImageGenerationFunctor(Generator* generator, GalleryInfo* info, const QString& destDir);

    void operator()(ImageElement& element);

private:

    bool writeDataToFile(const QByteArray& data, const QString& destPath);
    void emitWarning(const QString& msg);

private:

    Generator*       mGenerator;
    GalleryInfo*     mInfo;
    QString          mDestDir;
    UniqueNameHelper mUniqueNameHelper;
};

} // namespace KIPIHTMLExport

#endif /* IMAGEGENERATIONFUNCTOR_H */
