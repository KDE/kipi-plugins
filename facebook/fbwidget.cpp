/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-12-26
 * Description : a kipi plugin to import/export images to Facebook web service
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

#include "fbwidget.h"

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
#include <QComboBox>
#include <QApplication>
#include <QStyle>

// Libkipi includes

#include <KIPI/Interface>
#include <KIPI/UploadWidget>
#include <KIPI/ImageCollection>

// Local includes

#include "kpimageslist.h"
#include "kpprogresswidget.h"

namespace KIPIFacebookPlugin
{

FbWidget::FbWidget(QWidget* const parent, KIPI::Interface* const iface, const QString& pluginName)
    : KPSettingsWidget(parent, iface, pluginName)
{
    getUploadBox()->hide();
    getSizeBox()->hide();

    connect(getReloadBtn(), SIGNAL(clicked()),
            this, SLOT(slotReloadAlbumsRequest()));
}

FbWidget::~FbWidget()
{
}

void FbWidget::updateLabels(const QString& name, const QString& url)
{
    QString web(QString::fromLatin1("http://www.facebook.com"));

    if (!url.isEmpty())
        web = url;

    getHeaderLbl()->setText(QString::fromLatin1(
        "<b><h2><a href='%1'>"
        "<font color=\"#3B5998\">facebook</font>"
        "</a></h2></b>").arg(web));
    if (name.isEmpty())
    {
        getUserNameLabel()->clear();
    }
    else
    {
        getUserNameLabel()->setText(QString::fromLatin1("<b>%1</b>").arg(name));
    }
}

void FbWidget::slotReloadAlbumsRequest()
{
    emit reloadAlbums(0);
}

} // namespace KIPIFacebookPlugin
