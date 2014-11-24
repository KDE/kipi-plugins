/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Yandex.Fotki web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2011      by Roman Tsisyk <roman at tsisyk dot com>
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

#ifndef YANDEXFOTKI_LOGINDIALOG_H
#define YANDEXFOTKI_LOGINDIALOG_H

#include <QDialog>

class QLabel;
class KLineEdit;

namespace KIPIYandexFotkiPlugin
{

class LoginDialog : public QDialog
{
    Q_OBJECT

public:

    LoginDialog(QWidget* const parent, const QString& header,
                const QString& passwd=QString());
    ~LoginDialog();

    QString login() const;
    QString password() const;

    void setLogin(const QString&);
    void setPassword(const QString&);

protected Q_SLOTS:

    void slotAccept();
    void slotProcessUrl(const QString& url);

protected:

    QLabel*    m_headerLabel;
    KLineEdit* m_loginEdit;
    KLineEdit* m_passwordEdit;
};

} // namespace KIPIYandexFotkiPlugin

#endif // YANDEXFOTKI_LOGINDIALOG_H
