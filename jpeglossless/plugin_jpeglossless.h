/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-09-26
 * Description : loss less images transformations plugin.
 *
 * Copyright (C) 2003-2005 by Renchi Raju & Gilles Caulier <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// LibKipi includes.

#include <libkipi/plugin.h>
#include <libkipi/imagecollection.h>

class QCustomEvent;
class KActionMenu;
class KAction;

namespace KIPIJPEGLossLessPlugin
{
class ActionThread;
class ProgressDlg;
}

class Plugin_JPEGLossless : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_JPEGLossless(QObject *parent, const char* name, const QStringList &args);
    ~Plugin_JPEGLossless();

    virtual KIPI::Category category( KAction* action ) const;
    virtual void setup( QWidget* widget );

protected:

    void customEvent(QCustomEvent *event);
    KURL::List images();

private slots:

    void slotRotate();
    void slotFlip();
    void slotConvert2GrayScale();
    void slotCancel();

private:

    bool                                  m_failed;

    int                                   m_total;
    int                                   m_current;

    KAction                              *m_action_Convert2GrayScale;
    KAction                              *m_action_AutoExif;

    KActionMenu                          *m_action_RotateImage;
    KActionMenu                          *m_action_FlipImage;

    KURL::List                            m_images;

    KIPI::BatchProgressDialog            *m_progressDlg;

    KIPIJPEGLossLessPlugin::ActionThread *m_thread;
};

#endif /* PLUGIN_JPEGLOSSLESS_H */
