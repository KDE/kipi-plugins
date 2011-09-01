#ifndef SOLIDBORDERDRAWER_P_H
#define SOLIDBORDERDRAWER_P_H

#include "BorderDrawerInterface.h"

#include <QColor>

using namespace KIPIPhotoLayoutsEditor;

class SolidBorderDrawerFactory;
class SolidBorderDrawer : public BorderDrawerInterface
{
        Q_OBJECT
        Q_INTERFACES(KIPIPhotoLayoutsEditor::BorderDrawerInterface)

        int m_width;
        QColor m_color;
        int m_spacing;
        Qt::PenJoinStyle m_corners_style;
        QPainterPath m_path;

        static QMap<const char *,QString> m_properties;
        static QMap<Qt::PenJoinStyle, QString> m_corners_style_names;
        static int m_default_width;
        static QColor m_default_color;
        static int m_default_spacing;
        static Qt::PenJoinStyle m_default_corners_style;

    public:

        explicit SolidBorderDrawer(SolidBorderDrawerFactory * factory, QObject * parent = 0);

        virtual QPainterPath path(const QPainterPath & path);

        virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option);

        virtual QString propertyName(const QMetaProperty & property) const;

        virtual QVariant propertyValue(const QString & propertyName) const;

        virtual void setPropertyValue(const QString & propertyName, const QVariant & value);

        virtual QDomElement toSvg(QDomDocument & document) const;

        virtual QString toString() const;

        virtual operator QString() const;

        Q_PROPERTY(int width READ width WRITE setWidth)
        int width() const
        {
            return m_width;
        }
        void setWidth(int width)
        {
            if (width > 0)
                m_width = width;
        }

        Q_PROPERTY(QString corners_style READ cornersStyle WRITE setCornersStyle)
        QString cornersStyle() const
        {
            return m_corners_style_names.value(m_corners_style);
        }
        void setCornersStyle(const QString & cornersStyle)
        {
            m_corners_style = m_corners_style_names.key(cornersStyle);
        }

        Q_PROPERTY(QColor color READ color WRITE setColor)
        QColor color() const
        {
            return m_color;
        }
        void setColor(const QColor & color)
        {
            if (color.isValid())
                m_color = color;
        }

        Q_PROPERTY(int spacing READ spacing WRITE setSpacing)
        int spacing() const
        {
            return m_spacing;
        }
        void setSpacing(int spacing)
        {
            m_spacing = spacing;
        }

        virtual QVariant stringNames(const QMetaProperty & property);
        virtual QVariant minimumValue(const QMetaProperty & property);
        virtual QVariant maximumValue(const QMetaProperty & property);
        virtual QVariant stepValue(const QMetaProperty & property);

    private:

    friend class SolidBorderDrawerFactory;
};

#endif // SOLIDBORDERDRAWER_P_H
