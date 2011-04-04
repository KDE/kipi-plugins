/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-03-14
 * Description : a dialog to edit EXIF,IPTC and XMP metadata
 *
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

#ifndef EDITALLMETADATA_H
#define EDITALLMETADATA_H

#include <kpagedialog.h>
#include <kurl.h>
#include <QObject>

class QObject;
class QCloseEvent;
class Event;

namespace KIPI
{
    class Interface;
}

using namespace KIPI;

namespace KIPIMetadataEditPlugin
{

class MetadataEditDialogPrivate;

class MetadataEditDialog : public KDialog
{
    Q_OBJECT
        
public:
    MetadataEditDialog(QWidget *parent,KUrl::List urls,Interface *iface);
    ~MetadataEditDialog();
        
public Q_SLOTS:

    void slotModified();

private Q_SLOTS:

    void slotHelp();
    void slotOk();
    void slotClose();
    void slotItemChanged();
    void slotApply();
    void slotUser1();
    void slotUser2();
    void slotSetReadOnly(bool);
    void setWindowTitle(int);
    
Q_SIGNALS:

    void signalApply();
    void signalClose();
    void signalUser1();
    void signalUser2();    
    void signalOk();
    
protected:

    void closeEvent(QCloseEvent*);
    bool eventFilter(QObject*, QEvent*);
        
private:

    void saveSettings();
    void readSettings();

private:

    MetadataEditDialogPrivate* const d;
};

}  // namespace KIPIMetadataEditPlugin

#endif  /* EDITALLMETADATA_H */
