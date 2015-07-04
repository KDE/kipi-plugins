/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2005-17-06
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef PLUGIN_FLICKR_H
#define PLUGIN_FLICKR_H

// Qt includes

#include <QVariant>
#include <QAction>

// LibKIPI includes

#include <KIPI/Plugin>

using namespace KIPI;

namespace KIPIFlickrPlugin
{

class FlickrWindow;
class SelectUserDlg;

class Plugin_Flickr : public Plugin
{
    Q_OBJECT

public:

    Plugin_Flickr(QObject* const parent, const QVariantList& args);
    ~Plugin_Flickr();

    void setup(QWidget* const);

public Q_SLOTS:

    void slotActivateFlickr();
    void slotActivate23();
    void slotActivateZooomr();

private:

    void setupActions();

private:

    QAction*       m_actionFlickr;
    QAction*       m_action23;
    QAction*       m_actionZooomr;

    FlickrWindow*  m_dlgFlickr;
    FlickrWindow*  m_dlg23;
    FlickrWindow*  m_dlgZooomr;
    
    SelectUserDlg* selectFlickr;
    SelectUserDlg* select23;
    SelectUserDlg* selectZoomr;
};

} //namespace KIPIFlickrPlugin

#endif // PLUGIN_FLICKR_H
