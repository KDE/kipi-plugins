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

#include "gswidget.h"

// Qt includes

#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QComboBox>

// KDE includes

#include <klocalizedstring.h>

namespace KIPIGoogleServicesPlugin
{

GoogleServicesWidget::GoogleServicesWidget(QWidget* const parent, KIPI::Interface* const iface, const PluginName& pluginName, const QString& serviceName):KPSettingsWidget(parent,iface,serviceName)
{
    m_pluginName = pluginName;
    
    QGroupBox* m_LeafBox = new QGroupBox(QStringLiteral(""), getSettingsBox());
    QGridLayout* leafLayout = new QGridLayout(m_LeafBox);
    
    m_tagsBGrp = new QButtonGroup(m_LeafBox);
    
    if(m_pluginName == PluginName::PicasaExport)
    {
        QSpacerItem* const spacer = new QSpacerItem(1, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QLabel* const tagsLbl     = new QLabel(i18n("Tag path behavior :"), m_LeafBox);

        QRadioButton* const leafTagsBtn     = new QRadioButton(i18n("Leaf tags only"), m_LeafBox);
        leafTagsBtn->setWhatsThis(i18n("Export only the leaf tags of tag hierarchies"));
        QRadioButton* const splitTagsBtn    = new QRadioButton(i18n("Split tags"), m_LeafBox);
        splitTagsBtn->setWhatsThis(i18n("Export the leaf tag and all ancestors as single tags."));
        QRadioButton* const combinedTagsBtn = new QRadioButton(i18n("Combined String"), m_LeafBox);
        combinedTagsBtn->setWhatsThis(i18n("Build a combined tag string."));

        m_tagsBGrp->addButton(leafTagsBtn, PwTagLeaf);
        m_tagsBGrp->addButton(splitTagsBtn, PwTagSplit);
        m_tagsBGrp->addButton(combinedTagsBtn, PwTagCombined);

        leafLayout->addItem(spacer,             0, 1, 1, 1);
        leafLayout->addWidget(tagsLbl,          1, 1, 1, 1);
        leafLayout->addWidget(leafTagsBtn,      2, 1, 1, 1);
        leafLayout->addWidget(splitTagsBtn,     3, 1, 1, 1);
        leafLayout->addWidget(combinedTagsBtn,  4, 1, 1, 1);  
        
        addWidgetToSettingsBox(m_LeafBox);
    }

    switch(m_pluginName)
    {
        case PluginName::PicasaImport :
            imagesList()->hide();
            getNewAlbmBtn()->hide();
            getOptionsBox()->hide();            
            break;
        default :
            getUploadBox()->hide();
            getSizeBox()->hide();  
            break;
    }
}

GoogleServicesWidget::~GoogleServicesWidget()
{
}

void GoogleServicesWidget::updateLabels(const QString& name, const QString& url)
{
    switch(m_pluginName)
    {
        case PluginName::GDrive :
        {
            QString web(QStringLiteral("http://www.drive.google.com"));
            getHeaderLbl()->setText(QStringLiteral(
                "<b><h2><a href='%1'>"
                "<font color=\"#9ACD32\">Google Drive</font>"
                "</a></h2></b>").arg(web));
            break;
        }
        default :
            getHeaderLbl()->setText(QStringLiteral(
                "<b><h2><a href='http://picasaweb.google.com/%1'>"
                "<font color=\"#9ACD32\">Google Photos/PicasaWeb</font>"
                "</a></h2></b>").arg(url));
            break;
    }

    if (name.isEmpty())
    {
        getUserNameLabel()->clear();
    }
    else
    {
        getUserNameLabel()->setText(QStringLiteral("<b>%1</b>").arg(name));
    }
}

} // namespace KIPIGoogleServicesPlugin
