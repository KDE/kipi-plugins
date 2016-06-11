/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2015-07-28
 * Description : Common widgets shared by plugins
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
 * Copyright (C) 2015 by Shourya Singh Gupta <shouryasgupta at gmail dot com>
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

#ifndef KPSETTINGSWIDGET_H
#define KPSETTINGSWIDGET_H

//Qt includes

#include <QWidget>

// Local includes

#include "kipiplugins_export.h"
#include "kpimageslist.h"
#include "kpprogresswidget.h"

class QLabel;
class QSpinBox;
class QCheckBox;
class QButtonGroup;
class QComboBox;
class QPushButton;
class QGroupBox;
class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;

namespace KIPI
{
    class Interface;
    class UploadWidget;
}

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT KPSettingsWidget : public QWidget
{
    Q_OBJECT

public:

    KPSettingsWidget(QWidget* const parent, KIPI::Interface* const iface, const QString& pluginName);
    ~KPSettingsWidget();

    virtual void updateLabels(const QString& name = QString(), const QString& url = QString()) = 0;
    QString getDestinationPath()    const;

    KPImagesList*     imagesList()  const;
    KPProgressWidget* progressBar() const;
    void              replaceImageList(QWidget* const widget);

    QWidget*          getSettingsBox() const;
    QVBoxLayout*      getSettingsBoxLayout() const;
    void              addWidgetToSettingsBox(QWidget* const widget);

    QGroupBox*        getAlbumBox() const;
    QGridLayout*      getAlbumBoxLayout() const;

    QGroupBox*        getOptionsBox() const;
    QGridLayout*      getOptionsBoxLayout() const;

    QGroupBox*        getUploadBox() const;
    QVBoxLayout*      getUploadBoxLayout() const;

    QGroupBox*        getSizeBox() const;
    QVBoxLayout*      getSizeBoxLayout() const;

    QGroupBox*        getAccountBox() const;
    QGridLayout*      getAccountBoxLayout() const;

    QLabel*           getHeaderLbl() const;
    QLabel*           getUserNameLabel() const;
    QPushButton*      getChangeUserBtn() const;
    QComboBox*        getDimensionCoB() const;
    QPushButton*      getNewAlbmBtn() const;
    QPushButton*      getReloadBtn() const;
    QCheckBox*        getResizeCheckBox() const;
    QSpinBox*         getDimensionSpB() const;
    QSpinBox*         getImgQualitySpB() const;
    QComboBox*        getAlbumsCoB() const;


protected Q_SLOTS:

    void slotResizeChecked();

private:

    class Private;
    Private* const d;
};

} // namespace KIPIPlugins

#endif /* KPSETTINGSWIDGET_H */
