/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "kpaboutdata.h"

class QTextBrowser;

namespace KIPIBatchProcessImagesPlugin
{

class OutputDialog : public KDialog
{
    Q_OBJECT

public:

    OutputDialog(QWidget* parent = 0, const QString& caption = QString(),
                 const QString& Messages = QString(), const QString& Header = QString());
    ~OutputDialog();

private Q_SLOTS:

    void slotHelp();
    void slotCopyToCliboard();

private:

    QTextBrowser*             m_debugView;

    KIPIPlugins::KPAboutData* m_about;
};

}  // namespace KIPIBatchProcessImagesPlugin

#endif  // OUTPUTDIALOG_H
