/* ============================================================
 * File  : messagebox.h
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

#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <qdialog.h>

class QListView;
class QString;

namespace KIPIJPEGLossLessPlugin
{

class MessageBox : public QDialog
{
    Q_OBJECT

public:

    MessageBox();
    ~MessageBox();

    void addMsg(const QString& fileName,
                const QString& msg);

    static void showMsg(const QString& fileName,
                        const QString& msg);
    static MessageBox* instance();

private:

    static MessageBox* m_instance;

    QListView *m_msgView;

private slots:

    void slotClose();
    
};

}  // NameSpace KIPIJPEGLossLessPlugin

#endif /* MESSAGEBOX_H */
