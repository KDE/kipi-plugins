/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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
    class KPImagesList;
    class KPProgressWidget;
}

namespace KIPISmugPlugin
{

class SmugWidget : public QWidget
{
    Q_OBJECT

public:

    SmugWidget(QWidget* const parent, KIPI::Interface* const iface, bool import);
    ~SmugWidget();

    void updateLabels(const QString& email = "", const QString& name = "", 
                      const QString& nick = "");

    bool isAnonymous() const;
    void setAnonymous(bool checked);

    QString getNickName() const;
    void    setNickName(const QString& nick);

    QString getSitePassword() const ;
    QString getAlbumPassword() const;
    QString getDestinationPath() const;

    KIPIPlugins::KPImagesList* imagesList() const;

    KIPIPlugins::KPProgressWidget* progressBar() const;

Q_SIGNALS:

    void signalUserChangeRequest(bool anonymous);

private Q_SLOTS:

    void slotAnonymousToggled(bool checked);
    void slotChangeUserClicked();
    void slotResizeChecked();

private:

    QLabel*                    m_headerLbl;
    QLabel*                    m_userNameLbl;
    QLabel*                    m_userName;
    QLabel*                    m_emailLbl;
    QLabel*                    m_email;
    QLabel*                    m_nickNameLbl;
    QLabel*                    m_sitePasswordLbl;
    QLabel*                    m_albumPasswordLbl;

    QRadioButton*              m_anonymousRBtn;
    QRadioButton*              m_accountRBtn;

    QCheckBox*                 m_resizeChB;

    QSpinBox*                  m_dimensionSpB;
    QSpinBox*                  m_imageQualitySpB;

    KComboBox*                 m_albumsCoB;

    KPushButton*               m_newAlbumBtn;
    KPushButton*               m_reloadAlbumsBtn;
    KPushButton*               m_changeUserBtn;

    KLineEdit*                 m_albumPasswordEdt;
    KLineEdit*                 m_nickNameEdt;
    KLineEdit*                 m_sitePasswordEdt;

    KIPIPlugins::KPProgressWidget* m_progressBar;
    KIPIPlugins::KPImagesList* m_imgList;
    KIPI::UploadWidget*        m_uploadWidget;

    friend class SmugWindow;
};

} // namespace KIPISmugPlugin

#endif // SMUGWIDGET_H
