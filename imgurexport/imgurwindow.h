/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-12
 * Description : a kipi plugin to export images to the Imgur web service
 *
 * Copyright (C) 2012-2012 by Marius Orcsik <marius at habarnam dot ro>
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

#ifndef IMGURWINDOW_H
#define IMGURWINDOW_H

// Qt includes

#include <QObject>

// KDE includes

#include <kdialog.h>
#include <klocale.h>

// LibKIPI includes

#include "libkipi/interface.h"

// Local includes

#include "kpimageslist.h"
#include "imgurtalker.h"
#include "imgurwidget.h"

namespace KIPIPlugins {
class KPImagesList;
}

namespace KIPI
{
class Interface;
}

namespace KIPIImgurExportPlugin
{
class ImgurWindow : public KDialog
{
    Q_OBJECT

public:
    ImgurWindow(KIPI::Interface* interface, QWidget* parent = 0);
    ~ImgurWindow();

    void reactivate();

private:
    ImgurTalker* m_webService;
    ImgurWidget* m_widget;

    int m_imagesCount;
    int m_imagesTotal;


public Q_SLOTS:
//    void slotHelp();
    void slotImageListChanged();
    void slotStartUpload();
    void slotBusy(bool val);

    void slotButtonClicked(int button);
//    void slotAddPhotoDone(int errCode, const QString& errMsg);
    void slotAddPhotoDone();
    void slotAddPhotoSuccess (ImgurSuccess success);
    void slotAddPhotoError (ImgurError error);

private:
    void uploadNextItem();
    void closeEvent(QCloseEvent* e);
    void close();
};
} // namespace KIPIImgurExportPlugin
#endif /* IMGURWINDOW_H */
