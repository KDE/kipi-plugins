/* ============================================================
 * File  : picasaweblogin.h
 * Author: Vardhman Jain <vardhman @ gmail.com>
 * Date  : 2007-16-07
 * Description : 
 * 
 * Copyright 2007 by Vardhman Jain <vardhman @ gmail.com>

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

#ifndef PICASAWEBLOGIN_H
#define PICASAWEBLOGIN_H

#include <qdialog.h>

class QLabel;
class QLineEdit;

namespace KIPIPicasawebExportPlugin
{

class PicasawebLogin : public QDialog
{
public:
    
    PicasawebLogin(QWidget* parent, const QString& header,
                 const QString& _name=QString(),
                 const QString& _passwd=QString());
    ~PicasawebLogin();

    QString name()     const;
    QString password() const;
    QString username() const;    
    QString setUsername(const QString&);    
    QString setPassword(const QString&);  
private:

    QLabel*    m_headerLabel;
    QLineEdit* m_nameEdit;
    QLineEdit* m_passwdEdit;
};

}

#endif 
