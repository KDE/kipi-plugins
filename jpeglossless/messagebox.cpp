/* ============================================================
 * File  : messagebox.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-01-22
 * Description : 
 * 
 * Copyright 2004 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#include <klocale.h>
#include <kapplication.h>
#include <kiconloader.h>

#include <qlistview.h>
#include <qstring.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include "messagebox.h"

namespace JPEGLossLess
{

MessageBox* MessageBox::m_instance = 0;

MessageBox::MessageBox()
    : QWidget(0,0,Qt::WDestructiveClose)
{
    m_instance = this;

    QVBoxLayout *layout = new QVBoxLayout(this, 5, 5);

    { 
        QHBoxLayout *l = new QHBoxLayout(layout);
        QLabel *label  = new QLabel(this);
        label->setSizePolicy(QSizePolicy::Minimum,
                             QSizePolicy::Minimum);
        label->setPixmap(kapp->iconLoader()->loadIcon("error",
                                                      KIcon::NoGroup,
                                                      KIcon::SizeMedium,
                                                      KIcon::DefaultState,
                                                      0, true));
        l->addWidget(label);
        label = new QLabel(this);
        label->setSizePolicy(QSizePolicy::Expanding,
                             QSizePolicy::Minimum);
        label->setText(i18n("The following error(s) occurred ..."));
        l->addWidget(label);
    }
    
    m_msgView = new QListView(this);
    m_msgView->addColumn(i18n("File"));
    m_msgView->addColumn(i18n("Error"));
    layout->addWidget(m_msgView);

    {
        QHBoxLayout *l = new QHBoxLayout(layout);
        l->addItem(new QSpacerItem(10,10, QSizePolicy::Expanding,
                                   QSizePolicy::Minimum));
        QPushButton *btn = new QPushButton(i18n("&Close"), this);
        l->addWidget(btn);
        l->addItem(new QSpacerItem(10,10, QSizePolicy::Expanding,
                                   QSizePolicy::Minimum));

        connect(btn, SIGNAL(clicked()), SLOT(slotClose()));

    }
}

MessageBox::~MessageBox()
{
    m_instance = 0;    
}

void MessageBox::addMsg(const QString& fileName,
                        const QString& msg)
{
    new QListViewItem(m_msgView, fileName, msg);
}

void MessageBox::slotClose()
{
    close();    
}

void MessageBox::showMsg(const QString& fileName,
 const QString& msg)
{
    if (!m_instance) 
        new MessageBox();

    m_instance->addMsg(fileName, msg);
    if (m_instance->isHidden())
        m_instance->show();
}

MessageBox* MessageBox::instance()
{
    return m_instance;    
}

}
