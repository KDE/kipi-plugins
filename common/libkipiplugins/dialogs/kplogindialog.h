/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2015-11-12
 * Description : a common login dialog for kipi plugins
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2011      by Roman Tsisyk <roman at tsisyk dot com>
 * Copyright (C) 2015      by Shourya Singh Gupta <shouryasgupta at gmail dot com>
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

#ifndef KPLOGINDIALOG_H
#define KPLOGINDIALOG_H

#include <QDialog>

// Local includes

#include "kipiplugins_export.h"

class QLabel;
class QLineEdit;

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT KPLoginDialog : public QDialog
{
    Q_OBJECT

public:

    KPLoginDialog(QWidget* const parent, const QString& prompt,
                  const QString& header=QString(), const QString& passwd=QString());
    ~KPLoginDialog();

    QString login()    const;
    QString password() const;

    void setLogin(const QString&);
    void setPassword(const QString&);

protected Q_SLOTS:

    void slotAccept();

protected:

    QLabel*    m_headerLabel;
    QLineEdit* m_loginEdit;
    QLineEdit* m_passwordEdit;
};

} // namespace KIPIPlugins

#endif // KPLOGINDIALOG_H
