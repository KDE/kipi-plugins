/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-03-14
 * Description : a KPageWidget to edit EXIF metadata
 *
 * Copyright (C) 2006-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2011 by Victor Dodon <dodonvictor at gmail dot com>
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
#include <kurl.h>

class QCloseEvent;
class QEvent;
class QObject;

namespace KIPI
{
    class Interface;
}

using namespace KIPI;

namespace KIPIMetadataEditPlugin
{

class allEXIFEditWidgetPrivate;

class allEXIFEditWidget : public KPageWidget
{
    Q_OBJECT

public:

    allEXIFEditWidget(QWidget* parent, KUrl::List urls, Interface* iface);
    ~allEXIFEditWidget();
    
    bool isModified();

Q_SIGNALS:
    void signalSetReadOnly( bool );
    void signalModified();

public Q_SLOTS:

    void slotModified();

protected Q_SLOTS:

    void slotOk();

private Q_SLOTS:
    void slotClose();
    void slotItemChanged();
    void slotApply();
    void slotUser1();
    void slotUser2();

private:
    void readSettings();
    void saveSettings();

    int  activePageIndex();
    void showPage(int page);

private:

    allEXIFEditWidgetPrivate* const d;
};

}  // namespace KIPIMetadataEditPlugin

#endif /* EXIFEDITDIALOG_H */
