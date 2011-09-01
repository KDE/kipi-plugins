#include "ToolsDockWidget.h"
#include "CanvasEditTool.h"
#include "EffectsEditorTool.h"
#include "TextEditorTool.h"
#include "BorderEditTool.h"

#include <QDebug>
#include <QButtonGroup>
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QScrollArea>

#include <kicon.h>
#include <kguiitem.h>
#include <klocalizedstring.h>

using namespace KIPIPhotoLayoutsEditor;

ToolsDockWidget * ToolsDockWidget::m_instance = 0;

class MyStackedLayout : public QStackedLayout
{
    public:

        MyStackedLayout(QWidget * parent = 0) : QStackedLayout(parent) {}

        virtual QSize sizeHint() const
        {
            QSize s = QStackedLayout::sizeHint();
            s.setHeight(this->currentWidget()->sizeHint().height());
            return s;
        }

        virtual QSize minimumSize() const
        {
            return sizeHint();
        }
};

ToolsDockWidget * ToolsDockWidget::instance(QWidget * parent)
{
    if (!m_instance)
        m_instance = new ToolsDockWidget(parent);
    return m_instance;
}

ToolsDockWidget::ToolsDockWidget(QWidget * parent) :
    QDockWidget("Tools",parent),
    m_has_selection(false),
    m_currentPhoto(0),
    m_scene(0)
{
    this->setFeatures(QDockWidget::DockWidgetMovable);
    this->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QWidget * widget = new QWidget(this);
    QVBoxLayout * layout = new QVBoxLayout(widget);
    layout->setSizeConstraint(QLayout::SetMinimumSize);

    // tools buttons layout
    QGridLayout * formLayout = new QGridLayout();
    formLayout->setSizeConstraint(QLayout::SetMinimumSize);
    layout->addLayout(formLayout);

    // stacked widget (with tools widgets)
    QScrollArea * sa = new QScrollArea(widget);
    sa->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    sa->setFrameShape(QFrame::NoFrame);
    sa->setWidgetResizable(true);
    QWidget * wsa = new QWidget(sa);
    m_tool_widget_layout = new MyStackedLayout();
    QWidget * emptyWidget = new QWidget(wsa);
    emptyWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    m_tool_widget_layout->addWidget(emptyWidget);
    wsa->setLayout(m_tool_widget_layout);
    sa->setWidget(wsa);
    layout->addWidget(sa,1);

    formLayout->addItem(new QSpacerItem(24,24,QSizePolicy::Expanding),0,0);
    formLayout->addItem(new QSpacerItem(24,24,QSizePolicy::Expanding),1,0);
    QButtonGroup * group = new QButtonGroup(widget);

    // Selection tool

    m_tool_pointer = new KPushButton(KGuiItem("",":/pointer.png",
                                              i18n("Allows to select and move images on canvas"),
                                              i18n("Tool which allows to select and move images on canvas. Any other operations are disabled.")), widget);
    m_tool_pointer->setIconSize(QSize(24,24));
    m_tool_pointer->setFixedSize(32,32);
    m_tool_pointer->setCheckable(true);
    m_tool_pointer->setFlat(true);
    group->addButton(m_tool_pointer);
    formLayout->addWidget(m_tool_pointer, 0,1, Qt::AlignCenter);
    connect(m_tool_pointer,SIGNAL(toggled(bool)),this,SLOT(emitPointerToolSelected(bool)));

    // View tool
    m_tool_hand = new KPushButton(KGuiItem("",":/hand.png",
                                           i18n("Viewing tool"),
                                           i18n("This tool allows to view whole canvas in read-only mode. Only scrolling and zooming are available.")), widget);
    m_tool_hand->setIconSize(QSize(24,24));
    m_tool_hand->setFixedSize(32,32);
    m_tool_hand->setCheckable(true);
    m_tool_hand->setFlat(true);
    group->addButton(m_tool_hand);
    formLayout->addWidget(m_tool_hand, 0,2, Qt::AlignCenter);
    connect(m_tool_hand,SIGNAL(toggled(bool)),this,SLOT(emitHandToolSelected(bool)));

    // Canvas edit tool
    m_canvas_button = new KPushButton(KGuiItem("", ":/tool_canvas.png",
                                               i18n("Canvas editor"),
                                               i18n("This tool allows you to edit canvas properties like size and background.")), widget);
    m_canvas_button->setIconSize(QSize(24,24));
    m_canvas_button->setFixedSize(32,32);
    m_canvas_button->setCheckable(true);
    m_canvas_button->setFlat(true);
    group->addButton(m_canvas_button);
    formLayout->addWidget(m_canvas_button, 0,3, Qt::AlignCenter);
    m_canvas_widget = new CanvasEditTool(0, wsa);
    m_tool_widget_layout->addWidget(m_canvas_widget);
    connect(m_canvas_button,SIGNAL(toggled(bool)),this,SLOT(setCanvasWidgetVisible(bool)));

    // Text tool
    m_text_button = new KPushButton(KGuiItem("", ":/tool_text.png",
                                              i18n("Text editor"),
                                              i18n("This tool allows you to write text on the canvas. It's simple - just click on the canvas where you want to add some text and write it!")), widget);

    m_text_button->setIconSize(QSize(24,24));
    m_text_button->setFixedSize(32,32);
    m_text_button->setCheckable(true);
    m_text_button->setFlat(true);
    group->addButton(m_text_button);
    formLayout->addWidget(m_text_button, 0,4, Qt::AlignCenter);
    m_text_widget = new TextEditorTool(0, wsa);
    m_tool_widget_layout->addWidget(m_text_widget);
    connect(m_text_button,SIGNAL(toggled(bool)),this,SLOT(setTextWidgetVisible(bool)));
    connect(m_text_widget, SIGNAL(itemCreated(AbstractPhoto*)), this, SLOT(emitNewItemCreated(AbstractPhoto*)));

    // Rotate tool
    m_rotate_button = new KPushButton(KGuiItem("", ":/tool_rotate.png",
                                              i18n("Rotation tool"),
                                              i18n("This tool allows you to rotate items on your canvas.")), widget);
    m_rotate_button->setIconSize(QSize(24,24));
    m_rotate_button->setFixedSize(32,32);
    m_rotate_button->setCheckable(true);
    m_rotate_button->setFlat(true);
    group->addButton(m_rotate_button);
    formLayout->addWidget(m_rotate_button, 0,5, Qt::AlignCenter);
    connect(m_rotate_button,SIGNAL(toggled(bool)),this,SLOT(setRotateWidgetVisible(bool)));

    // Scale tool
    m_scale_button = new KPushButton(KGuiItem("", ":/tool_scale4.png",
                                              i18n("Scaling tool"),
                                              i18n("This tool allows you to scale items on your canvas.")), widget);
    m_scale_button->setIconSize(QSize(24,24));
    m_scale_button->setFixedSize(32,32);
    m_scale_button->setCheckable(true);
    m_scale_button->setFlat(true);
    group->addButton(m_scale_button);
    formLayout->addWidget(m_scale_button, 0,6, Qt::AlignCenter);
    connect(m_scale_button,SIGNAL(toggled(bool)),this,SLOT(setScaleWidgetVisible(bool)));

    // Crop tool
    m_crop_button = new KPushButton(KGuiItem("", ":/tool_cropt.png",
                                              i18n("Crop tool"),
                                              i18n("This tool allows you to crop items.")), widget);
    m_crop_button->setIconSize(QSize(24,24));
    m_crop_button->setFixedSize(32,32);
    m_crop_button->setCheckable(true);
    m_crop_button->setFlat(true);
    group->addButton(m_crop_button);
    formLayout->addWidget(m_crop_button, 1,1, Qt::AlignCenter);
    connect(m_crop_button,SIGNAL(toggled(bool)),this,SLOT(setCropWidgetVisible(bool)));

    // Photo effects tool
    m_effects_button = new KPushButton(KGuiItem("", ":/tool_effects.png",
                                              i18n("Image effects editor"),
                                              i18n("This tool allows you to edit existing effects of your photo layers and add some new once.")), widget);
    qDebug() << "icon" << QIcon(":tool_effects.png").isNull() << m_effects_button->icon().actualSize(QSize(24,24));
    m_effects_button->setIconSize(QSize(24,24));
    m_effects_button->setFixedSize(32,32);
    m_effects_button->setCheckable(true);
    m_effects_button->setFlat(true);
    group->addButton(m_effects_button);
    formLayout->addWidget(m_effects_button, 1,2, Qt::AlignCenter);
    m_effects_widget = new EffectsEditorTool(0, wsa);
    m_tool_widget_layout->addWidget(m_effects_widget);
    connect(m_effects_button,SIGNAL(toggled(bool)),this,SLOT(setEffectsWidgetVisible(bool)));

    // Border edit tool
    m_tool_border = new KPushButton(KIcon(":/tool_border.png"), "", widget);
    m_tool_border->setIconSize(QSize(24,24));
    m_tool_border->setFixedSize(32,32);
    m_tool_border->setCheckable(true);
    m_tool_border->setFlat(true);
    group->addButton(m_tool_border);
    formLayout->addWidget(m_tool_border, 1,3, Qt::AlignCenter);
    m_border_widget = new BorderEditTool(0, wsa);
    m_tool_widget_layout->addWidget(m_border_widget);
    connect(m_tool_border,SIGNAL(toggled(bool)),this,SLOT(setBordersWidgetVisible(bool)));

    // Spacer
    formLayout->addItem(new QSpacerItem(24,24,QSizePolicy::Expanding),0,7);
    formLayout->addItem(new QSpacerItem(24,24,QSizePolicy::Expanding),1,7);
    formLayout->setColumnStretch(7,1);
    formLayout->setSpacing(0);
    formLayout->setMargin(0);

    layout->setSpacing(0);
    layout->setMargin(0);
    widget->setLayout(layout);
    widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    this->setWidget(widget);
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);

    setDefaultTool();
}

ToolsDockWidget::~ToolsDockWidget()
{
    m_instance = 0;
}

void ToolsDockWidget::setDefaultTool()
{
    m_tool_hand->setChecked(true);
    this->emitHandToolSelected(true);
}

void ToolsDockWidget::setScene(Scene * scene)
{
    if (scene)
        this->connect(scene, SIGNAL(destroyed()), this, SLOT(setScene()));
    if (sender() && !scene && this->m_scene)
        return;
    m_scene = scene;
    m_canvas_widget->setScene(scene);
    m_effects_widget->setScene(scene);
    m_text_widget->setScene(scene);
    m_border_widget->setScene(scene);
}

void ToolsDockWidget::itemSelected(AbstractPhoto * photo)
{
    qDebug() << "itemSelected" << (QGraphicsItem*)photo;
    m_currentPhoto = photo;
    AbstractItemsTool * tool =qobject_cast<AbstractItemsTool*>(m_tool_widget_layout->currentWidget());
    if (tool)
        tool->setCurrentItem(photo);
}

void ToolsDockWidget::mousePositionChoosen(const QPointF & position)
{
    AbstractItemsTool * tool =qobject_cast<AbstractItemsTool*>(m_tool_widget_layout->currentWidget());
    if (tool)
        tool->setMousePosition(position);
}

void ToolsDockWidget::emitNewItemCreated(AbstractPhoto * item)
{
    if (!item)
        return;
    emit newItemCreated(item);
}

void ToolsDockWidget::setCanvasWidgetVisible(bool isVisible)
{
    m_canvas_button->setChecked(isVisible);
    emit canvasToolSelectionChanged(isVisible);
    if (isVisible)
    {
        m_tool_widget_layout->setCurrentWidget(m_canvas_widget);
        emit requireMultiSelection();
        emit canvasToolSelected();
        this->adjustSize();
    }
}

void ToolsDockWidget::setEffectsWidgetVisible(bool isVisible)
{
    emit effectsToolSelectionChanged(isVisible);
    if (isVisible)
    {
        m_effects_widget->setCurrentItem(0);
        m_tool_widget_layout->setCurrentWidget(m_effects_widget);
        emit requireSingleSelection();
        emit effectsToolSelected();
        this->adjustSize();
    }
}

void ToolsDockWidget::setTextWidgetVisible(bool isVisible)
{
    emit textToolSelectionChanged(isVisible);
    if (isVisible)
    {
        m_text_widget->setCurrentItem(0);
        m_tool_widget_layout->setCurrentWidget(m_text_widget);
        emit requireSingleSelection();
        emit textToolSelected();
        this->adjustSize();
    }
}

void ToolsDockWidget::setRotateWidgetVisible(bool isVisible)
{
    emit rotateToolSelectionChanged(isVisible);
    if (isVisible)
    {
        m_tool_widget_layout->setCurrentIndex(0);
        emit requireSingleSelection();
        emit rotateToolSelected();
        this->adjustSize();
    }
}

void ToolsDockWidget::setScaleWidgetVisible(bool isVisible)
{
    emit scaleToolSelectionChanged(isVisible);
    if (isVisible)
    {
        m_tool_widget_layout->setCurrentIndex(0);
        emit requireSingleSelection();
        emit scaleToolSelected();
        this->adjustSize();
    }
}

void ToolsDockWidget::setCropWidgetVisible(bool isVisible)
{
    emit cropToolSelectionChanged(isVisible);
    if (isVisible)
    {
        m_tool_widget_layout->setCurrentIndex(0);
        emit requireSingleSelection();
        emit cropToolSelected();
        this->adjustSize();
    }
}

void ToolsDockWidget::setBordersWidgetVisible(bool isVisible)
{
    emit borderToolSelectionChanged(isVisible);
    if (isVisible)
    {
        m_border_widget->setCurrentItem(0);
        m_tool_widget_layout->setCurrentWidget(m_border_widget);
        emit requireSingleSelection();
        emit borderToolSelected();
        this->adjustSize();
    }
}
