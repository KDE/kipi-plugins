/* ============================================================
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2003-02-22
 * Description : a message box used in batch converter.
 *               If One Message Box is already open, and more 
 *               messages are posted they will be appended to 
 *               the open messagebox
 * 
 * Copyright 2003-2005 by Renchi Raju
 * Copyright 2006 by Gilles Caulier
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

// Qt includes.

#include <qlayout.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qhbox.h>
#include <qapplication.h>

// KDE includes.

#include <klocale.h>
#include <kiconloader.h>
#include <klistbox.h>

// Locale includes.

#include "dmessagebox.h"

namespace KIPIRawConverterPlugin
{

DMessageBox* DMessageBox::s_instance = 0;

DMessageBox::DMessageBox(QWidget *parent)
           : QDialog(parent, 0, true, WShowModal | WDestructiveClose)
{
    setCaption(i18n("Error"));
    
    s_instance = this;
    count_ = 0;

    QGridLayout *grid = new QGridLayout(this, 1, 1, 6, 11);

    // ----------------------------------------------------

    QHBox *hbox = new QHBox(this);
    hbox->setSpacing(5);
    
    QPixmap pix = SmallIcon("error", KIcon::SizeMedium, KIcon::DefaultState);
    QLabel *pixLabel = new QLabel(hbox);
    pixLabel->setPixmap(pix);
    pixLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    msgBox_ = new QLabel(hbox);
    msgBox_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    grid->addMultiCellWidget(hbox, 0, 0, 0, 2);
    
    // ---------------------------------------------------

    extraMsgBox_ = new KListBox(this);
    extraMsgBox_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    grid->addMultiCellWidget(extraMsgBox_, 1, 1, 0, 2);

    // ---------------------------------------------------

    QPushButton *okButton = new QPushButton(i18n("&OK"), this);
    grid->addWidget(okButton, 2, 1);

    // ---------------------------------------------------

    grid->addItem( new QSpacerItem(5, 10, QSizePolicy::Expanding, QSizePolicy::Minimum), 2, 0);
    grid->addItem( new QSpacerItem(5, 10, QSizePolicy::Expanding, QSizePolicy::Minimum), 2, 2);
    // ---------------------------------------------------

    connect(okButton, SIGNAL(clicked()),
            this, SLOT(slotOkClicked()));
}

DMessageBox::~DMessageBox()
{
    s_instance = 0;
}

void DMessageBox::appendMsg(const QString& msg, const QString& mainMsg)
{
    if (count_ == 0) 
    {
        msgBox_->setText(mainMsg);
    }

    QString str(msg);
    str.remove(QString("\n"));
    new QListBoxText(extraMsgBox_, str);    
    
    count_++;
}

void DMessageBox::slotOkClicked()
{
    close();    
}

void DMessageBox::showMsg(const QString& msg, const QString& mainMsg,
                          QWidget* parent)
{
    DMessageBox* msgBox = DMessageBox::s_instance;
    if (!msgBox) 
    {
        msgBox = new DMessageBox(parent);
    }

    msgBox->appendMsg(msg, mainMsg);
    if (msgBox->isHidden())
        msgBox->show();
}

} // NameSpace KIPIRawConverterPlugin

#include "dmessagebox.moc"
