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

#ifndef IMGURWIDGET_H
#define IMGURWIDGET_H

// Qt
#include <QWidget>
#include <QProgressBar>
#include <QLabel>

// KDE
#include <KDialog>
#include <KLocale>

// KIPI
#include "libkipi/interface.h"
#include "kpimageslist.h"
#include "imgurtalker.h"

namespace KIPIPlugins {
class KPImagesList;
}

namespace KIPI
{
class Interface;
}

namespace KIPIImgurExportPlugin
{
class ImgurWidget : public QWidget
{
    Q_OBJECT

public:
    ImgurWidget(KIPI::Interface* interface, QWidget* parent = 0);
    ~ImgurWidget();

    KIPIPlugins::KPImagesList* imagesList() const;
    QProgressBar* progressBar() const;

private:
    KIPIPlugins::KPImagesList   *m_imagesList;
    QLabel                      *m_headerLbl;
    QProgressBar                *m_progressBar;
};
} // namespace KIPIImgurExportPlugin
#endif /* IMGURWIDGET_H */
