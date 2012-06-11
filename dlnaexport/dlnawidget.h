/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-05-28
 * Description : a KIPI plugin to export pics through DLNA technology.
 *
 * Copyright (C) 2012 by Smit Mehta <smit dot meh at gmail dot com>
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

#ifndef KIPIDLNAEXPORTPLUGIN_DLNAWIDGET_H
#define KIPIDLNAEXPORTPLUGIN_DLNAWIDGET_H

// Qt includes

#include <QWidget>

// Local includes

class QLabel;

class KComboBox;
class KPushButton;

namespace KIPI
{
    class Interface;
    class UploadWidget;
}

using namespace KIPI;

namespace KIPIPlugins
{
    class KPImagesList;
}

class MediaServerWindow;

using namespace KIPIPlugins;

namespace KIPIDLNAExportPlugin
{

class DLNAWidget : public QWidget
{
    Q_OBJECT

public:

    explicit DLNAWidget(Interface* const interface, const QString& tmpFolder, QWidget* const parent);
    ~DLNAWidget();

public Q_SLOTS:

    void reactivate();
    void slotSelectDirectory();

private:

    QLabel*            m_headerLbl;

    KPushButton*       m_selectBtn;

    MediaServerWindow* m_dlna;

    QLabel*            m_directoryLbl;
};

} // namespace KIPIDLNAExportPlugin

#endif // KIPIDLNAEXPORTPLUGIN_DLNAWIDGET_H
