#ifndef PLUGIN_KAMERAKLIENT_H
#define PLUGIN_KAMERAKLIENT_H

#include <libkipi/plugin.h>

class CameraUI;
class CameraType;

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

