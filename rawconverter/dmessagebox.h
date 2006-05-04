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

#ifndef DMESSAGEBOX_H
#define DMESSAGEBOX_H

// Qt includes.

#include <qdialog.h>
#include <qstring.h>

class QLabel;
class KListBox;

namespace KIPIRawConverterPlugin
{

class DMessageBox : public QDialog
{
    Q_OBJECT

public:

    DMessageBox(QWidget *parent=0);
    ~DMessageBox();

    void appendMsg(const QString& msg, const QString& mainMsg);
    static void showMsg(const QString& msg, const QString& mainMsg, QWidget *parent=0);
    
private:

    static DMessageBox* s_instance;

private slots:

    void slotOkClicked();

private:

    int       count_;

    QLabel   *msgBox_;

    KListBox *extraMsgBox_;
    
};

} // NameSpace KIPIRawConverterPlugin

#endif /* DMESSAGEBOX_H */
