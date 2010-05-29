
#include "backend-rg.moc"

#include "backend-rg.h"

namespace KIPIGPSSyncPlugin 
{

class RGBackendPrivate
{
public:
    RGBackendPrivate()
    {
    }
};

RGBackend::RGBackend(QObject* const parent)
: QObject(parent) , d(new RGBackendPrivate())
{

}

RGBackend::~RGBackend()
{
   delete d;
}

} //KIPIGPSSyncPlugin

