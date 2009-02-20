//Added by qt3to4:
#include <QPixmap>
/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2003-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2005 by Owen Hirst <n8rider@sbcglobal.net>
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

// C Ansi includes.

extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
}

// C++ includes.

#include <cstdio>
#include <ctime>

// Qt includes.

#include <q3listview.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qtimer.h>
#include <q3progressdialog.h>
#include <q3groupbox.h>
#include <q3popupmenu.h>
#include <qregexp.h>
#include <qdir.h>
#include <qtooltip.h>

// KDE includes.

#include <klocale.h>
#include <kconfig.h>
#include <kio/previewjob.h>
#include <kio/renamedlg.h>
#include <kdebug.h>
#include <kdeversion.h>

// Libkipi includes.

#include <libkipi/interface.h>
#include <libkipi/imageinfo.h>
#include <libkipi/imagedialog.h>

// Local includes.

#include "batchprocessimagesitem.h"
#include "renameimageswidget.h"
#include "renameimageswidget.moc"

namespace KIPIBatchProcessImagesPlugin
{

RenameImagesWidget::RenameImagesWidget(QWidget *parent,
                                       KIPI::Interface* interface,
                                       const KURL::List& urlList)
    : RenameImagesBase(parent),
      m_interface(interface),
      m_urlList(urlList)
{
    m_listView->setSorting(-1);
#if KDE_IS_VERSION(3,4,0)
    // next can be done directly into designer but it seems not to compile
    // under kde < 3.4.0
    m_listView->setShadeSortColumn( FALSE );
#endif

    readSettings();

    Q3PopupMenu* sortMenu = new Q3PopupMenu(this);
    sortMenu->insertItem(i18n("Sort by Name"), BYNAME);
    sortMenu->insertItem(i18n("Sort by Size"), BYSIZE);
    sortMenu->insertItem(i18n("Sort by Date"), BYDATE);
    m_sortButton->setPopup(sortMenu);

    QToolTip::add(m_useExtraSymbolsCheck,
            "[e] - extension (small one - after last '.')\n"
            "[e-] - extension lower case\n"
            "[e+] extension upper case\n"
            "[i] - sequence number - no leading zeros\n"
            "[i:4] - sequence number in 4 digit with leading zeros format\n"
            "[n] - original file name\n"
            "[n+] - original file name upper case\n"
            "[n-] - original file name lower case\n"
            "[n:5..-2] - substring of original filename from char 5 to second from the end\n"
            "[n+:..5] - whole name (base + extension, characters from 1 to 5)\n"
            "[a] - album name\n"
            "[p+] - absolute path (uppercase)\n"
            "[B:4..-2] - base name (big one - all before last ',', from 4-th to one before last characters)\n"
            "[b-:-3..] - base name (small one - all before first '.', last 3 characters)");

    connect(m_listView, SIGNAL(doubleClicked(Q3ListViewItem*)),
            SLOT(slotListViewDoubleClicked(Q3ListViewItem*)));
    connect(m_listView, SIGNAL(selectionChanged(Q3ListViewItem*)),
            SLOT(slotImageSelected(Q3ListViewItem*)));

    connect(m_prefixEdit, SIGNAL(textChanged(const QString&)),
            SLOT(slotOptionsChanged()));
    connect(m_seqSpin, SIGNAL(valueChanged(int)),
            SLOT(slotOptionsChanged()));
    connect(m_addFileNameCheck, SIGNAL(toggled(bool)),
            SLOT(slotOptionsChanged()));
    connect(m_useExtraSymbolsCheck, SIGNAL(toggled(bool)),
            SLOT(slotOptionsChanged()));
    connect(m_addFileDateCheck, SIGNAL(toggled(bool)),
            SLOT(slotOptionsChanged()));
    connect(m_formatDateCheck, SIGNAL(toggled(bool)),
            SLOT(slotOptionsChanged()));
    connect(m_formatDateEdit, SIGNAL(textChanged(const QString&)),
            SLOT(slotOptionsChanged()));

    connect(m_addButton, SIGNAL(clicked()),
            SLOT(slotAddImages()));
    connect(m_removeButton, SIGNAL(clicked()),
            SLOT(slotRemoveImage()));

	connect(sortMenu, SIGNAL(activated(int)),
			SLOT(sortList(int)) );
	
	connect(m_reverseList, SIGNAL(clicked()),
			SLOT(reverseList()) );

    connect(m_moveUp, SIGNAL(clicked()),
            SLOT(moveCurrentItemUp()) );
    
    connect(m_moveDown, SIGNAL(clicked()),
            SLOT(moveCurrentItemDown()) );
    
    m_timer = new QTimer(this);
    m_progress = new Q3ProgressDialog(this, 0, true);
    connect(m_timer, SIGNAL(timeout()),
            SLOT(slotNext()));
    connect(m_progress, SIGNAL(canceled()),
            SLOT(slotAbort()));

    for (KURL::List::iterator it = m_urlList.begin();
         it != m_urlList.end(); ++it)
    {
        new BatchProcessImagesItem(m_listView,
                                   (*it).path().section('/', 0, -1),
                                   (*it).filename(),
                                   QString(),
                                   QString());
    }

    updateListing();
}

RenameImagesWidget::~RenameImagesWidget()
{
    delete m_timer;
    delete m_progress;
    
    saveSettings();
}

void RenameImagesWidget::readSettings()
{
    KConfig config("kipirc");
    config.setGroup("RenameImages Settings");

    m_prefixEdit->setText(config.readEntry("PrefixString", ""));
    m_seqSpin->setValue(config.readNumEntry("FirstRenameValue", 1));

    m_addFileNameCheck->setChecked(config.readBoolEntry("AddOriginalFileName", false));
    m_useExtraSymbolsCheck->setChecked(config.readBoolEntry("UseExtraSymbolsCheck", false));
    m_addFileDateCheck->setChecked(config.readBoolEntry("AddImageFileDate", false));
    m_formatDateCheck->setChecked(config.readBoolEntry("FormatDate", false));
    m_formatDateEdit->setText(config.readEntry("FormatDateString", "%Y-%m-%d"));

    slotOptionsChanged();
}

void RenameImagesWidget::saveSettings()
{
    KConfig config("kipirc");
    config.setGroup("RenameImages Settings");

    config.writeEntry("PrefixString", m_prefixEdit->text());
    config.writeEntry("FirstRenameValue", m_seqSpin->value());

    config.writeEntry("AddOriginalFileName", m_addFileNameCheck->isChecked());
    config.writeEntry("UseExtraSymbolsCheck", m_useExtraSymbolsCheck->isChecked());
    config.writeEntry("AddImageFileDate", m_addFileDateCheck->isChecked());
    config.writeEntry("FormatDate", m_formatDateCheck->isChecked());
    config.writeEntry("FormatDateString", m_formatDateEdit->text());

    config.sync();
}

void RenameImagesWidget::slotOptionsChanged()
{
    m_formatDateCheck->setEnabled(m_addFileDateCheck->isChecked());
    m_useExtraSymbolsCheck->setEnabled(m_addFileDateCheck->isChecked());
    m_formatDateEdit->setEnabled(m_formatDateCheck->isEnabled() &&
                                 m_formatDateCheck->isChecked());

    updateListing();
}

void RenameImagesWidget::slotListViewDoubleClicked(Q3ListViewItem*)
{
    // TODO: Implement    
}

void RenameImagesWidget::slotImageSelected(Q3ListViewItem* item)
{
    if (!item)
    {
        m_removeButton->setEnabled(false);
        return;
    }

    m_removeButton->setEnabled(true);
    m_pixLabel->clear();
    
    BatchProcessImagesItem* it = static_cast<BatchProcessImagesItem*>(item);
    KIO::PreviewJob* thumbJob = KIO::filePreview(KURL(it->pathSrc()),
                                                 m_pixLabel->height() );

    connect(thumbJob, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
            SLOT(slotGotPreview(const KFileItem*, const QPixmap&)));
}


void RenameImagesWidget::sortList(int intSortOrder)
{
    SortOrder sortOrder = static_cast<Qt::SortOrder>(intSortOrder);

    for (Q3ListViewItem* it = m_listView->firstChild(); it;
         it = it->nextSibling())
    {
        BatchProcessImagesItem* item = static_cast<BatchProcessImagesItem*>(it);

        switch (sortOrder)
        {
        case(BYNAME):
        {
            item->setKey(item->text(1), false);
            break;
        }
        case(BYSIZE):
        {
            QFileInfo fi(item->pathSrc());
            item->setKey(QString::number(fi.size()), false);
            break;
        }
        case(BYDATE):
        {
            KURL url(item->pathSrc());
            KIPI::ImageInfo info = m_interface->info(url);
            item->setKey(info.time().toString(Qt::ISODate), false);
            break;
        }
        }
    };
    
    // Update list order. We need to set the sorting column temporarily
    // otherwise sort() won't do anything
    m_listView->setSorting(1);
    m_listView->sort();
    m_listView->setSorting(-1);

    updateListing();
}


void RenameImagesWidget::reverseList()
{
    if (m_listView->childCount() < 2) return;

    Q3ListViewItem* lastItem = m_listView->lastItem();

    while (m_listView->firstChild() != lastItem) {
        m_listView->firstChild()->moveItem(lastItem);
    }

    updateListing();
}


void RenameImagesWidget::moveCurrentItemUp() {
    Q3ListViewItem* currentItem = m_listView->currentItem();
    if (!currentItem) return;

    for (Q3ListViewItem* previousItem = m_listView->firstChild(); previousItem;
         previousItem = previousItem->nextSibling()) 
    {
        if (previousItem->nextSibling() == currentItem) {
            previousItem->moveItem(currentItem);
            break;
        }
    }

    updateListing();
}


void RenameImagesWidget::moveCurrentItemDown() {
    Q3ListViewItem* currentItem = m_listView->currentItem();
    if (!currentItem) return;
    
    Q3ListViewItem* nextItem = currentItem->nextSibling();
    if (nextItem) {
        currentItem->moveItem(nextItem);
    }

    updateListing();
}


void RenameImagesWidget::updateListing()
{
    int pos = 0;
    for (Q3ListViewItem* it = m_listView->firstChild(); it;
         it = it->nextSibling())
    {
        BatchProcessImagesItem* item = static_cast<BatchProcessImagesItem*>(it);
        item->changeNameDest(oldToNewName(item, pos));
        item->changeResult(QString());
        item->changeError(QString());
        item->changeOutputMess(QString());
        pos++;
    }
}

QString RenameImagesWidget::oldToNewName(BatchProcessImagesItem* item,
                                         int itemPosition)
{
    KURL url;
    url.setPath(item->pathSrc());

    QFileInfo fi(item->pathSrc());

    KIPI::ImageInfo info = m_interface->info(url);
    
    bool useExtraSymbols = m_addFileDateCheck->isChecked() && 
        m_useExtraSymbolsCheck->isChecked();

    QString newName = m_prefixEdit->text();
    
    if (m_addFileNameCheck->isChecked())
    {
        newName += fi.baseName();
        newName += "_";
    }

    int seqNumber = itemPosition + m_seqSpin->value();
    if (m_addFileDateCheck->isChecked())
    {
        QString format = m_formatDateEdit->text();
        format = format.simplifyWhiteSpace();
        if (useExtraSymbols)
        {
            QRegExp rxI("\\[i(:(\\d+))?\\]");
            QRegExp rxN("\\[([anbBeEp])([-+]?)(:(\\d*|-\\d+)\\.\\.(\\d*|-\\d+))?\\]");

            for(int watchDog = 0; watchDog < 100; watchDog++)
            {
                QString to;
                int j, i = rxI.search(format);
                if (i != -1)
                {
                    j = rxI.matchedLength();
                    QString digits = rxI.cap(2);
                    int k = (!digits || !digits.length()) ? 0 : digits.toInt();
                    if (k < 2)
                    {
                        to = QString::number(seqNumber);
                    }
                    else
                    {
                        QString fmt;
                        fmt.sprintf("0%dd", (k > 10 ? 10 : k));
                        fmt = "%" + fmt;
                        to.sprintf(fmt.latin1(), seqNumber);
                    }
                }
                else
                {
                    if ((i = rxN.search(format)) == -1)
                    { 
                        break; 
                    }
                    j = rxN.matchedLength();
                    QString from = rxN.cap(1);
                    from = (from == "e") ? fi.extension(/*complete=*/FALSE) :
                           (from == "E") ? fi.extension(/*complete=*/TRUE) :
                           (from == "b") ? fi.baseName(/*complete=*/FALSE) :
                           (from == "B") ? fi.baseName(/*complete=*/TRUE) :
                           (from == "n") ? fi.fileName() :
                           (from == "a") ? fi.dir(/*absPath=*/TRUE).dirName() :
                           (from == "p") ? fi.dirPath(/*absPath=*/TRUE) :
                           "";
                    int len = from.length();
                    QString start = rxN.cap(4);
                    QString end = rxN.cap(5);
                    int k = (!start || !start.length()) ? 1 : start.toInt();
                    int l = (!end || !end.length()) ? len : end.toInt();
                    k = (k < -len) ? 0 : (k < 0) ? (len + k) : (k > 0) ? (k - 1) : 0;
                    l = (l < -len) ? -1 : (l < 0) ? (len + l) : (l > 0) ? (l - 1) : 0;
                    to = l < k ? "" : from.mid(k, l - k + 1);
                    QString changeCase = rxN.cap(2);
                    if (!!changeCase && changeCase.length())
                    {
                        to = (changeCase == "+") ? to.upper() : to.lower();
                    }
                }
                format.replace(i, j, to);
            }
        }
        format.replace("%%","%");
        format.replace("%s","");
        format.replace("/", "!");
        format.replace("%[","% [");

        time_t time = info.time().toTime_t();
        struct tm* time_tm = ::localtime(&time);
        char s[100];
        ::strftime(s, 100, QFile::encodeName(format), time_tm);

        newName += QString::fromLocal8Bit(s);
        if (!useExtraSymbols)
        {
            newName += "_";
        }
    }

    if (!useExtraSymbols)
    {
        int numDigits = 1;
        int count = m_listView->childCount();
        while (count > 0)
        {
            numDigits++;
            count = count / 10;
        }

        QString format;
        format.sprintf("0%dd", numDigits);
        format = "%" + format;

        QString seq;
        seq.sprintf(format.latin1(), seqNumber);
        newName += seq;

        newName += QString::fromLatin1(".") + fi.extension();
    }
    
    return newName;
}

void RenameImagesWidget::slotGotPreview(const KFileItem*, const QPixmap& pix)
{
    m_pixLabel->setPixmap(pix);    
}

void RenameImagesWidget::slotStart()
{
    m_timer->start(0, true);

    m_listView->setSelected(m_listView->firstChild(), true);
    m_listView->ensureItemVisible(m_listView->firstChild());

    m_progress->setTotalSteps(m_listView->childCount());
    m_progress->setProgress(0);
    m_progress->show();

    m_overwriteAll = false;
    m_autoSkip     = false;
}

void RenameImagesWidget::slotAbort()
{
    m_timer->stop();
    m_progress->reset();
    m_progress->hide();
}

void RenameImagesWidget::slotNext()
{
    Q3ListViewItem* it = m_listView->selectedItem();
    if (!it)
    {
        slotAbort();
        return;
    }

    BatchProcessImagesItem* item = static_cast<BatchProcessImagesItem*>(it);
    KURL src;
    src.setPath(item->pathSrc());
    KURL dst = src.upURL();
    dst.addPath(item->text(2));

    bool skip      = false;
    bool overwrite = false;
    
    if (!m_overwriteAll)
    {
        struct stat info;
        while (::stat(QFile::encodeName(dst.path()), &info) == 0)
        {
            if (m_autoSkip)
            {
                skip = true;
                break;
            }

            KIO::RenameDlg dlg(this, i18n("Rename File"), src.path(), dst.path(),
                               KIO::RenameDlg_Mode(KIO::M_MULTI |
                                                   KIO::M_OVERWRITE |
                                                   KIO::M_SKIP));
            int result = dlg.exec();
            dst        = dlg.newDestURL();

            switch (result)
            {
            case KIO::R_CANCEL:
            {
                slotAbort();
                return;
            }
            case KIO::R_SKIP:
            {
                skip = true;
                break;
            }
            case KIO::R_AUTO_SKIP:
            {
                m_autoSkip = true;
                skip       = true;
                break;
            }
            case KIO::R_OVERWRITE:
            {
                overwrite       = true;
                break;
            }
            case KIO::R_OVERWRITE_ALL:
            {
                m_overwriteAll = true;
                overwrite      = true;
                break;
            }
            default:
                break;
            }

            if (skip || overwrite)
                break;
        }
    }

    if (skip)
    {
        item->changeResult(i18n("Skipped"));
    }
    else
    {
        // Get the src info
        KIPI::ImageInfo srcInfo = m_interface->info(src);
        
        if (::rename(QFile::encodeName(src.path()),
                     QFile::encodeName(dst.path())) == 0)
        {
            srcInfo.setTitle(dst.filename());
            
            item->changeResult(i18n("OK"));
        }
        else
        {
            item->changeResult(i18n("Failed"));
        }
    }

    m_progress->setProgress(m_progress->progress() + 1);

    if (it->nextSibling())
    {
        m_listView->setSelected(it->nextSibling(), true);
        m_listView->ensureItemVisible(it->nextSibling());
        m_timer->start(0, true);
    }
}

void RenameImagesWidget::slotAddImages()
{
    KURL::List urls = KIPI::ImageDialog::getImageURLs(this, m_interface);

    for (KURL::List::iterator it = urls.begin(); it != urls.end(); ++it)
    {
        if (m_urlList.contains(*it))
            continue;

        new BatchProcessImagesItem(m_listView,
                                   (*it).path().section('/', 0, -1),
                                   (*it).filename(),
                                   QString(),
                                   QString());
        m_urlList.append(*it);
    }

    updateListing();
}

void RenameImagesWidget::slotRemoveImage()
{
    if (!m_listView->selectedItem())
        return;

    BatchProcessImagesItem* item =
        static_cast<BatchProcessImagesItem*>(m_listView->selectedItem());
    delete item;

    m_pixLabel->clear();
    
    updateListing();
}

}  // namespace KIPIBatchProcessImagesPlugin
