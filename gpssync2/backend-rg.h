#ifndef BACKEND_RG_H
#define BACKEND_RG_H

//Qt includes

#include <QWidget>

//KDE includes

#include <kurl.h>

//local includes

#include "gpsreversegeocodingwidget.h"


namespace KIPIGPSSyncPlugin
{

class RGBackendPrivate;

class RGBackend : public QObject
{

    Q_OBJECT


public:
    RGBackend(QObject* const parent);
    RGBackend();
    virtual ~RGBackend();

    virtual void callRGBackend(QList<RGInfo>, QString) = 0;


Q_SIGNALS:

    /**
     * @brief Emitted whenever some items are ready
     */
    void signalRGReady(QList<RGInfo>&);   

 

private:

    RGBackendPrivate* const d;    

};

} //KIPIGPSPlugin

#endif
