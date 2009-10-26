/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-12-03
 * Description : a class to manage JPEGLossLess plugin 
 *               actions using threads
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2004-2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef ACTIONTHREAD_H
#define ACTIONTHREAD_H

// Qt includes

#include <QThread>

// KDE includes

#include <kurl.h>

// Local includes

#include "actions.h"

namespace KIPI
{
    class Interface;
}

namespace KIPIJPEGLossLessPlugin
{

class ActionThreadPriv;

class ActionThread : public QThread
{
    Q_OBJECT

public:

    ActionThread(KIPI::Interface* interface, QObject *parent);
    ~ActionThread();

    void rotate(const KUrl::List& urlList, RotateAction val);
    void flip(const KUrl::List& urlList, FlipAction val);
    void convert2grayscale(const KUrl::List& urlList);
    void cancel();

protected:

    void run();

Q_SIGNALS:

    void starting(const QString& filePath, int action);
    void finished(const QString& filePath, int action);
    void failed(const QString& filePath, int action, const QString& errString);

private:

    ActionThreadPriv* const d;
};

}  // namespace KIPIJPEGLossLessPlugin

#endif /* ACTIONTHREAD_H */
