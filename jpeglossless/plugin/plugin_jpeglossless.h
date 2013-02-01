/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-09-26
 * Description : loss less images transformations plugin.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2004-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PLUGIN_JPEGLOSSLESS_H
#define PLUGIN_JPEGLOSSLESS_H

// KDE includes

#include <kurl.h>

// LibKIPI includes

#include <libkipi/plugin.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "actions.h"

using namespace KIPI;

namespace KIPIJPEGLossLessPlugin
{

class Plugin_JPEGLossless : public Plugin
{
    Q_OBJECT

public:

    Plugin_JPEGLossless(QObject* const parent, const QVariantList& args);
    ~Plugin_JPEGLossless();

    void setup(QWidget* const widget);

protected:

    KUrl::List images();

private Q_SLOTS:

    void slotFlipHorizontally();
    void slotFlipVertically();
    void slotRotateRight();
    void slotRotateLeft();
    void slotRotateExif();
    void slotConvert2GrayScale();
    void slotCancel();
    void slotStarting(const KUrl& url, int action);
    void slotFinished(const KUrl& url, int action);
    void slotFailed(const KUrl& url, int action, const QString& errString);

private:

    void flip(FlipAction action, const QString& title);
    void rotate(RotateAction action, const QString& title);
    void oneTaskCompleted();
    void setupActions();

private:

    class Private;
    Private* const d;
};

} // namespace KIPIJPEGLossLessPlugin

#endif /* PLUGIN_JPEGLOSSLESS_H */
