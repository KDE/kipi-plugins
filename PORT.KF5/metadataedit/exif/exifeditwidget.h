/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-03-14
 * Description : a KPageWidget to edit EXIF metadata
 *
 * Copyright (C) 2006-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2011 by Victor Dodon <dodon dot victor at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef EXIFEDITWIDGET_H
#define EXIFEDITWIDGET_H

// KDE includes

#include <kpagewidget.h>

namespace KIPIMetadataEditPlugin
{

class MetadataEditDialog;

class EXIFEditWidget : public KPageWidget
{
    Q_OBJECT

public:

    explicit EXIFEditWidget(MetadataEditDialog* const parent);
    ~EXIFEditWidget();

    bool isModified() const;
    void saveSettings();
    void apply();

Q_SIGNALS:

    void signalSetReadOnly(bool);
    void signalModified();

public Q_SLOTS:

    void slotModified();
    void slotItemChanged();

private:

    void readSettings();

    int  activePageIndex() const;
    void showPage(int page);

private:

    class EXIFEditWidgetPrivate;
    EXIFEditWidgetPrivate* const d;
};

}  // namespace KIPIMetadataEditPlugin

#endif /* EXIFEDITDIALOG_H */
