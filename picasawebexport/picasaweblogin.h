/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef PICASAWEBLOGIN_H
#define PICASAWEBLOGIN_H

// Qt includes

#include <QDialog>

class QLabel;
class KLineEdit;

namespace KIPIPicasawebExportPlugin
{

class PicasawebLogin : public QDialog
{
    Q_OBJECT

public:

    PicasawebLogin(QWidget* parent, const QString& header, const QString& _name=QString(),
                   const QString& _passwd=QString());
    ~PicasawebLogin();

    QString name()     const;
    QString password() const;
    QString username() const;
    void setUsername(const QString&);
    void setPassword(const QString&);

private:

    QLabel*    m_headerLabel;
    KLineEdit* m_nameEdit;
    KLineEdit* m_passwdEdit;
};

} // namespace KIPIPicasawebExportPlugin

#endif // PICASAWEBLOGIN_H
