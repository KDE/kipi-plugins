/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to test bindings to QML
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

#ifndef PLUGIN_QMLIMAGEVIEWER_H
#define PLUGIN_QMLIMAGEVIEWER_H

// Qt includes

#include <QVariant>
#include <QObject>
#include <QAction>

// Libkipi includes

#include <KIPI/Plugin>

class QAction;
class KPQmlImageViewer;

class Plugin_QmlImageViewer : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_QmlImageViewer(QObject* const parent, const QVariantList& args);
    ~Plugin_QmlImageViewer();

    void setup(QWidget* const);

public Q_SLOTS:

    void slotView();

private:

    void setupActions();
    QString getTempDirPath();

private:

    QAction*       m_actionView = 0;

    KPQmlImageViewer*      m_viewer = 0;
};

#endif
