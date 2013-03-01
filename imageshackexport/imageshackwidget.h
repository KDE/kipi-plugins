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

class QRadioButton;
class QSpinBox;
class QCheckBox;
class QLineEdit;
class QLabel;
class QGroupBox;
class KComboBox;
class KPushButton;

namespace KIPIPlugins
{
    class KPImagesList;
    class KPProgressWidget;
}

namespace KIPIImageshackExportPlugin
{

class Imageshack;

class ImageshackWidget : public QWidget
{
    Q_OBJECT

public:
    ImageshackWidget(QWidget* const parent, Imageshack* const imageshack);
    ~ImageshackWidget();

    KIPIPlugins::KPImagesList* imagesList() const;
    KIPIPlugins::KPProgressWidget* progressBar() const;

    void getGalleriesDone(int errCode);

Q_SIGNALS:

    void signalReloadGalleries();

private:

    void updateLabels();
    void updateResizeOpts();

    void removeVideosFromList();

private Q_SLOTS:

    void slotEnablePredefComboBox(bool checked);
    void slotEnableCustomSize(bool checked);
    void slotGetGalleries(const QStringList& gTexts, const QStringList& gNames);
    void slotEnableNewGalleryLE(int index);
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

    KPushButton*                   m_chgRegCodeBtn;
    KPushButton*                   m_reloadGalleriesBtn;

    KComboBox*                     m_resizeOptsCob;
    KComboBox*                     m_galleriesCob;

    QSpinBox*                      m_widthSpb;
    QSpinBox*                      m_heightSpb;

    QGroupBox*                     m_galleriesBox;
    QWidget*                       m_galleriesWidget;

    KIPIPlugins::KPProgressWidget* m_progressBar;

    friend class ImageshackWindow;

}; // class ImageshackWidget

}  // namespace KIPIImageshackExportPlugin

#endif // IMAGESHACKWIDGET_H
