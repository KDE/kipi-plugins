/* ============================================================
 * File  : actionthread.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-12-03
 * Description :
 *
 * Copyright 2003 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef ACTIONTHREAD_H
#define ACTIONTHREAD_H

#include <qthread.h>
#include <qstringlist.h>

#include "actions.h"
#include "mtqueue.h"
#include <kurl.h>
#include <libkipi/interface.h>

class QObject;

namespace JPEGLossLess
{

class ActionThread : public QThread
{
public:

    ActionThread( KIPI::Interface* interface, QObject *parent);
    ~ActionThread();

    void rotate(const KURL::List& urlList, RotateAction val);
    void flip(const KURL::List& urlList, FlipAction val);
    void convert2grayscale(const KURL::List& urlList);
    void cancel();

protected:

    void run();

private:

    void deleteDir(const QString& dirPath);

    struct Task_ {
        QString       filePath;
        Action        action;
        RotateAction  rotAction;
        FlipAction    flipAction;
    };

    typedef struct Task_ Task;

    QObject        *parent_;
    MTQueue<Task>   taskQueue_;
    QString         tmpFolder_;
    KIPI::Interface* interface_;
};

}

#endif /* ACTIONTHREAD_H */
