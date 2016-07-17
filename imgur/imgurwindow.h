/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-12
 * Description : a kipi plugin to export images to the Imgur web service
 *
 * Copyright (C) 2010-2012 by Marius Orcsik <marius at habarnam dot ro>
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
#include <QLabel>

// Libkipi includes

#include <KIPI/Interface>

// Local includes

#include "imgurimageslist.h"
#include "kptooldialog.h"
#include "imgurapi3.h"

namespace KIPI
{
    class Interface;
}

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIImgurPlugin
{

class ImgurWindow : public KPToolDialog
{
    Q_OBJECT

public:
    ImgurWindow(QWidget* const parent = 0);
    ~ImgurWindow();

    void reactivate();

public Q_SLOTS:
    /* UI callbacks */
    void forgetButtonClicked();
    void slotUpload();
    void slotAnonUpload();
    void slotFinished();
    void slotCancel();

    /* ImgurAPI3 callbacks */
    void apiAuthorized(bool success, const QString& username);
    void apiAuthError(const QString& msg);
    void apiProgress(unsigned int percent, const ImgurAPI3Action& action);
    void apiRequestPin(const QUrl& url);
    void apiSuccess(const ImgurAPI3Result& result);
    void apiError(const QString &msg, const ImgurAPI3Action& action);
    void apiBusy(bool busy);

private:
    void closeEvent(QCloseEvent* e);
    void setContinueUpload(bool state);
    void readSettings();
    void saveSettings();

private:
    ImgurImagesList* list = nullptr;
    ImgurAPI3*       api  = nullptr;
    QPushButton*     forgetButton = nullptr;
    QPushButton*     uploadAnonButton = nullptr;
    QLabel*          userLabel = nullptr;
    /* Contains the imgur username if API authorized.
     * If not, username is null. */
    QString          username;
};

} // namespace KIPIImgurPlugin

#endif /* IMGURWINDOW_H */
