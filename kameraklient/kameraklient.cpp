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
	mKameraKlientAction = new KAction(i18n("Kamera Klient"),
							"camera",
							KShortcut(),
							this,
							SLOT(slotActivate()),
							actionCollection(),
							"kipiplugin_kameraklient");
	addAction(mKameraKlientAction);
}

void Plugin_KameraKlient::slotActivate() {
    CameraUI *mCameraUI = CameraUI::getInstance();
    mCameraUI->exec(); 
}

KIPI::Category Plugin_KameraKlient::category(KAction* action) const {
	if (action==mKameraKlientAction) {
	    return KIPI::IMPORTPLUGIN;
	}

    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::IMPORTPLUGIN; // no warning from compiler, please   
}

#include "kameraklient.moc"

