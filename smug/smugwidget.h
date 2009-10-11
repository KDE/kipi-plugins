/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-01
 * Description : a kipi plugin to import/export images to/from 
                 SmugMug web service
 *
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
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

#ifndef SMUGWIDGET_H
#define SMUGWIDGET_H

// Qt includes

#include <QWidget>

class QLabel;
class QSpinBox;
class QCheckBox;
class QRadioButton;
class KPushButton;
class KComboBox;
class KLineEdit;

namespace KIPI
{
    class Interface;
    class UploadWidget;
}

namespace KIPIPlugins
{
    class ImagesList;
}

namespace KIPISmugPlugin
{

class SmugWidget : public QWidget
{
    Q_OBJECT

public:
    SmugWidget(QWidget* parent, KIPI::Interface *iface, bool import);
    ~SmugWidget();

    void updateLabels(const QString& email = "", const QString& name = "", 
                      const QString& nick = "");

    bool isAnonymous();
    void setAnonymous(bool checked);

    QString getNickName();
    void setNickName(const QString& nick);

    QString getSitePassword();
    QString getAlbumPassword();

    QString getDestinationPath();

Q_SIGNALS:
    void signalUserChangeRequest(bool anonymous);

private Q_SLOTS:
    void slotAnonymousToggled(bool checked);
    void slotChangeUserClicked();
    void slotResizeChecked();

private:
    KIPIPlugins::ImagesList*   m_imgList;
    KIPI::UploadWidget*        m_uploadWidget;

    QLabel*       m_headerLbl;
    QRadioButton* m_anonymousRBtn;
    QRadioButton* m_accountRBtn;
    QLabel*       m_userNameLbl;
    QLabel*       m_userName;
    QLabel*       m_emailLbl;
    QLabel*       m_email;
    KPushButton*  m_changeUserBtn;

    KComboBox*    m_albumsCoB;
    KPushButton*  m_newAlbumBtn;
    KPushButton*  m_reloadAlbumsBtn;
    QLabel*       m_nickNameLbl;
    KLineEdit*    m_nickNameEdt;
    QLabel*       m_sitePasswordLbl;
    KLineEdit*    m_sitePasswordEdt;
    QLabel*       m_albumPasswordLbl;
    KLineEdit*    m_albumPasswordEdt;

    QCheckBox*    m_resizeChB;
    QSpinBox*     m_dimensionSpB;
    QSpinBox*     m_imageQualitySpB;

    friend class SmugWindow;
};

} // namespace KIPISmugPlugin

#endif // SMUGWIDGET_H
