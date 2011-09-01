#include "PixelizePhotoEffect.h"
#include "KEditFactory.h"

#include <QImage>
#include <QPainter>
#include <QtTreePropertyBrowser>
#include <QtIntPropertyManager>

#include <klocalizedstring.h>

using namespace KIPIPhotoFramesEditor;

class PixelizePhotoEffect::PixelizeUndoCommand : public QUndoCommand
{
        PixelizePhotoEffect * m_effect;
        int m_pixelSize;
    public:
        PixelizeUndoCommand(PixelizePhotoEffect * effect, int pixelSize);
        virtual void redo();
        virtual void undo();
        void setPixelSize(int pixelSize);
    private:
        void runCommand()
        {
            int temp = m_effect->pixelSize();
            if (temp != m_pixelSize)
            {
                m_effect->setPixelSize(m_pixelSize);
                m_pixelSize = temp;
            }
        }
};

PixelizePhotoEffect::PixelizeUndoCommand::PixelizeUndoCommand(PixelizePhotoEffect * effect, int pixelSize) :
    m_effect(effect),
    m_pixelSize(pixelSize)
{}

void PixelizePhotoEffect::PixelizeUndoCommand::redo()
{
    runCommand();
}

void PixelizePhotoEffect::PixelizeUndoCommand::undo()
{
    runCommand();
}

void PixelizePhotoEffect::PixelizeUndoCommand::setPixelSize(int pixelSize)
{
    m_pixelSize = pixelSize;
}

const QString PixelizePhotoEffect::PIXEL_SIZE_STRING = i18n("Pixel size");

PixelizePhotoEffect::PixelizePhotoEffect(int pixelSize, QObject * parent) :
    PhotoEffectsLoader(parent),
    m_pixelSize(pixelSize)
{
}

QString PixelizePhotoEffect::effectName() const
{
    return i18n("Pixelize effect");
}

QImage PixelizePhotoEffect::apply(const QImage & image)
{
    QImage result = image;
    QPainter p(&result);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    p.drawImage(0, 0, PhotoEffectsLoader::apply(pixelize(image, m_pixelSize)));
    return result;
}

QtAbstractPropertyBrowser * PixelizePhotoEffect::propertyBrowser() const
{
    QtAbstractPropertyBrowser * browser = PhotoEffectsLoader::propertyBrowser();
    QtIntPropertyManager * intManager = new QtIntPropertyManager(browser);
    KSliderEditFactory * sliderFactory = new KSliderEditFactory(browser);
    browser->setFactoryForManager(intManager, sliderFactory);

    // Radius property
    QtProperty * pixelSize = intManager->addProperty(PIXEL_SIZE_STRING);
    intManager->setMaximum(pixelSize,200);
    intManager->setMinimum(pixelSize,1);
    browser->addProperty(pixelSize);

    intManager->setValue(pixelSize,m_pixelSize);
    connect(intManager,SIGNAL(propertyChanged(QtProperty*)),this,SLOT(propertyChanged(QtProperty*)));
    connect(sliderFactory,SIGNAL(editingFinished()),this,SLOT(postEffectChangedEvent()));

    return browser;
}

QString PixelizePhotoEffect::toString() const
{
    return i18n("Pixelize [") + PIXEL_SIZE_STRING + "=" + QString::number(m_pixelSize) + "]";
}

void PixelizePhotoEffect::propertyChanged(QtProperty * property)
{
    QtIntPropertyManager * manager = qobject_cast<QtIntPropertyManager*>(property->propertyManager());
    int pixelSize = m_pixelSize;

    if (property->propertyName() == PIXEL_SIZE_STRING)
        pixelSize = manager->value(property);
    else
    {
        PhotoEffectsLoader::propertyChanged(property);
        return;
    }

    beginUndoCommandChange();
    if (m_undo_command)
    {
        PixelizeUndoCommand * undo = dynamic_cast<PixelizeUndoCommand*>(m_undo_command);
        undo->setPixelSize(pixelSize);
    }
    else
        m_undo_command = new PixelizeUndoCommand(this,pixelSize);
    endUndoCommandChange();
}
