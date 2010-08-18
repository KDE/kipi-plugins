/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-24
 * Description : a dialog to display processed messages in background
 *
 * Copyright (C) 2009-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef OUTPUTDIALOG_H
#define OUTPUTDIALOG_H

// Qt includes

#include <QString>

// KDE includes

#include <kdialog.h>

// Local includes

#include "kipiplugins_export.h"

namespace KIPIPlugins
{

class KPAboutData;

class KIPIPLUGINS_EXPORT OutputDialog : public KDialog
{
    Q_OBJECT

public:

    explicit OutputDialog(QWidget* parent=0,
                          const QString& caption=QString(),
                          const QString& Messages=QString(),
                          const QString& Header=QString());
    ~OutputDialog();

    void setAboutData(KPAboutData* about, const QString& handbookName);

private Q_SLOTS:

    void slotHelp();
    void slotCopyToCliboard();

private:

    class OutputDialogPriv;
    OutputDialogPriv* const d;
};

}  // namespace KIPIPlugins

#endif  // OUTPUTDIALOG_H
