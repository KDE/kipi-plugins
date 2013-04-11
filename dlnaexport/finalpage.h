/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-05-28
 * Description : a KIPI plugin to export pics through DLNA technology.
 *
 * Copyright (C) 2012 by Smit Mehta <smit dot meh at gmail dot com>
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

#ifndef FINALPAGE_H
#define FINALPAGE_H

// Qt includes

#include <QWidget>
#include <QString>
#include <kurl.h>

// Local includes

#include "welcomepage.h"

namespace KIPIDLNAExportPlugin
{

class MediaServerWindow;

class FinalPage : public QWidget
{
    Q_OBJECT

public:

    explicit FinalPage(QWidget* const parent);
    ~FinalPage();

    void setImages(const KUrl::List&);
    void setCollectionMap(const QMap<QString, KUrl::List>&);
    void setDirectories (const QStringList&);
    void clearImages();
    void setOptions(WelcomePage::ImplementationGetOption);
    void setMinidlnaBinaryPath(const QString&);

    void startHupnpMediaServer();
    void startMinidlnaMediaServer();

Q_SIGNALS:

    void sharing(bool state);

public Q_SLOTS:

    void turnOff();
    void turnOn();

private:

    class Private;
    Private* const d;
};

} // namespace KIPIDLNAExportPlugin

#endif // FINALPAGE_H

