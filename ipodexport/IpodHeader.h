/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-12-05
 * Description : a tool to export image to an Ipod device.
 *
 * Copyright (C) 2006-2008 by Seb Ruiz <ruiz@kde.org>
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

#ifndef IPOD_HEADER_H
#define IPOD_HEADER_H

#include <QFrame> //Baseclass
#include <QLabel>

class KPushButton;

namespace KIPIIpodExportPlugin
{

class IpodHeader : public QFrame
{
    Q_OBJECT

    public:
        IpodHeader( QWidget *parent = 0, Qt::WFlags f = 0 );
        ~IpodHeader() {};

        enum ViewType 
        {
            NoIpod, 
            IncompatibleIpod, 
            ValidIpod
        };

        void setViewType( ViewType view );
        ViewType viewType() const { return m_viewType; };

    Q_SIGNALS:
        void refreshDevices();
        void updateSysInfo();

    private:
        void setNoIpod();
        void setIncompatibleIpod();
        void setValidIpod();

    private:
        ViewType     m_viewType;

        KPushButton *m_button;
        QLabel      *m_messageLabel;
};

} // namespace KIPIIpodExportPlugin

#endif /* IPOD_HEADER_H */
