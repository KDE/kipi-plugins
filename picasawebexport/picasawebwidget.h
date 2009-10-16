/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
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

// Local includes

#include "ui_uploadwidget.h"

class QCheckBox;

class KHTMLPart;

namespace KIPIPicasawebExportPlugin
{


class UploadWidget : public QWidget, public Ui::UploadWidget
{
public:

    UploadWidget( QWidget *parent ) : QWidget( parent )
    {
        setupUi( this );
    }
};

class PicasawebWidget : public UploadWidget
{
    Q_OBJECT

public:

    PicasawebWidget(QWidget* parent=0);
    ~PicasawebWidget();

private Q_SLOTS:

    void slotResizeChecked();
    void slotSelectionChecked();

private:

//  QCheckBox*    m_resizeCheckBox;
//  QCheckBox*    m_exportApplicationTags;

    KHTMLPart*    m_photoView;

    friend class PicasawebWindow;
};

} // namespace KIPIPicasawebExportPlugin

#endif // PICASAWEBWIDGET_H
