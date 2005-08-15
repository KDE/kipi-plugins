// ============================================================
// Copyright (C) 2003-2005 Gilles CAULIER <caulier dot gilles at free.fr>
// Copyright (C) 2005 by Owen Hirst <n8rider@sbcglobal.net>
//
// This program is free software; you can redistribute it
// and/or modify it under the terms of the GNU General
// Public License as published by the Free Software Foundation;
// either version 2, or (at your option)
// any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Steet, Fifth Floor, Cambridge, MA 02110-1301, USA.
// 
// ============================================================ 

#ifndef RENAMEIMAGESWIDGET_H
#define RENAMEIMAGESWIDGET_H

#include <kurl.h>

#include "renameimagesbase.h"

class KFileItem;
class QTimer;
class QProgressDialog;

namespace KIPI
{
class Interface;
}

namespace KIPIBatchProcessImagesPlugin
{

class BatchProcessImagesItem;

class RenameImagesWidget : public RenameImagesBase
{
    Q_OBJECT
    
public:

    enum SortOrder
    {
        BYNAME = 0,
        BYSIZE,
        BYDATE
    }; 

    RenameImagesWidget(QWidget *parent, KIPI::Interface* interface,
                       const KURL::List& urlList);
    ~RenameImagesWidget();

private:

    void readSettings();
    void saveSettings();
    void reListImages();
    void updateListing();
    QString oldToNewName(BatchProcessImagesItem* item,
                         int itemPosition);
    
    KIPI::Interface* m_interface;
    KURL::List       m_urlList;
    QTimer*          m_timer;
    QProgressDialog* m_progress;
    bool             m_overwriteAll;
    bool             m_autoSkip;

public slots:

    void slotStart();
    void slotAbort();
    void slotNext();
    
private slots:

    void slotListViewDoubleClicked(QListViewItem*);
    void slotImageSelected(QListViewItem*);
    void slotOptionsChanged();
    void slotGotPreview(const KFileItem*, const QPixmap&);

    void slotAddImages();
    void slotRemoveImage();
};

}

#endif /* RENAMEIMAGESWIDGET_H */
