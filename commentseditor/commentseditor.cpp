/* ============================================================
 * File  : commentseditor.cpp
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

#include <klocale.h>
#include <kurl.h>
#include <kmessagebox.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qvgroupbox.h>
#include <qlineedit.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qbrush.h>
#include <qfont.h>
#include <qevent.h>

#include <libkipi/imagecollection.h>
#include <libkipi/imageinfo.h>

#include "commentseditor.h"
#include <kio/previewjob.h>

namespace CommentsPlugin
{

// --------------------------------------------------------------------

class CommentsListViewItem : public QListViewItem
{
public:

    CommentsListViewItem( const KIPI::ImageInfo& info, const KURL& url, QListView *parent)
        : QListViewItem(parent), info( info ), url(url)
        { modified = false; }

    CommentsListViewItem(const KIPI::ImageInfo& info, const KURL& url, QListView *parent, QListViewItem* after)
        : QListViewItem(parent, after), info( info ), url(url)
        { modified = false; }

    void paintCell(QPainter *p, const QColorGroup &cg,
                   int column, int width, int align) {

        if (column != 2) {
            QListViewItem::paintCell(p, cg, column, width, align);
            return;
        }

        QFont f(listView()->font());
        int size = f.pixelSize();
        if (size <= 0){
            size = f.pointSize();
            f.setPointSize(size - 1);
        }
        else{
            f.setPixelSize(size - 1);
        }
        f.setBold(true);
        f.setItalic(true);

        p->save();
        p->setFont(f);
        QListViewItem::paintCell(p, cg, column, width, align);
        p->restore();
    }

    bool modified;
    KIPI::ImageInfo info;
    KURL url;
};

// --------------------------------------------------------------------

class CListView : public QListView
{
public:

    CListView(QWidget *parent) : QListView(parent)
        {}

    void pressKey(QKeyEvent *e) {
        keyPressEvent(e);
    }

};

// --------------------------------------------------------------------

class CLineEdit : public QLineEdit
{
public:

    CLineEdit(QWidget *parent, CListView *keyReceiver)
        : QLineEdit(parent), m_keyReceiver(keyReceiver)
        {}

protected:

    void keyPressEvent(QKeyEvent *e) {
        switch(e->key()) {
        case(Qt::Key_Up):
        case(Qt::Key_Down):
        case(Qt::Key_Next):
        case(Qt::Key_Prior): {
            m_keyReceiver->pressKey(e);
            break;
        }
        default: {
            QLineEdit::keyPressEvent(e);
        }
        }

    }

    CListView *m_keyReceiver;

};

// --------------------------------------------------------------------

CommentsEditor::CommentsEditor( KIPI::Interface* interface, KIPI::ImageCollection images, QWidget *parent )
    : KDialogBase(Plain, i18n("Edit Comments"), Help|User1|Ok|Cancel, Ok,
                  parent, 0, true, true, i18n("&About") )

{
    setHelp("commentseditor", "kipi-plugins");
    m_images    = images;
    m_interface = interface;

    connect(this, SIGNAL(okClicked()),
            this, SLOT(slotOkClicked()));

    QVBoxLayout *topLayout = new QVBoxLayout( plainPage(),
                                              0, spacingHint());

    QLabel *topLabel = new QLabel( plainPage() );
    topLabel->setText( i18n( "Edit '%1' Album Comments").
                       arg(m_images.name()));
    topLayout->addWidget( topLabel  );

    // --------------------------------------------------------

    QFrame *topLine = new QFrame( plainPage() );
    topLine->setFrameShape( QFrame::HLine );
    topLine->setFrameShadow( QFrame::Sunken );
    topLayout->addWidget( topLine );

    // --------------------------------------------------------

    m_listView = new CListView(plainPage());
    topLayout->addWidget(m_listView);

    m_listView->addColumn(i18n("Preview"));
    m_listView->addColumn(i18n("Name"));
    m_listView->addColumn(i18n("Comments"));

    m_listView->setAllColumnsShowFocus(true);
    m_listView->setResizeMode(QListView::LastColumn);
    m_listView->setSorting(-1);

    // --------------------------------------------------------

    QVGroupBox *vbox = new QVGroupBox(i18n("Change Comments"),
                                      plainPage());
    m_edit = new CLineEdit(vbox, m_listView);

    topLayout->addWidget(vbox);

    // setup connections ----------------------------------------

    connect(m_listView, SIGNAL(selectionChanged()),
            this, SLOT(slotSelectionChanged()));
    connect(m_edit, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotCommentChanged(const QString&)));
    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotAboutClicked()));

    // ----------------------------------------------------------

    setInitialSize(configDialogSize("CommentsEditor Settings"));

    loadItems();
}

CommentsEditor::~CommentsEditor()
{
}

void CommentsEditor::loadItems()
{
    CommentsListViewItem *prevItem = 0;
    CommentsListViewItem *viewItem = 0;

    KURL::List urlList = m_images.images();

    for( KURL::List::Iterator it = urlList.begin(); it != urlList.end(); ++it ) {
        KIPI::ImageInfo info = m_interface->info( *it );
        if (!prevItem)
            viewItem = new CommentsListViewItem( info, *it, m_listView );
        else
            viewItem = new CommentsListViewItem( info, *it, m_listView, prevItem );


        viewItem->setText(1, info.title());
        viewItem->setText(2, info.description());
        viewItem->setRenameEnabled(2, true);

        prevItem = viewItem;
    }

    KIO::PreviewJob* thumbJob = KIO::filePreview( urlList, 64);
    connect(thumbJob, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
            SLOT(slotGotPreview(const KFileItem*, const QPixmap&)));
}

void CommentsEditor::slotGotPreview(const KFileItem* url,
                                    const QPixmap &pixmap)
{
    QPixmap pix(70,70);
    int xoffset =  pix.width()/2 - pixmap.width()/2;
    int yoffset =  pix.height()/2 - pixmap.height()/2;

    QPainter p(&pix);
    p.fillRect(0, 0, pix.width(), pix.height(),
               QBrush(m_listView->colorGroup().base()));
    p.drawPixmap(xoffset, yoffset, pixmap);
    p.end();


    QListViewItemIterator it(m_listView);
    while ( it.current() ) {
        CommentsListViewItem *viewItem =
            (CommentsListViewItem*)it.current();
        if ( viewItem->url == url->url() )
            viewItem->setPixmap(0,pix);
        ++it;
    }
}

void CommentsEditor::slotSelectionChanged()
{
    QListViewItem *item = m_listView->selectedItem();
    if (!item) return;

    m_edit->setText(item->text(2));
    m_edit->setFocus();
}

void CommentsEditor::slotCommentChanged(const QString& newComment)
{
    QListViewItem *item = m_listView->selectedItem();
    if (!item) return;
    CommentsListViewItem *viewItem = (CommentsListViewItem*)item;
    viewItem->setText(2, newComment);
    viewItem->modified = true;
}

void CommentsEditor::slotOkClicked()
{
    bool changed = false;

    QListViewItemIterator it(m_listView);
    while ( it.current() ) {
        CommentsListViewItem *viewItem =
            (CommentsListViewItem*)it.current();
        if (viewItem->modified) {
            viewItem->info.setDescription( viewItem->text(2) );
            changed = true;
        }
        ++it;
    }

    saveDialogSize("CommentsEditor Settings");
}

void CommentsEditor::slotAboutClicked()
{
    KMessageBox::about(this, i18n("A KIPI plugin for comments images edition\n\n"
                                  "Author: Renchi Raju\n\n"
                                  "Email: renchi@pooh.tam.uiuc.edu"),
                                  i18n("About CommentsEditor"));
}

}

#include "commentseditor.moc"
