#include "EffectsEditorTool.h"
#include "AbstractPhoto.h"
#include "PhotoEffectsLoader.h"
#include "PhotoEffectsGroup.h"
#include "AbstractPhotoEffectInterface.h"
#include "ToolsDockWidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QListView>
#include <QtAbstractPropertyBrowser>

#include <klocalizedstring.h>
#include <kpushbutton.h>
#include <kcombobox.h>

using namespace KIPIPhotoLayoutsEditor;

class KIPIPhotoLayoutsEditor::EffectsEditorToolPrivate
{
    EffectsEditorToolPrivate() :
        m_list_widget(0),
        m_add_button(0),
        m_remove_button(0),
        m_down_button(0),
        m_up_button(0),
        m_opened_editor(0,QModelIndex()),
        m_editors_effect(0)
    {}
    EffectsListView * m_list_widget;
    KPushButton * m_add_button;
    KPushButton * m_remove_button;
    KPushButton * m_down_button;
    KPushButton * m_up_button;
    QPair<EffectListViewDelegate*,QPersistentModelIndex> m_opened_editor;
    AbstractPhotoEffectInterface * m_editors_effect;

    void removeEffectChooser()
    {
        if (m_opened_editor.first)
            m_opened_editor.first->deleteLater();
        if (m_opened_editor.second.isValid() && !m_opened_editor.second.internalPointer())
            const_cast<QAbstractItemModel*>(m_opened_editor.second.model())->removeRow(m_opened_editor.second.row());
        if (m_editors_effect)
            m_editors_effect->deleteLater();
        m_opened_editor.first = 0;
        m_opened_editor.second = QPersistentModelIndex();
        m_editors_effect = 0;
    }

    void setButtonsEnabled(bool isEnabled)
    {
        m_add_button->setEnabled(isEnabled);
        QModelIndex index = m_list_widget->selectedIndex();
        m_remove_button->setEnabled(isEnabled && index.isValid());
        m_down_button->setEnabled(isEnabled && index.isValid() && index.row() < index.model()->rowCount());
        m_up_button->setEnabled(isEnabled && index.isValid() && index.row() > 0);
    }

    friend class EffectsEditorTool;
};

EffectsEditorTool::EffectsEditorTool(Scene * scene, QWidget * parent) :
    AbstractItemsTool(scene, Canvas::SingleSelcting, parent),
    d(new EffectsEditorToolPrivate)
{
    QGridLayout * layout = new QGridLayout(this);

    // Title
    QLabel * title = new QLabel(i18n("Effects editor"), this);
    QFont titleFont = title->font();
    titleFont.setBold(true);
    title->setFont(titleFont);
    layout->addWidget(title,0,0);

    // Move add/remove buttons
    QHBoxLayout * addLayout = new QHBoxLayout();
    d->m_add_button = new KPushButton(KGuiItem("",":action_add.png",
                             i18n("Add new effect"),
                             i18n("This button adds new effect to the list. You'll be able to select effect type after you click this button.")));
    d->m_add_button->setIconSize(QSize(16,16));
    d->m_add_button->setFixedSize(24,24);
    d->m_remove_button = new KPushButton(KGuiItem("",":action_remove.png",
                             i18n("Remove selected item"),
                             i18n("This button removes selected item from the effects list.")));
    d->m_remove_button->setIconSize(QSize(16,16));
    d->m_remove_button->setFixedSize(24,24);
    addLayout->addWidget(d->m_add_button);
    addLayout->addWidget(d->m_remove_button);
    addLayout->setSpacing(0);
    layout->addLayout(addLayout,0,1);
    connect(d->m_add_button,SIGNAL(clicked()),this,SLOT(addEffect()));
    connect(d->m_remove_button,SIGNAL(clicked()),this,SLOT(removeSelected()));

    // Move up/down buttons
    QHBoxLayout * moveLayout = new QHBoxLayout();
    d->m_down_button = new KPushButton(KGuiItem("",":arrow_down.png",
                             i18n("Moves effect down"),
                             i18n("This button moves the selected effect down in stack of effect's layers.")));
    d->m_down_button->setIconSize(QSize(16,16));
    d->m_down_button->setFixedSize(24,24);
    d->m_up_button = new KPushButton(KGuiItem("",":arrow_top.png",
                             i18n("Moves effect up"),
                             i18n("This button moves the selected effect up in stack of effect's layers.")));

    d->m_up_button->setIconSize(QSize(16,16));
    d->m_up_button->setFixedSize(24,24);
    moveLayout->addWidget(d->m_down_button);
    moveLayout->addWidget(d->m_up_button);
    moveLayout->setSpacing(0);
    layout->addLayout(moveLayout,0,2);
    connect(d->m_down_button,SIGNAL(clicked()),this,SLOT(moveSelectedDown()));
    connect(d->m_up_button,SIGNAL(clicked()),this,SLOT(moveSelectedUp()));

    // Effects list
    d->m_list_widget = new EffectsListView(this);
    layout->addWidget(d->m_list_widget,1,0,1,-1);
    connect(d->m_list_widget,SIGNAL(selectedIndex(QModelIndex)),this,SLOT(viewCurrentEffectEditor(QModelIndex)));

    this->setLayout(layout);
    this->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Maximum);
    layout->setRowStretch(2,1);
    d->setButtonsEnabled(false);
}

EffectsEditorTool::~EffectsEditorTool()
{
    delete d;
}

void EffectsEditorTool::currentItemAboutToBeChanged()
{
    closeEffectPropertyBrowser();
    addEffectCommand();
    d->removeEffectChooser();
}

void EffectsEditorTool::currentItemChanged()
{
    AbstractPhoto * photo = this->currentItem();
    if (photo)
        d->m_list_widget->setModel(photo->effectsGroup());
    else
        d->m_list_widget->setModel(0);
    d->setButtonsEnabled(true);
}

void EffectsEditorTool::viewCurrentEffectEditor(const QModelIndex & index)
{
    closeEffectPropertyBrowser();
    d->setButtonsEnabled(true);
    AbstractPhotoEffectInterface * effect = static_cast<AbstractPhotoEffectInterface*>(index.internalPointer());
    viewEffectEditor(effect);
}

void EffectsEditorTool::viewEffectEditor(AbstractPhotoEffectInterface * effect)
{
    if (effect)
    {
        QtAbstractPropertyBrowser * browser = PhotoEffectsLoader::propertyBrowser(effect);
        static_cast<QGridLayout*>(layout())->addWidget(browser,2,0,1,-1);
        browser->show();
    }
}

void EffectsEditorTool::addEffect()
{
    PhotoEffectsGroup * model = qobject_cast<PhotoEffectsGroup*>(d->m_list_widget->model());
    if (model)
    {
        int row = 0;
        QModelIndex selectedIndex = d->m_list_widget->selectedIndex();
        if (selectedIndex.isValid())
            row = selectedIndex.row();
        model->insertRow(row);
        EffectListViewDelegate * w = new EffectListViewDelegate(d->m_list_widget);
        d->m_opened_editor.first = w;
        d->m_opened_editor.second = QPersistentModelIndex(model->index(row,0));
        d->m_list_widget->setIndexWidget(model->index(row,0),w);
        d->m_list_widget->setSelectionMode(QAbstractItemView::NoSelection);
        connect(w,SIGNAL(editorAccepted()),this,SLOT(addEffectCommand()));
        connect(w,SIGNAL(editorClosed()),this,SLOT(closeEffectChooser()));
        connect(w,SIGNAL(effectSelected(QString)),this,SLOT(editorEfectSelected(QString)));
        d->setButtonsEnabled(false);
        d->m_list_widget->setSelection(QRect(),QItemSelectionModel::Clear);
    }
}

void EffectsEditorTool::editorEfectSelected(const QString & effectName)
{
    EffectListViewDelegate * w = d->m_opened_editor.first;
    PhotoEffectsGroup * model = qobject_cast<PhotoEffectsGroup*>(d->m_list_widget->model());
    if (model && w)
    {
        d->m_editors_effect = PhotoEffectsLoader::getEffectByName(effectName);
        viewEffectEditor(d->m_editors_effect);
    }
}

void EffectsEditorTool::addEffectCommand()
{
    if (d->m_editors_effect)
    {
        AbstractPhotoEffectInterface * effect = d->m_editors_effect;
        d->m_editors_effect = 0;
        int row = d->m_opened_editor.second.row();
        closeEffectChooser();
        PhotoEffectsGroup * model = qobject_cast<PhotoEffectsGroup*>(d->m_list_widget->model());
        model->insertRow(row, effect);
    }
}

void EffectsEditorTool::closeEffectChooser()
{
    closeEffectPropertyBrowser();
    d->removeEffectChooser();
    d->m_list_widget->setSelectionMode(QAbstractItemView::SingleSelection);
    d->setButtonsEnabled(true);
}

void EffectsEditorTool::removeSelected()
{
    if (!d->m_list_widget)
        return;
    QModelIndex index = d->m_list_widget->selectedIndex();
    PhotoEffectsGroup * model = qobject_cast<PhotoEffectsGroup*>(d->m_list_widget->model());
    if (model && index.isValid())
        model->removeRow(index.row());
}

void EffectsEditorTool::moveSelectedDown()
{
    if (!d->m_list_widget)
        return;
    QModelIndex index = d->m_list_widget->selectedIndex();
    PhotoEffectsGroup * model = qobject_cast<PhotoEffectsGroup*>(d->m_list_widget->model());
    if (model && index.row() < model->rowCount()-1)
        model->moveRows(index.row(),1,index.row()+2);
}

void EffectsEditorTool::moveSelectedUp()
{
    if (!d->m_list_widget)
        return;
    QModelIndex index = d->m_list_widget->selectedIndex();
    PhotoEffectsGroup * model = qobject_cast<PhotoEffectsGroup*>(d->m_list_widget->model());
    if (model && index.row() > 0)
        model->moveRows(index.row(),1,index.row()-1);
}

void EffectsEditorTool::closeEffectPropertyBrowser()
{
    QLayoutItem * itemBrowser = static_cast<QGridLayout*>(layout())->itemAtPosition(2,0);
    if (!itemBrowser)
        return;
    QWidget * browser = itemBrowser->widget();
    if (!browser)
        return;
    static_cast<QGridLayout*>(layout())->removeWidget(browser);
    browser->deleteLater();
}

EffectListViewDelegate::EffectListViewDelegate(QWidget * parent) :
    QWidget(parent)
{
    QHBoxLayout * layout = new QHBoxLayout();
    layout->setSpacing(0);
    layout->setMargin(0);
    this->setLayout(layout);
    KComboBox * comboBox = new KComboBox(this);
    comboBox->addItems(PhotoEffectsLoader::registeredEffectsNames());
    comboBox->setCurrentIndex(-1);
    connect(comboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(emitEffectSelected(QString)));
    layout->addWidget(comboBox,1);
    m_acceptButton = new KPushButton(KIcon(":action_check.png"), "", this);
    m_acceptButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    m_acceptButton->setEnabled(false);
    connect(m_acceptButton,SIGNAL(clicked()),this,SLOT(emitEditorAccepted()));
    layout->addWidget(m_acceptButton);
    KPushButton * cancelButton = new KPushButton(KIcon(":action_delete.png"), "", this);
    cancelButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    connect(cancelButton,SIGNAL(clicked()),this,SLOT(emitEditorClosed()));
    layout->addWidget(cancelButton);
}
