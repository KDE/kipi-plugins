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

#ifndef IMAGESHACKWIDGET_H
#define IMAGESHACKWIDGET_H

// Qt includes

#include <QWidget>

//local includes

#include "kpsettingswidget.h"

class QRadioButton;
class QSpinBox;
class QCheckBox;
class QLineEdit;
class QLabel;
class QGroupBox;
class QComboBox;
class QPushButton;

namespace KIPIPlugins
{
    class KPImagesList;
    class KPProgressWidget;
}

using namespace KIPIPlugins;

namespace KIPIImageshackPlugin
{

class Imageshack;

class ImageshackWidget : public KPSettingsWidget
{
    Q_OBJECT

public:
    ImageshackWidget(QWidget* const parent, Imageshack* const imageshack, KIPI::Interface* const iface, const QString& pluginName);
    ~ImageshackWidget();
/*
    KIPIPlugins::KPImagesList* imagesList() const;
    KIPIPlugins::KPProgressWidget* progressBar() const;*/

//     void getGalleriesDone(int errCode);

Q_SIGNALS:

    void signalReloadGalleries();

private:

    void updateLabels(const QString& name = QString(), const QString& url = QString());
//     void updateResizeOpts();

//     void removeVideosFromList();

private Q_SLOTS:

//     void slotEnablePredefComboBox(bool checked);
    void slotGetGalleries(const QStringList& gTexts, const QStringList& gNames);
//     void slotEnableNewGalleryLE(int index);
    void slotReloadGalleries();

private:

    KIPIPlugins::KPImagesList*     m_imgList;

    Imageshack*                    m_imageshack;

    QLabel*                        m_headerLbl;
    QLabel*                        m_accountNameLbl;
    QLabel*                        m_accountEmailLbl;

    QLineEdit*                     m_tagsFld;
    QLineEdit*                     m_newGalleryName;

    QRadioButton*                  m_noResizeRdb;
    QRadioButton*                  m_predefSizeRdb;
    QRadioButton*                  m_customSizeRdb;

    QCheckBox*                     m_privateImagesChb;
    QCheckBox*                     m_remBarChb;
    QCheckBox*                     m_useGalleriesChb;

    QPushButton*                   m_chgRegCodeBtn;
    QPushButton*                   m_reloadGalleriesBtn;

    QComboBox*                     m_resizeOptsCob;
    QComboBox*                     m_galleriesCob;

    QSpinBox*                      m_widthSpb;
    QSpinBox*                      m_heightSpb;

    QGroupBox*                     m_galleriesBox;
    QWidget*                       m_galleriesWidget;

    KIPIPlugins::KPProgressWidget* m_progressBar;

    friend class ImageshackWindow;

}; // class ImageshackWidget

}  // namespace KIPIImageshackPlugin

#endif // IMAGESHACKWIDGET_H
