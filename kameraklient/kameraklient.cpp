/* ============================================================
 * File  : kameraklient.cpp
 * Author: Tudor Calin <tudor@1xtech.com>
 * Date  : 2004-06-18
 * Description : 
 * 
 * Copyright 2004 by Tudor Calin

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

// Qt
#include <qiconset.h>
#include <qwidget.h>
// KDE
#include <kaboutdata.h>
#include <kaction.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kinstance.h>
#include <kglobal.h>
#include <klocale.h>
#include <kshortcut.h>
#include <kstandarddirs.h>
// Local
#include "cameraui.h"
#include "kameraklient.h"

typedef KGenericFactory<Plugin_KameraKlient> Factory;
K_EXPORT_COMPONENT_FACTORY(kipiplugin_kameraklient, Factory("kipiplugin_kameraklient"))

Plugin_KameraKlient::Plugin_KameraKlient(QObject *parent, const char*, const QStringList&)
		: KIPI::Plugin(Factory::instance(), parent, "KameraKlient") {
	kdDebug() << "KameraKlient KIPI Plugin loaded" << endl;
}

Plugin_KameraKlient::~Plugin_KameraKlient() {
}

void Plugin_KameraKlient::setup(QWidget* widget) {
	KIPI::Plugin::setup(widget);
	mKameraKlientAction = new KAction(i18n("Digital Camera"),
							"camera_unmount",
							KShortcut(),
							this,
							SLOT(slotActivate()),
							actionCollection(),
							"kipiplugin_kameraklient");
	addAction(mKameraKlientAction);
}

void Plugin_KameraKlient::slotActivate() {
    KIPIKameraKlientPlugin::CameraUI *mCameraUI = new KIPIKameraKlientPlugin::CameraUI();
    mCameraUI->show(); 
}

KIPI::Category Plugin_KameraKlient::category(KAction* action) const {
	if (action==mKameraKlientAction) {
	    return KIPI::IMPORTPLUGIN;
	}

    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::IMPORTPLUGIN; // no warning from compiler, please   
}

#include "kameraklient.moc"

