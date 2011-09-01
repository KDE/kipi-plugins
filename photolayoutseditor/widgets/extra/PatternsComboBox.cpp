#include "PatternsComboBox.h"

#include <QStyledItemDelegate>
#include <QStylePainter>
#include <QDebug>
#include <QListView>
#include <QPaintEngine>
#include <QPaintEvent>

using namespace KIPIPhotoLayoutsEditor;

class PatternDelegate : public QStyledItemDelegate
{
    public:
        PatternDelegate(QObject * parent = 0) :
            QStyledItemDelegate(parent)
        {}
        virtual ~PatternDelegate()
        {}
        virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
        {
            QSize result = option.rect.size();
            if (index.isValid())
                result.setHeight(24);
            return result;
        }
        virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
        {
            if (index.isValid())
            {
                Qt::BrushStyle style = (Qt::BrushStyle) index.data(Qt::UserRole).toInt();
                QBrush b(Qt::black, style);
                QRectF r = option.rect;
                r.setHeight(24);
                painter->fillRect(r, b);
            }
        }
};

PatternsComboBox::PatternsComboBox(QWidget * parent) :
    QComboBox(parent)
{
    this->addItem("", QVariant(Qt::Dense1Pattern));
    this->addItem("", QVariant(Qt::Dense2Pattern));
    this->addItem("", QVariant(Qt::Dense3Pattern));
    this->addItem("", QVariant(Qt::Dense4Pattern));
    this->addItem("", QVariant(Qt::Dense5Pattern));
    this->addItem("", QVariant(Qt::Dense6Pattern));
    this->addItem("", QVariant(Qt::Dense7Pattern));
    this->addItem("", QVariant(Qt::HorPattern));
    this->addItem("", QVariant(Qt::VerPattern));
    this->addItem("", QVariant(Qt::CrossPattern));
    this->addItem("", QVariant(Qt::BDiagPattern));
    this->addItem("", QVariant(Qt::FDiagPattern));
    this->addItem("", QVariant(Qt::DiagCrossPattern));
    this->setItemDelegate(new PatternDelegate(this));
    this->setMinimumWidth(100);
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(emitPatternChanged(int)));
}

Qt::BrushStyle PatternsComboBox::pattern() const
{
    return (Qt::BrushStyle) this->itemData( this->currentIndex() ).toInt();
}

void PatternsComboBox::setPattern(Qt::BrushStyle pattern)
{
    for (int i = this->count()-1; i >= 0; --i)
    {
        if (static_cast<Qt::BrushStyle>(itemData(i).toInt()) == pattern)
        {
            this->setCurrentIndex(i);
            return;
        }
    }
    this->setCurrentIndex(-1);
    return;
}

void PatternsComboBox::paintEvent(QPaintEvent * e)
{
    QComboBox::paintEvent(e);
    QStylePainter p(this);

    QStyleOptionComboBox op;
    initStyleOption(&op);

    QRect r = style()->subElementRect( QStyle::SE_ComboBoxFocusRect, &op, this );
    r.setHeight(r.height()-3);
    r.setWidth(r.width()-3);
    r.setX(r.x()+1);
    r.setY(r.y()+1);
    QBrush b(Qt::black, (Qt::BrushStyle)this->itemData(this->currentIndex()).toInt());
    p.fillRect(r,b);
}

void PatternsComboBox::emitPatternChanged(int index)
{
    emit currentPatternChanged( (Qt::BrushStyle) this->itemData(index).toInt() );
}
