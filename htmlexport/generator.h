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

#ifndef GENERATOR_H
#define GENERATOR_H

#include <QObject>

// Local includes

#include "kpbatchprogressdialog.h"

namespace KIPI
{
    class Interface;
}

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIHTMLExport
{

class GalleryInfo;
class ImageGenerationFunctor;

/**
 * This class is responsible for generating the HTML and scaling the images
 * according to the settings specified by the user.
 */
class Generator : public QObject
{
    Q_OBJECT

public:

    Generator(Interface* const, GalleryInfo* const, KPBatchProgressDialog* const);
    ~Generator();

    bool run();
    bool warnings() const;

    static QString webifyFileName(const QString&);

Q_SIGNALS:
    /**
     * This signal is emitted from ImageGenerationFunctor. It uses a
     * QueuedConnection to switch between the ImageGenerationFunctor thread and
     * the gui thread.
     */
    void logWarningRequested(const QString& text);

private:

    struct Private;
    Private* d;

    friend struct Private;
    friend class ImageGenerationFunctor;

private Q_SLOTS:

    void logWarning(const QString& text);
};

} // namespace KIPIPlugins

#endif /* GENERATOR_H */
