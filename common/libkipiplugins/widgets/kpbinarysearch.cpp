/* ============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-01-05
 * Description : a widget to find missing binaries.
 *
 * Copyright (C) 2012-2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "kpbinarysearch.moc"

// Qt includes

#include <QLabel>
#include <QHeaderView>

// KDE includes

#include <kvbox.h>
#include <klocale.h>
#include <kdebug.h>

namespace KIPIPlugins
{

struct KPBinarySearch::Private
{
    Private()
    {
        downloadLabel = 0;
    }

    QVector<KPBinaryIface*>   binaryIfaces;
    QVector<QTreeWidgetItem*> items;
    QLabel*                   downloadLabel;
};

KPBinarySearch::KPBinarySearch(QWidget* const parent)
    : QTreeWidget(parent), d(new Private)
{
    setIconSize(QSize(KIconLoader::SizeSmall, KIconLoader::SizeSmall));
    setAlternatingRowColors(true);
    setSelectionMode(QAbstractItemView::NoSelection);
    setSortingEnabled(false);
    setAllColumnsShowFocus(true);
    setRootIsDecorated(false);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setColumnCount(5);
    setHeaderLabels(QStringList() << QString("")
                                  << i18n("Binary")
                                  << i18n("Version")
                                  << QString("")
                                  << QString(""));

    header()->setResizeMode(Status,  QHeaderView::ResizeToContents);
    header()->setResizeMode(Binary,  QHeaderView::Stretch);
    header()->setResizeMode(Version, QHeaderView::Stretch);
    header()->setResizeMode(Button,  QHeaderView::Stretch);
    header()->setResizeMode(Link,    QHeaderView::Stretch);

    d->downloadLabel = new QLabel(parentWidget());

    qobject_cast<QGridLayout*>(parentWidget()->layout())->addWidget(this, 0, 0);
}

KPBinarySearch::~KPBinarySearch()
{
    delete d;
}

void KPBinarySearch::addBinary(KPBinaryIface& binary)
{
    delete d->downloadLabel;

    binary.recheckDirectories();

    d->binaryIfaces.append(&binary);
    d->items.append(new QTreeWidgetItem());
    QTreeWidgetItem* item   = d->items[d->items.size() - 1];
    item->setIcon(Status, QIcon(SmallIcon("dialog-cancel")));
    item->setText(Binary, binary.baseName());
    item->setText(Version, binary.version());
    item->setToolTip(Status, i18n("Binary not found."));
    item->setToolTip(Version, i18n("Minimal version number required for this binary is %1", binary.minimalVersion()));
    insertTopLevelItem(d->binaryIfaces.size() - 1, item);
    QPushButton* findButton = new QPushButton(i18n("Find"));
    setItemWidget(item, Button, findButton);
    QLabel* downloadLabel   = new QLabel(i18n(" or <a href=\"%1\">download</a>", binary.url().url()));
    downloadLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    downloadLabel->setOpenExternalLinks(true);
    setItemWidget(item, Link, downloadLabel);

    // Starts a dialog to find the binary
    connect(findButton, SIGNAL(clicked(bool)),
            &binary, SLOT(slotNavigateAndCheck()));

    // Rechecks full validity when a binary is found and valid
    connect(&binary, SIGNAL(signalBinaryValid()),
            this, SLOT(slotAreBinariesFound()));

    // Scans (if no binary were found) a new directory where a binary was found
    connect(&binary, SIGNAL(signalSearchDirectoryAdded(QString)),
            this, SIGNAL(signalAddPossibleDirectory(QString)));

    connect(this, SIGNAL(signalAddPossibleDirectory(QString)),
            &binary, SLOT(slotAddPossibleSearchDirectory(QString)));

    // Force scan of a new directory
    connect(this, SIGNAL(signalAddDirectory(QString)),
            &binary, SLOT(slotAddSearchDirectory(QString)));

    d->downloadLabel    = new QLabel(i18n(
        "<qt><p><font color=\"red\"><b>Warning:</b> Some necessary binaries have not been found on "
        "your system. If you have these binaries installed, please click the 'Find' button to locate them on your "
        "system, otherwise please download and install them to proceed.</font></p></qt>"), parentWidget());

    QGridLayout* layout = qobject_cast<QGridLayout*>(parentWidget()->layout());
    layout->addWidget(d->downloadLabel, layout->rowCount(), 0);
    d->downloadLabel->setWordWrap(true);
    d->downloadLabel->setMargin(20);
    d->downloadLabel->hide();
}

void KPBinarySearch::addDirectory(const QString& dir)
{
    emit(signalAddPossibleDirectory(dir));
}

bool KPBinarySearch::allBinariesFound()
{
    bool ret = true;

    foreach(KPBinaryIface* const binary, d->binaryIfaces)
    {
        int index = d->binaryIfaces.indexOf(binary);

        if (binary->isValid())
        {
            if (!binary->developmentVersion())
            {
                d->items[index]->setIcon(Status, QIcon(SmallIcon("dialog-ok-apply")));
                d->items[index]->setToolTip(Status, QString());
            }
            else
            {
                d->items[index]->setIcon(Status, QIcon(SmallIcon("dialog-warning")));
                d->items[index]->setToolTip(Status, i18n("A development version has been detect. "
                                                         "There is no guarantee on the behavior of this binary."));
                d->downloadLabel->show();
            }

            d->items[index]->setText(Version, binary->version());
            qobject_cast<QPushButton*>(itemWidget(d->items[index], Button))->setText(i18n("Change"));
        }
        else
        {
            ret = false;
        }
    }

    if (ret)
    {
        d->downloadLabel->hide();
    }

    return ret;
}

void KPBinarySearch::slotAreBinariesFound()
{
    kDebug() << "new binary found!!";
    bool allFound = allBinariesFound();
    emit signalBinariesFound(allFound);
}

} // namespace KIPIPlugins
