/* ============================================================
 * File  : calwizard.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-11-03
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

#include <kglobal.h>
#include <klocale.h>
#include <kprinter.h>
#include <kapplication.h>

#include <qlabel.h>
#include <qdatetime.h>
#include <qstringlist.h>
#include <qtimer.h>
#include <qpainter.h>
#include <qprogressbar.h>
#include <qlayout.h>

#include "calwizard.h"
#include "caltemplate.h"
#include "calselect.h"
#include "calsettings.h"
#include "calpainter.h"

namespace DKCalendar
{

CalWizard::CalWizard( KIPI::Interface* interface )
    : KWizard(0,0,false,Qt::WDestructiveClose)
{
    cSettings_ = new CalSettings();

    // ---------------------------------------------------------------

    wTemplate_ = new CalTemplate(this, "wTemplate");
    addPage(wTemplate_, i18n("Create Template for the Calendar"));
    setHelpEnabled(wTemplate_, true);

    // ---------------------------------------------------------------

    wSelect_ = new CalSelect( interface, this, "wSelect");
    addPage(wSelect_, i18n("Select Year and Images"));
    setHelpEnabled(wSelect_, true);

    // ---------------------------------------------------------------

    wPrint_ = new QLabel(this, "wPrint");
    wPrint_->setIndent(20);
    addPage(wPrint_, i18n("Print"));
    setHelpEnabled(wPrint_, true);

    // ---------------------------------------------------------------

    wFinish_ = new QWidget(this, "wFinish");
    QVBoxLayout *wFinishLayout =
        new QVBoxLayout(wFinish_, 6, 11);

    wFinishLabel_ = new QLabel(wFinish_);
    wFinishLayout->addWidget(wFinishLabel_);

    QHBoxLayout *hboxlayout = new QHBoxLayout(0, 5, 5);
    hboxlayout->addWidget(new QLabel(i18n("Current Page"), wFinish_));
    wFinishProgressCurrent_ = new QProgressBar(wFinish_);
    hboxlayout->addWidget(wFinishProgressCurrent_);
    wFinishLayout->addLayout(hboxlayout);

    hboxlayout = new QHBoxLayout(0, 5, 5);
    hboxlayout->addWidget(new QLabel(i18n("Total Pages"), wFinish_));
    wFinishProgressTotal_ = new QProgressBar(wFinish_);
    hboxlayout->addWidget(wFinishProgressTotal_);
    wFinishLayout->addLayout(hboxlayout);

    wFinishLayout->addStretch();

    addPage(wFinish_, i18n("Printing"));
    setHelpEnabled(wFinish_, true);

    // ---------------------------------------------------------------

    printer_  = 0;
    painter_  = 0;

    connect(this, SIGNAL(selected(const QString&)),
            SLOT(slotPageSelected(const QString&)));
    connect(this, SIGNAL(helpClicked()),
            this, SLOT(slotHelp()));
}

CalWizard::~CalWizard()
{
    if (!cb_.isNull()) delete cb_;

    if (painter_) delete painter_;
    if (printer_) delete printer_;
    delete cSettings_;
}

void CalWizard::slotHelp()
{
    KApplication::kApplication()->invokeHelp("plugin-calendar.anchor",
                                             "kipi");
}

void CalWizard::slotPageSelected(const QString&)
{

    if (currentPage() == wPrint_) {

        totPages_ = 0;
        currPage_ = 0;
        monthNumbers_.clear();
        monthImages_.clear();

        QString image, month;
        QStringList printList;
        for (int i=1; i<=12; i++) {
            month =  KGlobal::locale()->monthName(i);
            image = cSettings_->getImage(i);
            if (!image.isEmpty()) {
                monthNumbers_.append(i);
                monthImages_.append(image);
                printList.append(month);
            }
        }

        if (!monthNumbers_.empty()) {
            QString year = QString::number(cSettings_->getYear());
            wPrint_->setText(i18n("Click Next to start Printing\n\n"
                                  "Following months will be printed for year %1:").arg(year)
                             + QString("\n")
                             + printList.join("\n"));
            setNextEnabled(wPrint_, true);
        }
        else {
            wPrint_->setText(i18n("No valid images selected for months\n"
                                  "Click Back to select images"));
            setNextEnabled(wPrint_, false);
        }
    }

    else if (currentPage() == wFinish_) {

        wFinishLabel_->clear();
        wFinishProgressTotal_->reset();
        wFinishProgressCurrent_->reset();

        setBackEnabled(wFinish_, false);
        setFinishEnabled(wFinish_, false);

        // Set printer settings ---------------------------------------

        if (!printer_)
            printer_ = new KPrinter(false);

        // TODO: Let user choose resolutions
        //, QPrinter::HighResolution);

        CalParams& params = cSettings_->calParams;

        // Orientation
        switch (params.imgPos) {
        case(CalParams::Top): {
            printer_->setOrientation(KPrinter::Portrait);
            break;
        }
        default:
            printer_->setOrientation(KPrinter::Landscape);
        }

        // PageSize
        printer_->setPageSize(params.pageSize);

        if (printer_->setup(this)) {

            // Start printing --------------------------------------------

            painter_ = new QPainter(printer_);
            totPages_ = monthImages_.count();
            currPage_ = -1;
            slotPrintOnePage();

        }
        else {
            wFinishLabel_->setText(i18n("Printing Cancelled"));
            setBackEnabled(wFinish_, true);
        }

    }

}

void CalWizard::slotPrintOnePage()
{
    if (monthNumbers_.empty()) {
        wFinishProgressTotal_->setProgress(totPages_,totPages_);
        painter_->end();
        delete painter_;
        painter_ = 0;
        setBackEnabled(wFinish_, true);
        setFinishEnabled(wFinish_, true);
        wFinishLabel_->setText(i18n("Printing Complete"));
        return;
    }

    int     month(monthNumbers_.first());
    QString image(monthImages_.first());
    monthNumbers_.pop_front();
    monthImages_.pop_front();

    QString yearName = QString::number(cSettings_->getYear());

    wFinishLabel_->setText(i18n("Printing Calendar Page for %1 of %2").
                           arg(KGlobal::locale()->monthName(month)).
                           arg(yearName));

    currPage_++;
    if (currPage_ != 0)
        printer_->newPage();
    wFinishProgressTotal_->setProgress(currPage_,totPages_);

    cb_ = new CalBlockPainter(this, cSettings_->getYear(), month,
                              image, painter_);
    connect(cb_, SIGNAL(signalCompleted()),
            SLOT(slotPrintOnePage()));
    connect(cb_, SIGNAL(signalProgress(int,int)),
            wFinishProgressCurrent_, SLOT(setProgress(int,int)));
}

}

