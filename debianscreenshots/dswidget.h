/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-11-29
 * Description : a kipi plugin to export images to Debian Screenshots
 *
 * Copyright (C) 2010 by Pau Garcia i Quiles <pgquiles at elpauer dot org>
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

#ifndef DSWIDGET_H
#define DSWIDGET_H

// Qt includes

#include <QWidget>
#include <QUrl>

class QButtonGroup;
class QProgressBar;
class QNetworkReply;
class KComboBox;
class KLineEdit;

namespace KIO
{
    class AccessManager;
}

namespace KIPI
{
    class Interface;
    class UploadWidget;
}

namespace KIPIPlugins
{
    class ImagesList;
}

namespace KIPIDebianScreenshotsPlugin
{

class KClickableImageLabel;

class DsWidget : public QWidget
{
    Q_OBJECT

public:

    DsWidget(QWidget* parent, KIPI::Interface* iface);
    ~DsWidget();

     QString getDestinationPath() const;

     KIPIPlugins::ImagesList* imagesList() const;

    QProgressBar* progressBar() const;

Q_SIGNALS:

    void requiredPackageInfoAvailable(bool available);

private Q_SLOTS:

    void slotCompletePackageName(const QString&);
    void slotCompletePackageNameFinished(QNetworkReply*);
    void slotFindVersionsForPackage(const QString&);
    void slotFindVersionsForPackageFinished(QNetworkReply*);
    void slotEnableUpload();

private:

    QButtonGroup*            m_dlGrp;
    QString                  m_lastTip;
    QUrl                     m_lastQueryUrl;
    QProgressBar*            m_progressBar;

    KLineEdit*               m_pkgLineEdit;
    KComboBox*               m_versionsComboBox;
    KLineEdit*               m_descriptionLineEdit;

    KIO::AccessManager*      m_httpManager;
    KIO::AccessManager*      m_jsonManager;

    KClickableImageLabel*    m_headerLabel;

    KIPIPlugins::ImagesList* m_imgList;
    KIPI::UploadWidget*      m_uploadWidget;

    friend class DsWindow;
};

} // namespace KIPIDebianScreenshotsPlugin

#endif // DSWIDGET_H
