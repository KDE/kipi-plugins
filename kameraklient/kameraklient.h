/* ============================================================
 * File  : kameraklient.h
 * Author: Tudor Calin <tudor@1xtech.com>
 * Date  : 2004-06-18
 * Description : 
 * 
 * Copyright 2004 by Tudor Calin

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

#ifndef PLUGIN_KAMERAKLIENT_H
#define PLUGIN_KAMERAKLIENT_H

#include <libkipi/plugin.h>

namespace KIPIKameraKlientPlugin
{
class CameraUI;
class CameraType;
}

class Plugin_KameraKlient : public KIPI::Plugin {
Q_OBJECT

public:
    Plugin_KameraKlient(QObject *parent, const char* name, const QStringList& args);
    ~Plugin_KameraKlient();
	virtual void setup(QWidget* widget);
    virtual KIPI::Category category(KAction*) const;
    QString id() const {
		return QString::fromLatin1("kameraklient");
    }
	KAction* mKameraKlientAction;

protected slots:
    void slotActivate();
};

#endif

