/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-04-12
 * Description : A KIPI Plugin to export albums to rajce.net
 *
 * Copyright (C) 2011 by Lukas Krejci <krejci.l at centrum dot cz>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef KIPIRAJCEEXPORTPLUGIN_RAJCELOGINDIALOG_H
#define KIPIRAJCEEXPORTPLUGIN_RAJCELOGINDIALOG_H

#include <QDialog>

class KLineEdit;

namespace KIPIRajceExportPlugin {

class RajceLoginDialog : public QDialog
{
    Q_OBJECT

public:

    RajceLoginDialog(QWidget* parent, const QString& _name=QString(),
                   const QString& _passwd=QString());
    ~RajceLoginDialog();

    QString password() const;
    QString username() const;
    void setUsername(const QString&);
    void setPassword(const QString&);

private:

    KLineEdit* _nameEdit;
    KLineEdit* _passwdEdit;
};

}

#endif // KIPIRAJCEEXPORTPLUGIN_RAJCELOGINDIALOG_H
