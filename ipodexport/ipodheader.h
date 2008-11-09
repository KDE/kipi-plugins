/***************************************************************************
 * copyright            : (C) 2006 Seb Ruiz <me@sebruiz.net>               *
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IPOD_HEADER_H
#define IPOD_HEADER_H

#include <q3frame.h> //baseclass
//Added by qt3to4:
#include <QLabel>

class QLabel;
class KPushButton;

namespace IpodExport
{

class IpodHeader : public Q3Frame
{
    Q_OBJECT

    public:
        IpodHeader( QWidget *parent=0, const char *name=0, Qt::WFlags f=0 );
        ~IpodHeader() { }

        enum ViewType { NoIpod, IncompatibleIpod, ValidIpod };

        void setViewType( ViewType view );
        ViewType viewType() const         { return m_viewType; }

    signals:
        void refreshDevices();
        void updateSysInfo();

    private:
        void setNoIpod();
        void setIncompatibleIpod();
        void setValidIpod();

        ViewType m_viewType;

        KPushButton *m_button;
        QLabel      *m_messageLabel;

};

}

#endif /* IPOD_HEADER_H */
