#include "TextEditorTool.h"
#include "TextItem.h"
#include "ToolsDockWidget.h"

#include <QVBoxLayout>

using namespace KIPIPhotoLayoutsEditor;

class KIPIPhotoLayoutsEditor::TextEditorToolPrivate
{
    KPushButton * m_create_button;
    QVBoxLayout * m_layout;

    friend class TextEditorTool;
};

TextEditorTool::TextEditorTool(Scene * scene, QWidget * parent) :
    AbstractItemsTool(scene, Canvas::SingleSelcting, parent),
    d(new TextEditorToolPrivate),
    m_text_item(0),
    m_created_text_item(0),
    m_browser(0),
    m_create_new_item(false)
{
    d->m_layout = new QVBoxLayout();
    d->m_create_button = new KPushButton(i18n("Create text item"), this);
    connect(d->m_create_button, SIGNAL(clicked()), this, SLOT(createNewItem()));
    d->m_layout->insertWidget(0, d->m_create_button);
    d->m_layout->insertStretch(1, 1);
    this->setLayout(d->m_layout);
}

TextEditorTool::~TextEditorTool()
{
    delete d;
}

void TextEditorTool::currentItemAboutToBeChanged()
{
    if (m_browser)
    {
        d->m_layout->removeWidget(m_browser);
        m_browser->deleteLater();
        d->m_layout->insertStretch(1, 1);
        m_browser = 0;
    }
}

void TextEditorTool::currentItemChanged()
{
    m_text_item = dynamic_cast<TextItem*>(currentItem());
    if (m_text_item)
    {
        m_browser = m_text_item->propertyBrowser();
        if (m_browser)
        {
            d->m_layout->removeItem( d->m_layout->itemAt(1) );
            d->m_layout->insertWidget(1, m_browser, 2);
        }
    }
    setEnabled(true);
}

void TextEditorTool::positionAboutToBeChanged()
{
}

void TextEditorTool::positionChanged()
{
    if (m_create_new_item)
    {
        if (!m_created_text_item || !m_created_text_item->text().join("\n").isEmpty())
            m_created_text_item = new TextItem();
        setCurrentItem( m_created_text_item );
        currentItem()->setPos( this->mousePosition() );
        emit itemCreated( currentItem() );
        m_create_new_item = false;
        d->m_create_button->setEnabled(true);
    }
}

void TextEditorTool::createNewItem()
{
    m_create_new_item = true;
    d->m_create_button->setEnabled(false);
}
