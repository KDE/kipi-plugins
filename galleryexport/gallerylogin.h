/* ============================================================
 * File  : gallerylogin.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-11-04
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

#ifndef GALLERYLOGIN_H
#define GALLERYLOGIN_H

#include <qdialog.h>

class QLabel;
class QLineEdit;
class QCheckBox;

namespace KIPIGalleryExportPlugin
{

class GalleryLogin : public QDialog
{
public:

    GalleryLogin(QWidget* parent, const QString& header,
                 const QString& _url=QString(),
                 const QString& _name=QString(),
                 const QString& _passwd=QString(),
                 const bool _use_gallery2=false);
    ~GalleryLogin();

    QString url()      const;
    QString name()     const;
    QString password() const;
    bool isgGallery2Enable() const;

private:

    QLabel*    m_headerLabel;
    QLineEdit* m_urlEdit;
    QLineEdit* m_nameEdit;
    QLineEdit* m_passwdEdit;
    QCheckBox* m_galleryVersion;
};

}

#endif 
