/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-02-11
 * Description : a kipi plugin to export images to WikiMedia web service
 *
 * Copyright (C) 2011 by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 * Copyright (C) 2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef WMWIDGET_H
#define WMWIDGET_H

// Qt includes

#include <QWidget>
#include <QTextEdit>

//KDE includes

#include <KLineEdit>
#include <KTextEdit>
#include <KConfig>

class QLabel;
class QSpinBox;
class QCheckBox;
class QButtonGroup;
class QProgressBar;
class QTabWidget;
class QComboBox;
class QLineEdit;

class KVBox;
class KHBox;
class KComboBox;
class KPushButton;

namespace KIPI
{
    class Interface;
    class UploadWidget;
}

namespace KIPIPlugins
{
    class ImagesList;
}

namespace KDcrawIface
{
    class RExpanderBox;
}

namespace KIPIWikiMediaPlugin
{

enum WmDownloadType
{
    WmMyAlbum = 0,
    WmFriendAlbum,
    WmPhotosMe,
    WmPhotosFriend
};

class WmWidget : public QWidget
{
    Q_OBJECT

public:

    WmWidget(QWidget* parent, KIPI::Interface* iface);
    ~WmWidget();

    void updateLabels(const QString& name = "", const QString& url = "");

    void invertAccountLoginBox();

    KIPIPlugins::ImagesList* imagesList() const;

    QProgressBar* progressBar() const;

    QString author();
    QString licence();
    QString description();

    void readSettings(KConfigGroup& group);
    void saveSettings(KConfigGroup& group);

Q_SIGNALS:

    void signalChangeUserRequest();
    void signalLoginRequest(const QString& login, const QString& pass, const QUrl& wiki);

private Q_SLOTS:

    void slotResizeChecked();
    void slotChangeUserClicked();
    void slotLoginClicked();

private:

    KVBox*                     m_userBox;
    QWidget*                   m_loginBox;
    QLabel*                    m_loginHeaderLbl;
    KLineEdit*                 m_nameEdit;
    KLineEdit*                 m_passwdEdit;
    QComboBox*                 m_wikiSelect;

    QWidget*                   m_textBox;
    QTextEdit*                 m_descriptionEdit;
    KLineEdit*                 m_authorEdit;

    KHBox*                     m_accountBox;
    QLabel*                    m_headerLbl;
    QLabel*                    m_userNameDisplayLbl;
    KPushButton*               m_changeUserBtn;

    QWidget*                   m_optionsBox;
    QCheckBox*                 m_resizeChB;
    QSpinBox*                  m_dimensionSpB;
    QSpinBox*                  m_imageQualitySpB;
    QComboBox*                 m_licenceComboBox;

    QProgressBar*              m_progressBar;

    KDcrawIface::RExpanderBox* m_settingsExpander;
    KIPIPlugins::ImagesList*   m_imgList;
    KIPI::UploadWidget*        m_uploadWidget;

    friend class WmWindow;
};

} // namespace KIPIWikiMediaPlugin

#endif // WMWIDGET_H
