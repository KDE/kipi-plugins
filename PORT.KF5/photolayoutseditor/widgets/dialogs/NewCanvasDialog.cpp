/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-01
 * Description : a plugin to create photo layouts by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011-2012 by Łukasz Spas <lukasz dot spas at gmail dot com>
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

#include "NewCanvasDialog.moc"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QTableView>
#include <QLabel>
#include <QPrinter>
#include <QStackedLayout>
#include <QDebug>
#include <QDir>
#include <QMap>
#include <QButtonGroup>

#include <klocalizedstring.h>
#include <kstandarddirs.h>
#include <kpushbutton.h>
#include <kicon.h>

#include "CanvasSizeWidget.h"
#include "TemplatesView.h"
#include "TemplatesModel.h"
#include "CanvasSize.h"

#define PAPER_SIZE_ROLE 128

using namespace KIPIPhotoLayoutsEditor;

class NewCanvasDialog::Private
{
    Private() :
        stack(0),
        paperSize(0),
        templatesList(0),
        canvasSize(0),
        horizontalButton(0),
        verticalButton(0),
        orientationGroup(0)
    {
        QListWidgetItem* temp = new QListWidgetItem("Custom");
        temp->setData(PAPER_SIZE_ROLE, -1);
        paperSizes.append(temp);

        names.insert( QPrinter::A0, QPair<QString,QString>("A0", "a0"));
        names.insert( QPrinter::A1, QPair<QString,QString>("A1", "a1"));
        names.insert( QPrinter::A2, QPair<QString,QString>("A2", "a2"));
        names.insert( QPrinter::A3, QPair<QString,QString>("A3", "a3"));
        names.insert( QPrinter::A4, QPair<QString,QString>("A4", "a4"));
        names.insert( QPrinter::A5, QPair<QString,QString>("A5", "a5"));
        names.insert( QPrinter::A6, QPair<QString,QString>("A6", "a6"));
        names.insert( QPrinter::A7, QPair<QString,QString>("A7", "a7"));
        names.insert( QPrinter::A8, QPair<QString,QString>("A8", "a8"));
        names.insert( QPrinter::A9, QPair<QString,QString>("A9", "a9"));
        names.insert( QPrinter::B0, QPair<QString,QString>("B0", "b0"));
        names.insert( QPrinter::B1, QPair<QString,QString>("B1", "b1"));
        names.insert( QPrinter::B2, QPair<QString,QString>("B2", "b2"));
        names.insert( QPrinter::B3, QPair<QString,QString>("B3", "b3"));
        names.insert( QPrinter::B4, QPair<QString,QString>("B4", "b4"));
        names.insert( QPrinter::B5, QPair<QString,QString>("B5", "b5"));
        names.insert( QPrinter::B6, QPair<QString,QString>("B6", "b6"));
        names.insert( QPrinter::B7, QPair<QString,QString>("B7", "b7"));
        names.insert( QPrinter::B8, QPair<QString,QString>("B8", "b8"));
        names.insert( QPrinter::B9, QPair<QString,QString>("B9", "b9"));
        names.insert( QPrinter::B10, QPair<QString,QString>("B10", "b10"));
        names.insert( QPrinter::C5E, QPair<QString,QString>("C5E", "c5e"));
        names.insert( QPrinter::DLE, QPair<QString,QString>("DLE", "dle"));
        names.insert( QPrinter::Executive, QPair<QString,QString>("Executive", "executive"));
        names.insert( QPrinter::Folio, QPair<QString,QString>("Folio", "folio"));
        names.insert( QPrinter::Ledger, QPair<QString,QString>("Ledger", "ledger"));
        names.insert( QPrinter::Legal, QPair<QString,QString>("Legal", "legal"));
        names.insert( QPrinter::Letter, QPair<QString,QString>("Letter", "letter"));
        names.insert( QPrinter::Tabloid, QPair<QString,QString>("Tabloid", "tabloid"));

        KStandardDirs sd;
        QStringList sl = sd.findDirs("templates", "kipiplugins_photolayoutseditor/data/templates/");
        if (sl.count() == 0)
            return;
        QString dir = sl.first();
        for (QMap<int, QPair<QString,QString> >::iterator pair = names.begin(); pair != names.end(); ++pair)
        {
            QString tmp = dir + pair->second;
            QDir dv(tmp + "/v");
            if (dv.exists() && dv.entryList(QStringList() << "*.ple", QDir::Files).count())
            {
                QListWidgetItem * temp = new QListWidgetItem(pair->first);
                temp->setData(PAPER_SIZE_ROLE, pair.key());
                paperSizes.append(temp);
                continue;
            }
            QDir dh(tmp + "/h");
            if (dh.exists() && dh.entryList(QStringList() << "*.ple", QDir::Files).count())
            {
                QListWidgetItem * temp = new QListWidgetItem(pair->first);
                temp->setData(PAPER_SIZE_ROLE, pair.key());
                paperSizes.append(temp);
                continue;
            }
        }
    }

    QStackedLayout * stack;
    QListWidget * paperSize;
    TemplatesView * templatesList;
    CanvasSizeWidget * canvasSize;

    KPushButton * horizontalButton;
    KPushButton * verticalButton;
    QButtonGroup * orientationGroup;

    QList<QListWidgetItem *> paperSizes;

    QMap<int, QPair<QString,QString> > names;

    friend class NewCanvasDialog;
};

NewCanvasDialog::NewCanvasDialog(QWidget *parent) :
    KDialog(parent),
    d(new Private)
{
    this->setupUI();
}

NewCanvasDialog::~NewCanvasDialog()
{
    delete d;
}

bool NewCanvasDialog::hasTemplateSelected() const
{
    return (d->stack->currentWidget() == d->templatesList);
}

QString NewCanvasDialog::templateSelected() const
{
    return d->templatesList->selectedPath();
}

CanvasSize NewCanvasDialog::canvasSize() const
{
    if (d->stack->currentWidget() == d->canvasSize)
        return d->canvasSize->canvasSize();
    else
    {
        int w = 0;
        int h = 0;
        switch (d->paperSize->currentItem()->data(PAPER_SIZE_ROLE).toInt())
        {
            case QPrinter::A0:
                w = 841; h = 1189;
                break;
            case QPrinter::A1:
                w = 594; h = 841;
                break;
            case QPrinter::A2:
                w = 420; h = 594;
                break;
            case QPrinter::A3:
                w = 297; h = 420;
                break;
            case QPrinter::A4:
                w = 210; h = 297;
                break;
            case QPrinter::A5:
                w = 148; h = 210;
                break;
            case QPrinter::A6:
                w = 105; h = 148;
                break;
            case QPrinter::A7:
                w = 74; h = 105;
                break;
            case QPrinter::A8:
                w = 52; h = 74;
                break;
            case QPrinter::A9:
                w = 37; h = 52;
                break;
            case QPrinter::B0:
                w = 1030; h = 1456 ;
                break;
            case QPrinter::B1:
                w = 728; h = 1030;
                break;
            case QPrinter::B2:
                w = 515; h = 728;
                break;
            case QPrinter::B3:
                w = 364; h = 515;
                break;
            case QPrinter::B4:
                w = 257; h = 364;
                break;
            case QPrinter::B5:
                w = 182; h = 257;
                break;
            case QPrinter::B6:
                w = 128; h = 182;
                break;
            case QPrinter::B7:
                w = 91; h = 128;
                break;
            case QPrinter::B8:
                w = 64; h = 91;
                break;
            case QPrinter::B9:
                w = 45; h = 64;
                break;
            case QPrinter::B10:
                w = 32; h = 45;
                break;
            case QPrinter::C5E:
                w = 163; h = 229;
                break;
            case QPrinter::Comm10E:
                w = 105; h = 241;
                break;
            case QPrinter::DLE:
                w = 110; h = 220;
                break;
            case QPrinter::Executive:
                w = 191; h = 254;
                break;
            case QPrinter::Folio:
                w = 210; h = 330;
                break;
            case QPrinter::Ledger:
                w = 432; h = 279;
                break;
            case QPrinter::Legal:
                w = 216; h = 356;
                break;
            case QPrinter::Letter:
                w = 216; h = 279;
                break;
            case QPrinter::Tabloid:
                w = 279; h = 432;
                break;
            case -1:
                return d->canvasSize->canvasSize();
        }

        if (d->horizontalButton->isChecked() && w < h)
        {
            int t = w;
            w = h;
            h = t;
        }

        return CanvasSize(QSizeF(w, h), CanvasSize::Milimeters, QSizeF(72, 72), CanvasSize::PixelsPerInch);
    }
}

void NewCanvasDialog::paperSizeSelected(QListWidgetItem * current, QListWidgetItem * /*previous*/)
{
    int size = current->data(PAPER_SIZE_ROLE).toInt();
    // Custom size
    if (size == -1)
    {
        d->stack->setCurrentWidget(d->canvasSize);
    }
    // Template
    else
    {
        d->stack->setCurrentWidget(d->templatesList);

        TemplatesModel * model = new TemplatesModel();
        d->templatesList->setModel(model);

        QPair<QString,QString> paper = d->names[size];
        model->addTemplate("", i18n("Empty"));
        if (!d->horizontalButton->isChecked())
            this->loadTemplatesList(QString("kipiplugins_photolayoutseditor/data/templates/") + paper.second + QString("/v"), model);
        if (!d->verticalButton->isChecked())
            this->loadTemplatesList(QString("kipiplugins_photolayoutseditor/data/templates/") + paper.second + QString("/h"), model);
    }
}

void NewCanvasDialog::orientationChanged()
{
    if (d->stack->currentWidget() == d->templatesList)
        this->paperSizeSelected(d->paperSize->currentItem(), 0);
    else
    {
        if (d->canvasSize->orientation() == CanvasSizeWidget::Vertical)
            d->verticalButton->setChecked(true);
        else
            d->horizontalButton->setChecked(true);
    }
}

void NewCanvasDialog::setHorizontal(bool isset)
{
    if (!isset || d->horizontalButton->isChecked())
        return;
    if (d->stack->currentWidget() == d->templatesList)
        this->paperSizeSelected(d->paperSize->currentItem(), 0);
}

void NewCanvasDialog::setVertical(bool isset)
{
    if (!isset || d->verticalButton->isChecked())
        return;
    if (d->stack->currentWidget() == d->templatesList)
        this->paperSizeSelected(d->paperSize->currentItem(), 0);
}

void NewCanvasDialog::setupUI()
{
    this->setCaption(i18n("Create new canvas..."));

    QWidget * main = new QWidget(this);
    setMainWidget(main);

    QVBoxLayout * layout = new QVBoxLayout();
    main->setLayout(layout);

    QHBoxLayout * mainLayout = new QHBoxLayout();
    layout->addLayout(mainLayout);

    QVBoxLayout * leftLayout = new QVBoxLayout();
    mainLayout->addLayout(leftLayout);

    leftLayout->addWidget(new QLabel(i18n("Paper sizes"), main));

    d->paperSize = new QListWidget(main);
    d->paperSize->setMaximumWidth(150);
    connect(d->paperSize, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(paperSizeSelected(QListWidgetItem*,QListWidgetItem*)));
    foreach (QListWidgetItem * i, d->paperSizes)
        d->paperSize->addItem(i);
    leftLayout->addWidget(d->paperSize);

    // Orientation buttons
    d->horizontalButton = new KPushButton(KIcon(":horizontal_orientation.png"),"", main);
    d->horizontalButton->setCheckable(true);
    d->horizontalButton->setIconSize(QSize(24,24));
    d->verticalButton = new KPushButton(KIcon(":vertical_orientation.png"),"", main);
    d->verticalButton->setCheckable(true);
    d->verticalButton->setIconSize(QSize(24,24));
    QHBoxLayout * hLayout = new QHBoxLayout();
    hLayout->addWidget(d->horizontalButton);
    hLayout->addWidget(d->verticalButton);
    leftLayout->addLayout(hLayout);
    d->orientationGroup = new QButtonGroup(main);
    d->orientationGroup->addButton(d->horizontalButton);
    d->orientationGroup->addButton(d->verticalButton);
    connect(d->horizontalButton, SIGNAL(toggled(bool)), this, SLOT(setHorizontal(bool)));
    connect(d->verticalButton, SIGNAL(toggled(bool)), this, SLOT(setVertical(bool)));

    QVBoxLayout * rightLayout = new QVBoxLayout();
    mainLayout->addLayout(rightLayout);

    rightLayout->addWidget(new QLabel(i18n("Select a template"), main));

    d->stack = new QStackedLayout();
    rightLayout->addLayout(d->stack, 1);

    d->canvasSize = new CanvasSizeWidget(main);
    d->stack->addWidget(d->canvasSize);
    connect(d->canvasSize, SIGNAL(orientationChanged()), this, SLOT(orientationChanged()));
    connect(d->horizontalButton, SIGNAL(toggled(bool)), d->canvasSize, SLOT(setHorizontal(bool)));
    connect(d->verticalButton, SIGNAL(toggled(bool)), d->canvasSize, SLOT(setVertical(bool)));

    d->templatesList = new TemplatesView(main);
    d->stack->addWidget(d->templatesList);

    d->paperSize->setCurrentRow(0);
}

void NewCanvasDialog::loadTemplatesList(const QString & path, TemplatesModel * model)
{
    KStandardDirs sd;
    QStringList sl = sd.findDirs("templates", path);
    if (sl.count() == 0)
        return;
    QDir diro(sl.first());
    QStringList files = diro.entryList(QStringList() << "*.ple", QDir::Files);
    foreach (QString s, files)
        model->addTemplate(diro.path() + "/" + s, s);
}
