/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Google-Drive web service
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
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

#ifndef GSWIDGET_H
#define GSWIDGET_H

//Qt includes

#include <QWidget>

// Local includes

#include "kpsettingswidget.h"

class QLabel;
class QSpinBox;
class QCheckBox;
class QButtonGroup;
class QComboBox;
class QPushButton;

namespace KIPI
{
    class Interface;
    class UploadWidget;
}

using namespace KIPIPlugins;

namespace KIPIGoogleServicesPlugin
{
  
enum PicasawebTagsBehaviour
{
    PwTagLeaf = 0,
    PwTagSplit,
    PwTagCombined
};

class GoogleServicesWidget : public KPSettingsWidget
{
    Q_OBJECT

public:

    GoogleServicesWidget(QWidget* const parent, KIPI::Interface* const iface, const QString& pluginName, const QString& serviceName);
    ~GoogleServicesWidget();

    virtual void updateLabels(const QString& name = QString(), const QString& url = QString());

private:
  
    bool                           m_picasaExport;
    bool                           m_picasaImport;
    bool                           m_gdrive;
    
    QString                        m_serviceName;
    QButtonGroup*                  m_tagsBGrp;

    friend class GSWindow;
};

} // namespace KIPIGoogleServicesPlugin

#endif /* GSWIDGET_H */
