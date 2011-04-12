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

#ifndef KIPIRAJCEEXPORTPLUGIN_RAJCEWIDGET_H
#define KIPIRAJCEEXPORTPLUGIN_RAJCEWIDGET_H

#include <QWidget>
#include "sessionstate.h"

class QLabel;
class QRadioButton;
class QCheckBox;
class QSpinBox;
class QButtonGroup;
class QProgressBar;

class KComboBox;
class KPushButton;

namespace KIPI {
    class Interface;
    class UploadWidget;
}

namespace KIPIPlugins {
    class ImagesList;
}

namespace KIPIRajceExportPlugin {

class RajceSession;
class Album;

class RajceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RajceWidget(KIPI::Interface* interface, const QString& tmpFolder, QWidget* parent);

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

    void uploadNext();

    void startUploadAfterAlbumOpened();

    void closeAlbum();

    void selectedAlbumChanged(const QString&);

private:

    void _setEnabled(bool);

    QLabel * _headerLbl;
    QLabel * _userNameLbl;
    QLabel * _userName;

    QRadioButton * _anonymousRBtn;
    QRadioButton * _accountRBtn;

    QSpinBox * _dimensionSpB;
    QSpinBox * _imageQualitySpB;

    KComboBox * _albumsCoB;

    KPushButton * _newAlbumBtn;
    KPushButton * _reloadAlbumsBtn;
    KPushButton * _changeUserBtn;

    KIPIPlugins::ImagesList * _imgList;
    KIPI::UploadWidget * _uploadWidget;

    QProgressBar * _progressBar;

    RajceSession * _session;

    QList<QString> _uploadQueue;
    QList<QString>::Iterator _currentUploadImage;

    bool _uploadingPhotos;
    bool _lastLoggedInState;
    QString _currentAlbumName;
};

}

#endif // KIPIRAJCEEXPORTPLUGIN_RAJCEWIDGET_H
