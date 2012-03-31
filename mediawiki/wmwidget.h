/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-11
 * Description : a kipi plugin to export images to WikiMedia web service
 *
 * Copyright (C) 2011      by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 * Copyright (C) 2011-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Parthasarathy Gopavarapu <gparthasarathy93 at gmail dot com>
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

//KDE includes

#include <klineedit.h>
#include <ktextedit.h>
#include <kconfig.h>
#include <kurlrequester.h>
#include <kurl.h>

class QLabel;
class QSpinBox;
class QCheckBox;

class KVBox;
class KHBox;
class KPushButton;

namespace KIPI
{
    class UploadWidget;
}

namespace KIPIPlugins
{
    class KPImagesList;
    class KPProgressWidget;
}

namespace KDcrawIface
{
    class RExpanderBox;
    class SqueezedComboBox;
}

using namespace KIPI;
using namespace KIPIPlugins;
using namespace KDcrawIface;

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

    WmWidget(QWidget* const parent);
    ~WmWidget();

    void updateLabels(const QString& name = QString(), const QString& url = QString());

    void invertAccountLoginBox();

    KPImagesList* imagesList() const;

    KPProgressWidget* progressBar() const;

    QString author() const;
    QString licence() const;

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
    KUrlComboRequester*        m_wikiSelect;

    QWidget*                   m_textBox;

    KLineEdit*                 m_authorEdit;

    KHBox*                     m_accountBox;
    QLabel*                    m_headerLbl;
    QLabel*                    m_userNameDisplayLbl;
    KPushButton*               m_changeUserBtn;

    QWidget*                   m_optionsBox;
    QCheckBox*                 m_resizeChB;
    QSpinBox*                  m_dimensionSpB;
    QSpinBox*                  m_imageQualitySpB;
    SqueezedComboBox*          m_licenceComboBox;

    KPProgressWidget*          m_progressBar;

    RExpanderBox*              m_settingsExpander;
    KPImagesList*              m_imgList;
    UploadWidget*              m_uploadWidget;
    
    KUrl::List                 m_history;
    
    friend class WmWindow;
};

} // namespace KIPIWikiMediaPlugin

#endif // WMWIDGET_H
