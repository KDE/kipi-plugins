/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2010 by Jens Mueller <tschenser at gmx dot de>
 * Copyright (C) 2010 by Caulier Gilles <caulier dot gilles at gmail dot com>
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

#ifndef PICASAWEBWIDGET_H
#define PICASAWEBWIDGET_H

// Qt includes

#include <QWidget>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/uploadwidget.h>

// Local includes

#include "kpimageslist.h"

class QLabel;
class QSpinBox;
class QCheckBox;
class QRadioButton;
class QProgressBar;
class KPushButton;
class KComboBox;

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIPicasawebExportPlugin
{

enum PicasawebTagsBehaviour
{
    PwTagLeaf = 0,
    PwTagSplit,
    PwTagCombined
};

class PicasawebWidget : public QWidget
{
    Q_OBJECT

public:

    PicasawebWidget(QWidget* const parent, Interface* const iface, bool import);
    ~PicasawebWidget();

    void updateLabels(const QString& name = QString());

    QString getDestinationPath();

    KPImagesList* imagesList() const;

    QProgressBar* progressBar() const;

Q_SIGNALS:

    void signalUserChangeRequest(bool anonymous);

private Q_SLOTS:

    void slotChangeUserClicked();
    void slotResizeChecked();

private:

    friend class PicasawebWindow;

    QLabel*       m_headerLbl;
    QLabel*       m_userNameLbl;
    QLabel*       m_userName;

    QRadioButton* m_anonymousRBtn;
    QRadioButton* m_accountRBtn;

    QCheckBox*    m_resizeChB;

    QSpinBox*     m_dimensionSpB;
    QSpinBox*     m_imageQualitySpB;

    KComboBox*    m_albumsCoB;
    KComboBox*    m_dlDimensionCoB;
    QButtonGroup* m_tagsBGrp;

    KPushButton*  m_newAlbumBtn;
    KPushButton*  m_reloadAlbumsBtn;
    KPushButton*  m_changeUserBtn;

    KPImagesList* m_imgList;
    UploadWidget* m_uploadWidget;

    QProgressBar* m_progressBar;
};

} // namespace KIPIPicasawebExportPlugin

#endif // PICASAWEBWIDGET_H
