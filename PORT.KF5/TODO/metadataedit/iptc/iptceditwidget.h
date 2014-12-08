/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-03-14
 * Description : a KPageWidget to edit IPTC metadata
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

#ifndef IPTCEDITWIDGET_H
#define IPTCEDITWIDGET_H

// KDE includes

#include <kpagedialog.h>

namespace KIPIMetadataEditPlugin
{

class MetadataEditDialog;

class IPTCEditWidget : public KPageWidget
{
    Q_OBJECT

public:

    explicit IPTCEditWidget(MetadataEditDialog* const parent);
    ~IPTCEditWidget();

    bool isModified() const;
    void saveSettings();
    void apply();

Q_SIGNALS:

    void signalModified();
    void signalSetReadOnly(bool);

public Q_SLOTS:

    void slotModified();
    void slotItemChanged();

private:

    void readSettings();

    int  activePageIndex() const;
    void showPage(int page);

private:

    class IPTCEditWidgetPrivate;
    IPTCEditWidgetPrivate* const d;
};

}  // namespace KIPIMetadataEditPlugin

#endif /* IPTCEDITWIDGET_H */
