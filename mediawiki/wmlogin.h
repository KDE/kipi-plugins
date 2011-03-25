/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-02-11
 * Description : a kipi plugin to export images to WikiMedia web service
 *
 * Copyright (C) 2011 by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
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

#ifndef WMLOGIN_H
#define WMLOGIN_H

// Qt includes

#include <QDialog>
#include <QComboBox>
#include <QUrl>

class QLabel;
class KLineEdit;

namespace KIPIWikiMediaPlugin
{

class WmLogin : public QDialog
{
    Q_OBJECT

public:

    WmLogin(QWidget* parent, const QString& header, const QString& _name=QString(),
            const QString& _passwd=QString());
    ~WmLogin();

    QString name()     const;
    QString password() const;
    QString username() const;
    QUrl wiki()        const;
    void setUsername(const QString&);
    void setPassword(const QString&);

private:

    QLabel*    m_headerLabel;
    KLineEdit* m_nameEdit;
    KLineEdit* m_passwdEdit;
    QComboBox* m_wikiSelect;
};

} // namespace KIPIWikiMediaPlugin

#endif // WMLOGIN_H
