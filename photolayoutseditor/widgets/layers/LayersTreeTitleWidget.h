#ifndef LAYERSTREETITLEWIDGET_H
#define LAYERSTREETITLEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

#include <kpushbutton.h>
#include <kicon.h>
#include <klocalizedstring.h>

namespace KIPIPhotoLayoutsEditor
{
    class LayersTreeTitleWidget : public QWidget
    {
            QHBoxLayout * m_layout;
            QLabel * m_label;
            KPushButton * m_up_btn;
            KPushButton * m_dwn_btn;

        public:

            LayersTreeTitleWidget (QWidget * parent = 0) :
                QWidget(parent),
                m_layout(new QHBoxLayout(this)),
                m_label(new QLabel(i18n("Layers"),this)),
                m_up_btn(new KPushButton(KIcon(":/arrow_top.png"), "", this)),
                m_dwn_btn(new KPushButton(KIcon(":/arrow_down.png"), "", this))
            {
                m_layout->addWidget(m_label,1);
                m_layout->addWidget(m_up_btn);
                m_layout->addWidget(m_dwn_btn);
                this->setLayout(m_layout);
                m_layout->setContentsMargins(0,0,0,0);
                m_layout->setMargin(0);
                m_layout->setSpacing(0);
                m_layout->update();

                m_up_btn->setFixedSize(24,24);
                m_dwn_btn->setFixedSize(24,24);
            }

            QAbstractButton * moveUpButton() const
            {
                return m_up_btn;
            }

            QAbstractButton * moveDownButton() const
            {
                return m_dwn_btn;
            }
    };
}

#endif // LAYERSTREETITLEWIDGET_H
