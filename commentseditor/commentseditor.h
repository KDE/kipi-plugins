/* ============================================================
 * File  : commentseditor.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-09-26
 * Description :
 *
 * Copyright 2003 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef COMMENTSEDITOR_H
#define COMMENTSEDITOR_H

// Qt includes.

#include <qguardedptr.h>

// KDE includes.

#include <kdialogbase.h>

// LibKipi includes.

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

class QListViewItem;
class QPixmap;

class KFileItem;
class KURL;

namespace KIPICommentsEditorPlugin
{

class CListView;
class CLineEdit;

class CommentsEditor : public KDialogBase
{
    Q_OBJECT

public:

    CommentsEditor( KIPI::Interface* interface, KIPI::ImageCollection images, QWidget *parent=0L );
    ~CommentsEditor();

private:

    void loadItems();

    CListView                        *m_listView;
    CLineEdit                        *m_edit;
    KIPI::ImageCollection             m_images;
    KIPI::Interface*                  m_interface;

private slots:

    void slotGotPreview(const KFileItem* url,
                        const QPixmap &pixmap);
    void slotSelectionChanged();
    void slotCommentChanged(const QString& newComment);
    void slotOkClicked();
    void slotAboutClicked();
};

} // NameSpace KIPICommentsEditorPlugin

#endif  // COMMENTSEDITOR_H
