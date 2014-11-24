/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-10-23
 * Description : a kipi plugin to export images to shwup.com web service
 *
 * Copyright (C) 2009 by Timothée Groleau <kde at timotheegroleau dot com>
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

#ifndef SWLOGIN_H
#define SWLOGIN_H

// KDE includes

#include <kdialog.h>

class KLineEdit;

namespace KIPIShwupPlugin
{

class SwUser;

class SwLogin : public KDialog
{
    Q_OBJECT

public:

    SwLogin(QWidget* const parent);
    ~SwLogin();

    void getUserProperties(SwUser& user);

private:

    KLineEdit* m_emailEdt;
    KLineEdit* m_passwordEdt;

    friend class SwWindow;
};

} // namespace KIPIShwupPlugin

#endif // SWLOGIN_H
