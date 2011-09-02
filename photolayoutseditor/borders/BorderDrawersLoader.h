#ifndef BORDERDRAWERSLOADER_H
#define BORDERDRAWERSLOADER_H

#include <QMap>
#include <QObject>
#include <QDomDocument>

#include "BorderDrawerFactoryInterface.h"

class QtProperty;

namespace KIPIPhotoLayoutsEditor
{
    class BorderDrawerFactoryInterface;

    class BorderDrawersLoaderPrivate;
    class BorderDrawersLoader : public QObject
    {
            Q_OBJECT

            BorderDrawersLoaderPrivate * d;
            static BorderDrawersLoader * m_instance;

        public:

            ~BorderDrawersLoader();
            static BorderDrawersLoader * instance(QObject * parent = 0);
            static void registerDrawer(BorderDrawerFactoryInterface * factory);
            static QStringList registeredDrawers();
            static BorderDrawerFactoryInterface * getFactoryByName(const QString & name);
            static BorderDrawerInterface * getDrawerByName(const QString & name, const QMap<QString,QString> & properties = QMap<QString,QString>());
            static BorderDrawerInterface * getDrawerFromSvg(QDomElement & drawerElement);
            static QDomElement drawerToSvg(BorderDrawerInterface * drawer, QDomDocument & document);
            static QWidget * createEditor(BorderDrawerInterface * drawer, bool createCommands);

        private:

            explicit BorderDrawersLoader(QObject * parent = 0);
    };
}

#endif // BORDERDRAWERSLOADER_H
