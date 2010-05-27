#ifndef BACKEND_GOOGLE_MAPS_H
#define BACKEND_GOOGLE_MAPS_H 


//Qt includes


#include <QStringList>

//local includes

//#include "gpsreversegeocodingwidget.h"
#include "backend-rg.h"
#include "../worldmapwidget2/lib/html_widget.h"


namespace KIPIGPSSyncPlugin
{
   
class BackendGoogleRGPrivate;

class BackendGoogleRG : public RGBackend
{

    Q_OBJECT

public:

    BackendGoogleRG(QObject* const parent);
    virtual ~BackendGoogleRG();

    virtual void callRGBackend(QList <RGInfo>, QString); 
    QMap<QString, QString> makeQMap(QString);

public Q_SLOTS:

    void nextPhoto();
    void slotHTMLInitialized();
    void slotHTMLEvents( const QStringList& );    



private :

    BackendGoogleRGPrivate* const d;
    

};

} //KipiGPSSyncPlugin

#endif
