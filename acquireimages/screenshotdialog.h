//////////////////////////////////////////////////////////////////////////////
//
//    SCREENSHOOTDIALOG.H
//
//    Copyright (C) Richard J. Moore 1997-2002 from KSnapshot
//    Copyright (C) Matthias Ettrich 2000 from KSnapshot
//    Copyright (C) Aaron J. Seigo 2002 from KSnapshot
//
//    Copyright (C) 2004 Gilles Caulier <caulier.gilles at free.fr>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef SCREENSHOOTDIALOG_H
#define SCREENSHOOTDIALOG_H

// Include files for Qt

#include <qimage.h>
#include <qpixmap.h>
#include <qtimer.h>

// Include files for KDE

#include <kdialogbase.h>

// Include files for X11

extern "C"
{
#include <X11/X.h>
#include <X11/Xlib.h>
}

// Include files for libKipi.

#include <libkipi/interface.h>

class QWidget;
class QCheckBox;

class KConfig;
class KIntNumInput;

namespace KIPIAcquireImagesPlugin
{

class AcquireImageDialog;

class ScreenGrabDialog : public KDialogBase
{
Q_OBJECT

public:
    ScreenGrabDialog( KIPI::Interface* interface, 
                      QWidget *parent=0, const char *name=0);
    ~ScreenGrabDialog();

protected slots:
    void slotAbout(void);
    void slotClose(void);
    void slotGrab(void);
    void slotPerformGrab(void);

protected:
    KIPI::Interface    *m_interface;
    bool                m_inSelect;
    QCheckBox          *m_desktopCB;
    QCheckBox          *m_hideCB;
    KIntNumInput       *m_delay;
    AcquireImageDialog *m_acquireImageDialog;
    QImage              m_screenshotImage;
    KConfig            *m_config;
    QWidget            *m_grabber;
    QTimer              m_grabTimer;
    QPixmap             m_snapshot;

    bool eventFilter( QObject* o, QEvent* e);
    void endGrab(void);
};

}  // NameSpace KIPIAcquireImagesPlugin

#endif  // SCREENSHOOTDIALOG_H
