/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2003-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "renameimageswidget.moc"

// C++ includes

#include <cstdio>
#include <ctime>

// Qt includes

#include <QMenu>
#include <QAction>
#include <QGroupBox>
#include <QCheckBox>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QRegExp>
#include <QSpinBox>
#include <QTimer>
#include <QToolTip>
#include <QPointer>

// KDE includes

#include <kconfig.h>
#include <kdebug.h>
#include <kio/renamedialog.h>
#include <klocale.h>
#include <kde_file.h>
#include <kprogressdialog.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "batchprocessimagesitem.h"
#include "ui_renameimagesbase.h"
#include "kpimageinfo.h"
#include "kpimagedialog.h"
#include "kpmetadata.h"

namespace KIPIBatchProcessImagesPlugin
{

RenameImagesWidget::RenameImagesWidget(QWidget* parent, KIPI::Interface* interface,
                                       const KUrl::List& urlList)
    : QWidget(parent),
      m_interface(interface),
      m_urlList(urlList),
      m_progress(0),
      m_overwriteAll(false),
      m_autoSkip(false),
      ui(new Ui::RenameImagesBase)
{
    ui->setupUi(this);
    readSettings();

    QMenu* sortMenu = new QMenu(this);
    m_byNameAction  = sortMenu->addAction(i18n("Sort by Name"));
    m_bySizeAction  = sortMenu->addAction(i18n("Sort by Size"));
    m_byDateAction  = sortMenu->addAction(i18n("Sort by Date"));
    ui->m_sortButton->setMenu(sortMenu);

    ui->m_useExtraSymbolsCheck->setToolTip(
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

    connect(ui->m_listView, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(slotListViewDoubleClicked(QTreeWidgetItem*)));

    connect(ui->m_listView, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(slotImageSelected(QTreeWidgetItem*)));

    connect(ui->m_prefixEdit, SIGNAL(textChanged(QString)),
            this, SLOT(slotOptionsChanged()));

    connect(ui->m_seqSpin, SIGNAL(valueChanged(int)),
            this, SLOT(slotOptionsChanged()));

    connect(ui->m_addFileNameCheck, SIGNAL(toggled(bool)),
            this, SLOT(slotOptionsChanged()));

    connect(ui->m_useExtraSymbolsCheck, SIGNAL(toggled(bool)),
            this, SLOT(slotOptionsChanged()));

    connect(ui->m_addFileDateCheck, SIGNAL(toggled(bool)),
            this, SLOT(slotOptionsChanged()));

    connect(ui->m_formatDateCheck, SIGNAL(toggled(bool)),
            this, SLOT(slotOptionsChanged()));

    connect(ui->m_formatDateEdit, SIGNAL(textChanged(QString)),
            this, SLOT(slotOptionsChanged()));

    connect(ui->m_addButton, SIGNAL(clicked()),
            this, SLOT(slotAddImages()));

    connect(ui->m_removeButton, SIGNAL(clicked()),
            this, SLOT(slotRemoveImage()));

    connect(sortMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(sortList(QAction*)));

    connect(ui->m_reverseList, SIGNAL(clicked()),
            this, SLOT(reverseList()));

    connect(ui->m_moveUp, SIGNAL(clicked()),
            this, SLOT(moveCurrentItemUp()));

    connect(ui->m_moveDown, SIGNAL(clicked()),
            this, SLOT(moveCurrentItemDown()));

    m_timer    = new QTimer(this);

    connect(m_timer, SIGNAL(timeout()),
            this, SLOT(slotNext()));

    connect(m_interface, SIGNAL(gotThumbnail(KUrl,QPixmap)),
            this, SLOT(slotGotPreview(KUrl,QPixmap)));

    kDebug() << m_urlList;
    for (KUrl::List::iterator it = m_urlList.begin();
            it != m_urlList.end(); ++it)
    {
        new BatchProcessImagesItem(ui->m_listView,
                                   (*it).path().section('/', 0, -1),
                                   (*it).fileName(),
                                   QString(),
                                   QString());
    }

    updateListing();
}

RenameImagesWidget::~RenameImagesWidget()
{
    delete m_timer;

    saveSettings();

    delete ui;
}

void RenameImagesWidget::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("RenameImages Settings");

    ui->m_prefixEdit->setText(group.readEntry("PrefixString", ""));
    ui->m_seqSpin->setValue(group.readEntry("FirstRenameValue", 1));
    ui->m_addFileNameCheck->setChecked(group.readEntry("AddOriginalFileName", false));
    ui->m_useExtraSymbolsCheck->setChecked(group.readEntry("UseExtraSymbolsCheck", false));
    ui->m_addFileDateCheck->setChecked(group.readEntry("AddImageFileDate", false));
    ui->m_formatDateCheck->setChecked(group.readEntry("FormatDate", false));
    ui->m_formatDateEdit->setText(group.readEntry("FormatDateString", "%Y-%m-%d"));

    slotOptionsChanged();
}

void RenameImagesWidget::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("RenameImages Settings");

    group.writeEntry("PrefixString", ui->m_prefixEdit->text());
    group.writeEntry("FirstRenameValue", ui->m_seqSpin->value());
    group.writeEntry("AddOriginalFileName", ui->m_addFileNameCheck->isChecked());
    group.writeEntry("UseExtraSymbolsCheck", ui->m_useExtraSymbolsCheck->isChecked());
    group.writeEntry("AddImageFileDate", ui->m_addFileDateCheck->isChecked());
    group.writeEntry("FormatDate", ui->m_formatDateCheck->isChecked());
    group.writeEntry("FormatDateString", ui->m_formatDateEdit->text());

    config.sync();
}

void RenameImagesWidget::slotOptionsChanged()
{
    ui->m_formatDateCheck->setEnabled(ui->m_addFileDateCheck->isChecked());
    ui->m_useExtraSymbolsCheck->setEnabled(ui->m_addFileDateCheck->isChecked());
    ui->m_formatDateEdit->setEnabled(ui->m_formatDateCheck->isEnabled() &&
                                 ui->m_formatDateCheck->isChecked());

    updateListing();
}

void RenameImagesWidget::slotListViewDoubleClicked(QTreeWidgetItem*)
{
    // TODO: Implement
}

void RenameImagesWidget::slotImageSelected(QTreeWidgetItem* item)
{
    if (!item)
    {
        ui->m_removeButton->setEnabled(false);
        return;
    }

    ui->m_removeButton->setEnabled(true);
    ui->m_pixLabel->clear();

    BatchProcessImagesItem* it = static_cast<BatchProcessImagesItem*>(item);
    m_interface->thumbnail(KUrl(it->pathSrc()), ui->m_pixLabel->height());
}

void RenameImagesWidget::sortList(QAction* action)
{
    QTreeWidgetItemIterator it(ui->m_listView->topLevelItem(0));
    for (; *it; ++it)
    {
        BatchProcessImagesItem* item = static_cast<BatchProcessImagesItem*>(*it);

        if (action == m_byNameAction)
        {
            item->changeSortKey(item->nameSrc());
        }
        else if (action == m_bySizeAction)
        {
            QFileInfo fi(item->pathSrc());
            item->changeSortKey(QString::number(fi.size()));
        }
        else if (action == m_byDateAction)
        {
            KUrl url(item->pathSrc());
            KIPIPlugins::KPImageInfo info(url);
            item->changeSortKey(info.date().toString(Qt::ISODate));
        }
    }

    ui->m_listView->sortByColumn(BatchProcessImagesItem::columnOfSortKey(), Qt::AscendingOrder);

    updateListing();
}


void RenameImagesWidget::reverseList()
{
    if (ui->m_listView->topLevelItemCount() < 2) return;

    QList<QTreeWidgetItem*> lst;
    while (ui->m_listView->topLevelItemCount() > 0)
    {
        lst.prepend(ui->m_listView->takeTopLevelItem(0));
    }
    ui->m_listView->addTopLevelItems(lst);

    updateListing();
}


void RenameImagesWidget::moveCurrentItemUp()
{
    QTreeWidgetItem* currentItem = ui->m_listView->currentItem();
    if (!currentItem) return;

    int index = ui->m_listView->indexOfTopLevelItem(currentItem);
    if (index == 0)
    {
        return;
    }

    ui->m_listView->takeTopLevelItem(index);
    ui->m_listView->insertTopLevelItem(index - 1, currentItem);
    ui->m_listView->setCurrentItem(currentItem);

    updateListing();
}

void RenameImagesWidget::moveCurrentItemDown()
{
    QTreeWidgetItem* currentItem = ui->m_listView->currentItem();
    if (!currentItem) return;

    int index = ui->m_listView->indexOfTopLevelItem(currentItem);
    if (index == ui->m_listView->topLevelItemCount() - 1)
    {
        return;
    }

    ui->m_listView->takeTopLevelItem(index);
    ui->m_listView->insertTopLevelItem(index + 1, currentItem);
    ui->m_listView->setCurrentItem(currentItem);

    updateListing();
}

void RenameImagesWidget::updateListing()
{
    int pos = 0;
    QTreeWidgetItemIterator it(ui->m_listView);
    for (; *it; ++it)
    {
        BatchProcessImagesItem* item = static_cast<BatchProcessImagesItem*>(*it);
        item->changeNameDest(oldToNewName(item, pos));
        item->changeResult(QString());
        item->changeError(QString());
        item->changeOutputMess(QString());
        pos++;
    }
}

QString RenameImagesWidget::oldToNewName(BatchProcessImagesItem* item, int itemPosition)
{
    KUrl url;
    url.setPath(item->pathSrc());

    QFileInfo fi(item->pathSrc());

    KIPIPlugins::KPImageInfo info(url);

    bool useExtraSymbols = ui->m_addFileDateCheck->isChecked() &&
                           ui->m_useExtraSymbolsCheck->isChecked();

    QString newName = ui->m_prefixEdit->text();

    if (ui->m_addFileNameCheck->isChecked())
    {
        newName += fi.baseName();
        newName += '_';
    }

    int seqNumber = itemPosition + ui->m_seqSpin->value();
    if (ui->m_addFileDateCheck->isChecked())
    {
        QString format = ui->m_formatDateEdit->text();
        format         = format.simplified();
        if (useExtraSymbols)
        {
            QRegExp rxI("\\[i(:(\\d+))?\\]");
            QRegExp rxN("\\[([anbBeEp])([-+]?)(:(\\d*|-\\d+)\\.\\.(\\d*|-\\d+))?\\]");

            for (int watchDog = 0; watchDog < 100; ++watchDog)
            {
                QString to;
                int j, i = rxI.indexIn(format);
                if (i != -1)
                {
                    j              = rxI.matchedLength();
                    QString digits = rxI.cap(2);
                    int k          = digits.isEmpty() ? 0 : digits.toInt();
                    if (k < 2)
                    {
                        to = QString::number(seqNumber);
                    }
                    else
                    {
                        QString fmt;
                        fmt.sprintf("0%dd", (k > 10 ? 10 : k));
                        fmt = '%' + fmt;
                        to.sprintf(fmt.toLatin1(), seqNumber);
                    }
                }
                else
                {
                    if ((i = rxN.indexIn(format)) == -1)
                    {
                        break;
                    }
                    j            = rxN.matchedLength();
                    QString from = rxN.cap(1);
                    from = (from == "e") ? fi.suffix() :
                           (from == "E") ? fi.completeSuffix() :
                           (from == "b") ? fi.baseName() :
                           (from == "B") ? fi.completeBaseName() :
                           (from == "n") ? fi.fileName() :
                           (from == "a") ? fi.absoluteDir().dirName() :
                           (from == "p") ? fi.absolutePath() :
                           "";
                    int len            = from.length();
                    QString start      = rxN.cap(4);
                    QString end        = rxN.cap(5);
                    int k              = start.isEmpty() ? 1 : start.toInt();
                    int l              = end.isEmpty() ? len : end.toInt();
                    k                  = (k < -len) ? 0 : (k < 0) ? (len + k) : (k > 0) ? (k - 1) : 0;
                    l                  = (l < -len) ? -1 : (l < 0) ? (len + l) : (l > 0) ? (l - 1) : 0;
                    to                 = l < k ? "" : from.mid(k, l - k + 1);
                    QString changeCase = rxN.cap(2);
                    if (!changeCase.isEmpty())
                    {
                        to = (changeCase == "+") ? to.toUpper() : to.toLower();
                    }
                }
                format.replace(i, j, to);
            }
        }
        format.replace("%%", "%");
        format.remove("%s");
        format.replace('/', '!');
        format.replace("%[", "% [");

        time_t time        = info.date().toTime_t();
        struct tm* time_tm = ::localtime(&time);
        char s[100];
        ::strftime(s, 100, QFile::encodeName(format), time_tm);

        newName += QString::fromLocal8Bit(s);
        if (!useExtraSymbols)
        {
            newName += '_';
        }
    }

    if (!useExtraSymbols)
    {
        int numDigits = 1;
        int count = ui->m_listView->topLevelItemCount();
        while (count > 0)
        {
            numDigits++;
            count = count / 10;
        }

        QString format;
        format.sprintf("0%dd", numDigits);
        format = '%' + format;

        QString seq;
        seq.sprintf(format.toLatin1(), seqNumber);
        newName += seq;

        newName += QString::fromLatin1(".") + fi.suffix();
    }

    return newName;
}

void RenameImagesWidget::slotGotPreview(const KUrl&, const QPixmap& pixmap)
{
    ui->m_pixLabel->setPixmap(pixmap);
}

void RenameImagesWidget::slotStart()
{
    m_timer->setSingleShot(true);
    m_timer->start(0);

    QTreeWidgetItem* item = ui->m_listView->topLevelItem(0);
    ui->m_listView->setCurrentItem(item);
    ui->m_listView->scrollToItem(item);


    m_progress = new KProgressDialog(this);
    m_progress->setAutoReset(true);
    m_progress->setAutoClose(true);
    m_progress->progressBar()->setMaximum(ui->m_listView->topLevelItemCount());
    m_progress->progressBar()->setValue(0);

    connect(m_progress, SIGNAL(cancelClicked()),
            this, SLOT(slotAbort()));


    m_overwriteAll = false;
    m_autoSkip     = false;
}

void RenameImagesWidget::slotAbort()
{
    m_timer->stop();
    m_progress->progressBar()->reset();
    m_progress->hide();
}

void RenameImagesWidget::slotNext()
{
    QTreeWidgetItem* it = ui->m_listView->selectedItems().first();
    if (!it)
    {
        slotAbort();
        return;
    }

    BatchProcessImagesItem* item = static_cast<BatchProcessImagesItem*>(it);
    KUrl src;
    src.setPath(item->pathSrc());
    KUrl dst = src.upUrl();
    dst.addPath(item->text(2));

    bool skip      = false;
    bool overwrite = false;

    if (!m_overwriteAll)
    {
        KDE_struct_stat info;
        while (KDE_stat(QFile::encodeName(dst.toLocalFile()), &info) == 0)
        {
            if (m_autoSkip)
            {
                skip = true;
                break;
            }

            QPointer<KIO::RenameDialog> dlg = new KIO::RenameDialog(this, i18n("Rename File"),
                                              src.path(), dst.path(),
                                              KIO::RenameDialog_Mode(KIO::M_MULTI | KIO::M_OVERWRITE | KIO::M_SKIP));
            int result = dlg->exec();
            dst        = dlg->newDestUrl();

            delete dlg;

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
        item->changeResult(i18nc("batch process result", "Skipped"));
    }
    else
    {
        // Get the src info
        KIPIPlugins::KPImageInfo srcInfo(src);

        if (KDE_rename(QFile::encodeName(src.toLocalFile()),
                       QFile::encodeName(dst.toLocalFile())) == 0)
        {
            // Rename XMP sidecar file
            KIPIPlugins::KPMetadata::moveSidecar(src, dst);

            srcInfo.setName(dst.fileName());

            item->changeResult(i18nc("batch process result", "OK"));
        }
        else
        {
            item->changeResult(i18nc("batch process result", "Failed"));
        }
    }

    m_progress->progressBar()->setValue(m_progress->progressBar()->value() + 1);

    it = ui->m_listView->itemBelow(it);
    if (it)
    {
        ui->m_listView->setCurrentItem(it);
        ui->m_listView->scrollToItem(it);
        m_timer->setSingleShot(true);
        m_timer->start(0);
    }
}

void RenameImagesWidget::slotAddImages()
{
    KUrl::List urls = KIPIPlugins::KPImageDialog::getImageUrls(this);

    for (KUrl::List::iterator it = urls.begin(); it != urls.end(); ++it)
    {
        if (m_urlList.contains(*it))
            continue;

        new BatchProcessImagesItem(ui->m_listView,
                                   (*it).path().section('/', 0, -1),
                                   (*it).fileName(),
                                   QString(),
                                   QString());
        m_urlList.append(*it);
    }

    updateListing();
}

void RenameImagesWidget::slotRemoveImage()
{
    if (ui->m_listView->selectedItems().isEmpty())
        return;

    BatchProcessImagesItem* item = static_cast<BatchProcessImagesItem*>(ui->m_listView->selectedItems().first());
    delete item;

    ui->m_pixLabel->clear();

    updateListing();
}

}  // namespace KIPIBatchProcessImagesPlugin
