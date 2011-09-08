#include "ZoomTool.h"
#include "MousePressListener.h"

#include <QButtonGroup>
#include <QRadioButton>

using namespace KIPIPhotoLayoutsEditor;

class ZoomTool::ZoomToolPrivate
{
    QRadioButton * out;
    QRadioButton * in;
    MousePressListener * listener;
    friend class ZoomTool;
};

ZoomTool::ZoomTool(Scene * scene, QWidget * parent) :
    AbstractTool(scene, Canvas::Viewing, parent),
    d(new ZoomToolPrivate)
{
    QVBoxLayout * layout = new QVBoxLayout();
    this->setLayout(layout);

    QButtonGroup * group = new QButtonGroup(this);
    d->out = new QRadioButton(i18n("Zoom out"), this);
    group->addButton(d->out);
    layout->addWidget(d->out);
    d->in  = new QRadioButton(i18n("Zoom in"), this);
    group->addButton(d->in);
    layout->addWidget(d->in);

    layout->addSpacerItem(new QSpacerItem(10,10));
    layout->setStretch(2,1);

    d->listener = new MousePressListener();
    connect(d->listener, SIGNAL(mouseReleased(QPointF)), this, SLOT(zoom(QPointF)));

    d->in->setChecked(true);
}

ZoomTool::~ZoomTool()
{
    delete d;
}

void ZoomTool::sceneChange()
{
    Scene * scene = this->scene();
    if (!scene)
        return;
    scene->readSceneMousePress( 0 );
}

void ZoomTool::sceneChanged()
{
    Scene * scene = this->scene();
    if (!scene)
        return;
    scene->readSceneMousePress( d->listener );
}

void ZoomTool::zoom(const QPointF & point)
{
    Scene * scene = this->scene();
    if (!scene)
        return;
    QList<QGraphicsView*> views = scene->views();
    qreal factor = 1 + (d->out->isChecked() ? -0.1 : 0.1);
    foreach (QGraphicsView * view, views)
    {
        Canvas * canvas = qobject_cast<Canvas*>(view);
        if (!canvas)
            continue;
        if (d->listener->wasDragged())
            canvas->scale(QRect(canvas->mapFromScene(d->listener->mousePressPosition()),
                                canvas->mapFromScene(point)));
        else
            canvas->scale(factor, canvas->mapFromScene(point));
    }
}
