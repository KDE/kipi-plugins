/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-01
 * Description : a plugin to create photo layouts by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Łukasz Spas <lukasz dot spas at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

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
