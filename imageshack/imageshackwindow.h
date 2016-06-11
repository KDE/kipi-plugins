/* ============================================================
*
* This file is a part of kipi-plugins project
* http://www.digikam.org
*
* Date        : 2012-02-02
* Description : a plugin to export photos or videos to ImageShack web service
*
* Copyright (C) 2012 Dodon Victor <dodonvictor at gmail dot com>
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

#ifndef IMAGESHACKWINDOW_H
#define IMAGESHACKWINDOW_H

// Qt includes

#include <QList>
#include <QTextStream>
#include <QFile>

// Local includes

#include "kptooldialog.h"

namespace KIPI
{
    class Interface;
}

namespace KIPIPlugins
{
    class KPImagesList;
}

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIImageshackPlugin
{

class Imageshack;
class ImageshackWidget;
class ImageshackTalker;
class NewAlbumDlg;

class ImageshackWindow : public KPToolDialog
{
    Q_OBJECT

public:

    ImageshackWindow(QWidget* const parent, Imageshack* const pImageshack);
    ~ImageshackWindow();

    KPImagesList* getImagesList() const;

Q_SIGNALS:

    void signalBusy(bool val);

private Q_SLOTS:

    void slotImageListChanged();
    void slotStartTransfer();
    void slotBusy(bool val);
    void slotFinished();
    void slotCancelClicked();

    void slotJobInProgress(int step, int, const QString& format);
    void slotLoginDone(int errCode, const QString& errMsg);
    void slotGetGalleriesDone(int errCode, const QString& errMsg);
    void slotGetGalleries();

    void slotAddPhotoDone(int errCode, const QString& errMsg);

    void authenticate();

private:

    void readSettings();
    void saveSettings();

    void authenticationDone(int errCode, const QString& errMsg);

    void uploadNextItem();

    void closeEvent(QCloseEvent* e);

private Q_SLOTS:

    void slotChangeRegistrantionCode();
    void slotNewAlbumRequest();

private:

    bool              m_import;
    unsigned int      m_imagesCount;
    unsigned int      m_imagesTotal;
    QString           m_newAlbmTitle;

    QList<QUrl>        m_transferQueue;

    Imageshack*       m_imageshack;
    ImageshackWidget* m_widget;
    ImageshackTalker* m_talker;

    NewAlbumDlg*      m_albumDlg;
};

} // namespace KIPIImageshackPlugin

#endif // IMAGESHACKWINDOW_H
