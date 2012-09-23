/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-08-04
 * Description : Integration of the Photivo RAW-Processor.
 *
 * Copyright (C) 2012 by Dominic Lyons <domlyons at googlemail dot com>
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

#include "piwidget.h"
#include "piwidget.moc"

// Qt includes

#include <qboxlayout.h>
#include <qlabel.h>
#include <qlistwidget.h>

// KDE includes

#include <kdialog.h>
#include <klocale.h>

// local includes

#include "piimglist.h"
#include "xmpidmap.h"
#include "xmpinfo.h"

namespace KIPIPhotivoIntegrationPlugin
{

// pImpl //////////////////////////////////////////////////////////////////////

class PIWidget::Private
{
public:

    Private()
        : photivoLink(0),
          imagesList(0),
          info(0)
    {
    }

    QLabel*      photivoLink;
    PIImgList*   imagesList;
    QListWidget* info;
};

// public /////////////////////////////////////////////////////////////////////

PIWidget::PIWidget(QWidget* const parent /* = 0 */)
    : QWidget(parent),
      d(new Private)
{
    // list of currently selected images
    d->imagesList = new PIImgList(this);
    d->imagesList->loadImagesFromCurrentSelection();

    // group all information widgets 
    QWidget* infoBox = new QWidget(this);

    // link to Photivo homepage
    d->photivoLink = new QLabel(infoBox);
    d->photivoLink->setWhatsThis(i18n("This is a clickable link to open the Photivo home page in a web browser"));
    d->photivoLink->setText(QString("<h3><a href='http://photivo.org'>photivo.org</a></h3>"));
    d->photivoLink->setOpenExternalLinks(true);
    d->photivoLink->setFocusPolicy(Qt::NoFocus);

    // info about selected image
    d->info = new QListWidget;

    // vertical layout: photivo link abouve, info below
    QVBoxLayout* infoBoxLayout = new QVBoxLayout(infoBox);
    infoBoxLayout->addWidget(d->photivoLink);
    infoBoxLayout->addWidget(d->info);
    infoBoxLayout->addStretch(10);
    infoBoxLayout->setAlignment(d->photivoLink, Qt::AlignTop);
    infoBoxLayout->setSpacing(KDialog::spacingHint());
    infoBoxLayout->setMargin(KDialog::spacingHint());

    // horizontal layout: image list on the left side; link and info on the right
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(d->imagesList);
    mainLayout->addWidget(infoBox);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);

    // update image info after selected image has changed
    connect(d->imagesList->listView(), SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(slotCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
}

PIWidget::~PIWidget()
{
    delete d;
}

// ----------------------------------------------------------------------------

PIImgList* PIWidget::imagesList() const
{
    return d->imagesList;
}

// public Q_SLOTS /////////////////////////////////////////////////////////////

void PIWidget::slotCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* /*previous*/)
{
    // Warning: 'previous' could be nullptr!

    //TODO: Beutify code and output style & make output more usable
    //TODO: i18n

    // preparation
    QStringList infoList;
    QString     empty;
    XmpInfo     xmpInfo;
    XmpIDMap    &idmap  = XmpIDMap::getMap();

    // get url of selected file
    PIImgListViewItem* picur = static_cast<PIImgListViewItem*>(current);
    const QString      file  = picur->url().toLocalFile();//KPMetadata seems to be picky (doesn't accept file://)

    // read xmpMM
    const XmpMM   mm           = xmpInfo.getXmpMM(file);
    const QString pureOrigID   = mm.pureID(mm.originalDocumentID);//remove prefix

    // simple data
    infoList.append("--- " + file + " ---");
    infoList.append(empty);
    infoList.append("isDerivate():   "       + xmpInfo.isDerivate(file));
    infoList.append(empty);
    infoList.append("originalDocumentID:   " + mm.originalDocumentID);
    infoList.append("documentID:   "         + mm.documentID);
    infoList.append("instanceID:   "         + mm.instanceID);
    infoList.append(empty);

    // history
    int hsize = static_cast<int>(mm.history.size());
    for (int h = 0; h < hsize; h++)
    {
        infoList.append(QString("history %1: action:   ").arg(h)     + mm.history[h].action);
        infoList.append(QString("history %1: instanceID:   ").arg(h) + mm.history[h].instanceID);
        infoList.append(QString("history %1: when:   ").arg(h)       + mm.history[h].when);
    }
    infoList.append(empty);

// TODO: enable when XmpMM::loadDerivedFrom() is implemented
//         // derived from
//         int dsize = static_cast<int>(mm.derivedFrom.size());
//         for (int d = 0; d < dsize; d++)
//         {
//             infoList.append(QString("    derivedFrom %1: documentID: ").arg(d) + mm.derivedFrom[d].documentID);
//             infoList.append(QString("    derivedFrom %1: instanceID: ").arg(d) + mm.derivedFrom[d].instanceID);
//         }

    // map originalDocumentID to file
    if (!pureOrigID.isEmpty())
    {
        infoList.append("Origin file:   " + idmap.value("xmp.did/" + pureOrigID));
    }

    // update the widget
    d->info->clear();
    d->info->addItems(infoList);
}

// ----------------------------------------------------------------------------

} // namespace KIPIPhotivoIntegrationPlugin
