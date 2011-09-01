#include "BorderDrawersLoader.h"
#include "BorderDrawerInterface.h"
#include "BorderDrawerFactoryInterface.h"
#include "BorderChangeListener.h"

#include <QMap>
#include <QStringList>
#include <QMetaProperty>
#include <QtTreePropertyBrowser>
#include <KEditFactory.h>
#include <QUndoCommand>

using namespace KIPIPhotoLayoutsEditor;

class KIPIPhotoLayoutsEditor::BorderDrawersLoaderPrivate
{
    QMap<QString,BorderDrawerFactoryInterface*> factories;

    friend class BorderDrawersLoader;
};

BorderDrawersLoader * BorderDrawersLoader::m_instance = 0;

BorderDrawersLoader::BorderDrawersLoader(QObject * parent) :
    QObject(parent),
    d(new BorderDrawersLoaderPrivate)
{
}

BorderDrawersLoader::~BorderDrawersLoader()
{
    delete d;
}

BorderDrawersLoader * BorderDrawersLoader::instance(QObject * parent)
{
    if (!m_instance)
        m_instance = new BorderDrawersLoader();
    if (parent)
        m_instance->setParent(parent);
    return m_instance;
}

void BorderDrawersLoader::registerDrawer(BorderDrawerFactoryInterface * factory)
{
    instance()->d->factories.insert(factory->drawerName(), factory);
}

QStringList BorderDrawersLoader::registeredDrawers()
{
    return instance()->d->factories.keys();
}

BorderDrawerFactoryInterface * BorderDrawersLoader::getFactoryByName(const QString & name)
{
    return instance()->d->factories.value(name, 0);
}

BorderDrawerInterface * BorderDrawersLoader::getDrawerByName(const QString & name, const QMap<QString,QString> & properties)
{
    BorderDrawerFactoryInterface * factory = getFactoryByName(name);
    if (factory)
    {
        BorderDrawerInterface * drawer = factory->getDrawerInstance(factory);
        if (!drawer)
            return 0;
        const QMetaObject * meta = drawer->metaObject();
        for (int i = meta->propertyCount()-1; i >= 0; --i)
        {
            QMetaProperty prop = meta->property(i);
            QString value = properties.value(QString(prop.name()));
            if (value.isEmpty())
                continue;
            prop.write(drawer, QVariant(value.toAscii()));
        }
        return drawer;
    }
    return 0;
}

BorderDrawerInterface * BorderDrawersLoader::getDrawerFromSvg(QDomElement & drawerElement)
{
    QMap<QString,QString> properties;
    QDomNamedNodeMap attributes = drawerElement.attributes();
    for (int j = attributes.count()-1; j >= 0; --j)
    {
        QDomAttr attr = attributes.item(j).toAttr();
        if (attr.isNull())
            continue;
        properties.insert(attr.name(), attr.value());
    }
    QString drawerName = properties.take("name");
    if (!instance()->registeredDrawers().contains(drawerName))
        return 0;
    BorderDrawerInterface * drawer = getDrawerByName(drawerName);
    const QMetaObject * meta = drawer->metaObject();
    int count = meta->propertyCount();
    for (int i = 0; i < count; ++i)
    {
        QMetaProperty p = meta->property(i);
        QString value = properties.take(p.name());
        if (value.isEmpty())
            continue;
        p.write(drawer, QVariant(QByteArray::fromBase64(value.toAscii())));
    }
    return drawer;
}

QDomElement BorderDrawersLoader::drawerToSvg(BorderDrawerInterface * drawer, QDomDocument & document)
{
    if (!drawer)
        return QDomElement();
    QDomElement result = document.createElement("g");
    result.setAttribute("name", drawer->factory()->drawerName());

    result.appendChild( drawer->toSvg(document) );

    const QMetaObject * meta = drawer->metaObject();
    int count = meta->propertyCount();
    for (int i = 0; i < count; ++i)
    {
        QMetaProperty p = meta->property(i);
        result.setAttribute( p.name(), QString(p.read(drawer).toByteArray().toBase64()) );
    }

    return result;
}

QWidget * BorderDrawersLoader::createEditor(BorderDrawerInterface * drawer, bool createCommands)
{
    if (!drawer)
        return 0;

    QtTreePropertyBrowser * browser = new QtTreePropertyBrowser();
    BorderChangeListener * listener = new BorderChangeListener(drawer, browser, createCommands);

    // QVariant type of property
    QtVariantPropertyManager * variantManager = 0;
    KVariantEditorFactory * variantFactory = 0;

    // Integer type of property
    QtIntPropertyManager * integerManager = 0;
    KSliderEditFactory * integerFactory = 0;

    // Double type of property
    QtDoublePropertyManager * doubleManager = 0;
    KDoubleSpinBoxFactory * doubleFactory = 0;

    const QMetaObject * meta = drawer->metaObject();
    int propertiesCount = meta->propertyCount();
    for (int i = 0; i < propertiesCount; ++i)
    {
        QMetaProperty metaProperty = meta->property(i);
        QString propertyName = drawer->propertyName(metaProperty);
        if (propertyName.isEmpty())
            continue;
        QtProperty * property;
        switch (metaProperty.type())
        {
            case QVariant::Int:
                {
                    if (!integerManager || !integerFactory)
                    {
                        integerManager = new QtIntPropertyManager(browser);
                        integerFactory = new KSliderEditFactory(browser);
                        browser->setFactoryForManager(integerManager, integerFactory);
                    }
                    property = integerManager->addProperty(propertyName);
                    integerManager->setValue(property, metaProperty.read(drawer).toInt());
                    integerManager->setMinimum(property, drawer->minimumValue(metaProperty).toInt());
                    integerManager->setMaximum(property, drawer->maximumValue(metaProperty).toInt());
                }
                break;
            case QVariant::Double:
                {
                    if (!doubleManager || !doubleFactory)
                    {
                        doubleManager = new QtDoublePropertyManager(browser);
                        doubleFactory = new KDoubleSpinBoxFactory(browser);
                        browser->setFactoryForManager(doubleManager, doubleFactory);
                    }
                    property = doubleManager->addProperty(propertyName);
                    doubleManager->setValue(property, metaProperty.read(drawer).toDouble());
                    doubleManager->setMinimum(property, drawer->minimumValue(metaProperty).toDouble());
                    doubleManager->setMaximum(property, drawer->maximumValue(metaProperty).toDouble());
                }
                break;
            default:
                {
                    if (!variantManager || !variantFactory)
                    {
                        variantManager = new QtVariantPropertyManager(browser);
                        variantFactory = new KVariantEditorFactory(browser);
                        browser->setFactoryForManager(variantManager, variantFactory);
                    }
                    property = variantManager->addProperty(metaProperty.type(), propertyName);
                    variantManager->setValue(property, metaProperty.read(drawer));
                }
        }
        browser->addProperty(property);
    }

    if (integerManager)
    {
        connect(integerFactory, SIGNAL(editingFinished()), listener, SLOT(editingFinished()));
        connect(integerManager, SIGNAL(propertyChanged(QtProperty*)), listener, SLOT(propertyChanged(QtProperty*)));
    }
    if (doubleManager)
    {
        connect(doubleFactory, SIGNAL(editingFinished()), listener, SLOT(editingFinished()));
        connect(doubleManager, SIGNAL(propertyChanged(QtProperty*)), listener, SLOT(propertyChanged(QtProperty*)));
    }
    if (variantManager)
    {
        connect(variantFactory, SIGNAL(editingFinished()), listener, SLOT(editingFinished()));
        connect(variantManager, SIGNAL(propertyChanged(QtProperty*)), listener, SLOT(propertyChanged(QtProperty*)));
    }

    return browser;
}
