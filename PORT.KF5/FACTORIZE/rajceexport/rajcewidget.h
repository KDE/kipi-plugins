/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

#ifndef KIPIRAJCEEXPORTPLUGIN_RAJCEWIDGET_H
#define KIPIRAJCEEXPORTPLUGIN_RAJCEWIDGET_H

// Qt includes

#include <QWidget>

// Local includes

#include "sessionstate.h"

class QLabel;
class QRadioButton;
class QSpinBox;
class QProgressBar;

class KComboBox;
class KPushButton;

namespace KIPI
{
    class Interface;
    class UploadWidget;
}

namespace KIPIPlugins
{
    class KPImagesList;
}

namespace KIPIRajceExportPlugin
{

class RajceSession;

class RajceWidget : public QWidget
{
    Q_OBJECT

public:

    explicit RajceWidget(KIPI::Interface* const interface, const QString& tmpFolder, QWidget* const parent);

Q_SIGNALS:

    void loginStatusChanged(bool loggedIn);

public Q_SLOTS:

    void update();
    void reactivate();

    void startUpload();
    void cancelUpload();

    void writeSettings();
    void readSettings();

private Q_SLOTS:

    void changeUserClicked();

    void progressStarted(unsigned);
    void progressFinished(unsigned);
    void progressChange(unsigned, unsigned percent);

    void loadAlbums();
    void createAlbum();
    void closeAlbum();

    void uploadNext();

    void startUploadAfterAlbumOpened();
    void selectedAlbumChanged(const QString&);

private:

    void _setEnabled(bool);

private:

    QLabel*                    m_headerLbl;
    QLabel*                    m_userNameLbl;
    QLabel*                    m_userName;

    QRadioButton*              m_anonymousRBtn;
    QRadioButton*              m_accountRBtn;

    QSpinBox*                  m_dimensionSpB;
    QSpinBox*                  m_imageQualitySpB;

    KComboBox*                 m_albumsCoB;

    KPushButton*               m_newAlbumBtn;
    KPushButton*               m_reloadAlbumsBtn;
    KPushButton*               m_changeUserBtn;

    KIPIPlugins::KPImagesList* m_imgList;
    KIPI::UploadWidget*        m_uploadWidget;

    QProgressBar*              m_progressBar;

    RajceSession*              m_session;

    QList<QString>             m_uploadQueue;
    QList<QString>::Iterator   m_currentUploadImage;

    bool                       m_uploadingPhotos;
    bool                       m_lastLoggedInState;
    QString                    m_currentAlbumName;
};

} // namespace KIPIRajceExportPlugin

#endif // KIPIRAJCEEXPORTPLUGIN_RAJCEWIDGET_H
