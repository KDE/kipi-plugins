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

#include <klocale.h>
#include <kconfig.h>
#include <kio/previewjob.h>
#include <kio/renamedlg.h>
#include <kdebug.h>

#include <qlistview.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qtimer.h>
#include <qprogressdialog.h>
#include <qgroupbox.h>

#include <libkipi/interface.h>
#include <libkipi/imageinfo.h>
#include <libkipi/imagedialog.h>

extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
}

#include "batchprocessimagesitem.h"
#include "renameimageswidget.h"

namespace KIPIBatchProcessImagesPlugin
{

RenameImagesWidget::RenameImagesWidget(QWidget *parent,
                                       KIPI::Interface* interface,
                                       const KURL::List& urlList)
    : RenameImagesBase(parent),
      m_interface(interface),
      m_urlList(urlList)
{
    m_listView->addColumn(i18n("Album"));
    m_listView->addColumn(i18n("Source Image"));
    m_listView->addColumn(i18n("Target Image"));
    m_listView->addColumn(i18n("Result"));
    m_listView->setResizeMode(QListView::LastColumn);
    m_listView->setSelectionMode(QListView::Single);
    m_listView->setAllColumnsShowFocus ( true );

    m_removeButton->setEnabled(false);
    
    readSettings();
    
    connect(m_listView, SIGNAL(doubleClicked(QListViewItem*)),
            SLOT(slotListViewDoubleClicked(QListViewItem*)));
    connect(m_listView, SIGNAL(selectionChanged(QListViewItem*)),
            SLOT(slotImageSelected(QListViewItem*)));

    connect(m_prefixEdit, SIGNAL(textChanged(const QString&)),
            SLOT(slotOptionsChanged()));
    connect(m_seqSpin, SIGNAL(valueChanged(int)),
            SLOT(slotOptionsChanged()));
    connect(m_addFileNameCheck, SIGNAL(toggled(bool)),
            SLOT(slotOptionsChanged()));
    connect(m_addFileDateCheck, SIGNAL(toggled(bool)),
            SLOT(slotOptionsChanged()));
    connect(m_formatDateCheck, SIGNAL(toggled(bool)),
            SLOT(slotOptionsChanged()));
    connect(m_formatDateEdit, SIGNAL(textChanged(const QString&)),
            SLOT(slotOptionsChanged()));

    connect(m_sortCombo, SIGNAL(activated(int)),
            SLOT(slotOptionsChanged()));
    connect(m_reverseSortCheck, SIGNAL(toggled(bool)),
            SLOT(slotOptionsChanged()));

    connect(m_addButton, SIGNAL(clicked()),
            SLOT(slotAddImages()));
    connect(m_removeButton, SIGNAL(clicked()),
            SLOT(slotRemoveImage()));
    
    m_timer = new QTimer(this);
    m_progress = new QProgressDialog(this, 0, true);
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
    m_addFileDateCheck->setChecked(config.readBoolEntry("AddImageFileDate", false));
    m_formatDateCheck->setChecked(config.readBoolEntry("FormatDate", false));
    m_formatDateEdit->setText(config.readEntry("FormatDateString", "%Y-%m-%d"));

    m_sortCombo->setCurrentItem((enum SortOrder)(config.readNumEntry("SortMethod", 0))); 
    m_reverseSortCheck->setChecked(config.readBoolEntry("ReverseOrder", false));

    slotOptionsChanged();
}

void RenameImagesWidget::saveSettings()
{
    KConfig config("kipirc");
    config.setGroup("RenameImages Settings");

    config.writeEntry("PrefixString", m_prefixEdit->text());
    config.writeEntry("FirstRenameValue", m_seqSpin->value());

    config.writeEntry("AddOriginalFileName", m_addFileNameCheck->isChecked());
    config.writeEntry("AddImageFileDate", m_addFileDateCheck->isChecked());
    config.writeEntry("FormatDate", m_formatDateCheck->isChecked());
    config.writeEntry("FormatDateString", m_formatDateEdit->text());

    config.writeEntry("SortMethod", m_sortCombo->currentItem());
    config.writeEntry("ReverseOrder", m_reverseSortCheck->isChecked());
    config.sync();
}

void RenameImagesWidget::slotOptionsChanged()
{
    m_formatDateCheck->setEnabled(m_addFileDateCheck->isChecked());
    m_formatDateEdit->setEnabled(m_formatDateCheck->isEnabled() &&
                                 m_formatDateCheck->isChecked());

    updateListing();
}

void RenameImagesWidget::slotListViewDoubleClicked(QListViewItem*)
{
    // TODO: Implement    
}

void RenameImagesWidget::slotImageSelected(QListViewItem* item)
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

void RenameImagesWidget::updateListing()
{
    for (QListViewItem* it = m_listView->firstChild(); it;
         it = it->nextSibling())
    {
        BatchProcessImagesItem* item = static_cast<BatchProcessImagesItem*>(it);

        switch (m_sortCombo->currentItem())
        {
        case(BYNAME):
        {
            item->setKey(item->text(1),
                         m_reverseSortCheck->isChecked());
            break;
        }
        case(BYSIZE):
        {
            QFileInfo fi(item->pathSrc());
            item->setKey(QString::number(fi.size()),
                         m_reverseSortCheck->isChecked());
            break;
        }
        case(BYDATE):
        {
            KURL url(item->pathSrc());
            KIPI::ImageInfo info = m_interface->info(url);
            item->setKey(info.time().toString(Qt::ISODate),
                         m_reverseSortCheck->isChecked());
            break;
        }
        }
    };
    
    m_listView->sort();

    int pos = 0;
    for (QListViewItem* it = m_listView->firstChild(); it;
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
    
    QString newName = m_prefixEdit->text();
    
    if (m_addFileNameCheck->isChecked())
    {
        newName += fi.baseName();
        newName += "_";
    }

    if (m_addFileDateCheck->isChecked())
    {
        QString format = m_formatDateEdit->text();
        format = format.simplifyWhiteSpace();
        format.replace("%%","%");
        format.replace("%s","");
        format.replace("/", "");

        time_t time = info.time().toTime_t();
        struct tm* time_tm = ::localtime(&time);
        char s[100];
        ::strftime(s, 100, QFile::encodeName(format), time_tm);

        newName += QString::fromLatin1(s);
        newName += "_";
    }

    int count = m_listView->childCount();
    int numDigits = 1;
    while (count > 0)
    {
        numDigits++;
        count = count / 10;
    }

    QString format;
    format.sprintf("0%dd", numDigits);
    format = "%" + format;

    QString seq;
    seq.sprintf(format.latin1(), itemPosition + m_seqSpin->value());
    newName += seq;

    newName += QString::fromLatin1(".") + fi.extension();
    
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
    QListViewItem* it = m_listView->selectedItem();
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

}

#include "renameimageswidget.moc"
