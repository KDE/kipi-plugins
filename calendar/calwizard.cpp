/* ============================================================
 * File  : calwizard.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *         Tom Albers <tomalbers@kde.nl>
 * Date  : 2003-11-03
 * Description :
 *
 * Copyright 2003 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright 2006 by Tom Albers <tomalbers@kde.nl>
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

// Qt includes.

#include <qlabel.h>
#include <qdatetime.h>
#include <qstringlist.h>
#include <qtimer.h>
#include <qpainter.h>
#include <qprogressbar.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qvbox.h>

// KDE includes.

#include <kglobal.h>
#include <klocale.h>
#include <kprinter.h>
#include <kapplication.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kstandarddirs.h>
#include <kdeversion.h>
#include <kcalendarsystem.h>
#include <klineedit.h>

// LibKipi includes.

#include <libkipi/interface.h>

// Local includes.

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "caltemplate.h"
#include "calselect.h"
#include "calsettings.h"
#include "calpainter.h"
#include "calwizard.h"
#include "calwizard.moc"
#include "calevents.h"

namespace KIPICalendarPlugin
{

CalWizard::CalWizard( KIPI::Interface* interface, QWidget *parent )
         : KWizard(parent, 0, false, Qt::WDestructiveClose),
           interface_( interface )
{
    cSettings_ = new CalSettings();

    // ---------------------------------------------------------------

    wTemplate_ = new CalTemplate(this, "wTemplate");
    addPage(wTemplate_, i18n("Create Template for Calendar"));
    setHelpEnabled(wTemplate_, true);

    // ---------------------------------------------------------------

    wEvents_ = new CalEvents(this, "wEvents");
    addPage(wEvents_, i18n("Choose the events to show on the Calendar"));
    setHelpEnabled(wEvents_, true);

    // ---------------------------------------------------------------

    wSelect_ = new CalSelect( interface, this, "wSelect");
    addPage(wSelect_, i18n("Select Year & Images"));
    setHelpEnabled(wSelect_, true);

    // ---------------------------------------------------------------

    wPrint_ = new QVBox(this, "wPrint");

    wPrintLabel_ = new QLabel(wPrint_, "wPrint");
    wPrintLabel_->setIndent(20);

    wPrint_->setStretchFactor(wPrintLabel_, 2);

    addPage(wPrint_, i18n("Print"));
    setHelpEnabled(wPrint_, true);

    // ---------------------------------------------------------------

    wFinish_ = new QWidget(this, "wFinish");

    QVBoxLayout *wFinishLayout = new QVBoxLayout(wFinish_, 6, 11);

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

    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(I18N_NOOP("Calendar"),
                                           NULL,
                                           KAboutData::License_GPL,
                                           I18N_NOOP("A Kipi plugin to create a calendar"),
                                           "(c) 2003-2004, Renchi Raju, (c) 2006 Tom Albers");

    m_about->addAuthor("Tom Albers", I18N_NOOP("Author and maintainer"),
                       "tomalbers@kde.nl");

    m_about->addAuthor("Renchi Raju", I18N_NOOP("Former Author and maintainer"),
                       "renchi@pooh.tam.uiuc.edu");

    m_helpButton = helpButton();
    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Calendar Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    m_helpButton->setPopup( helpMenu->menu() );

    // ------------------------------------------

    printer_  = 0;
    painter_  = 0;

    formatter_ = new CalFormatter();

    connect(this, SIGNAL(selected(const QString&)),
            SLOT(slotPageSelected(const QString&)));

    setCaption(i18n("Create Calendar"));
}

CalWizard::~CalWizard()
{
    if (!cb_.isNull()) delete cb_;

    if (painter_) delete painter_;
    if (printer_) delete printer_;
    delete cSettings_;

    delete m_about;

    delete formatter_;
}

void CalWizard::slotHelp()
{
    KApplication::kApplication()->invokeHelp("calendar", "kipi-plugins");
}

void CalWizard::slotPageSelected(const QString&)
{

    if (currentPage() == wPrint_) {

        totPages_ = 0;
        currPage_ = 0;
        monthNumbers_.clear();
        monthImages_.clear();
        KURL image;
        QString month;
        QStringList printList;
        QDate d;
        KGlobal::locale()->calendar()->setYMD(d, cSettings_->getYear(), 1, 1);
        for (int i=1; i<=KGlobal::locale()->calendar()->monthsInYear(d); i++) {
#if KDE_IS_VERSION(3,2,0)
            month = KGlobal::locale()->calendar()->monthName(i, cSettings_->getYear(), false);
#else
            month = KGlobal::locale()->monthName(i);
#endif
            image = cSettings_->getImage(i);
            if (!image.isEmpty()) {
                monthNumbers_.append(i);
                monthImages_.append(image);
                printList.append(month);
            }
        }

        if (!monthNumbers_.empty()) {
            QString year = QString::number(cSettings_->getYear());

            QString extra;
            if ((KGlobal::locale()->calendar()->month(QDate::currentDate()) >= 6 &&
                 KGlobal::locale()->calendar()->year(QDate::currentDate()) == cSettings_->getYear()) ||
                KGlobal::locale()->calendar()->year(QDate::currentDate()) > cSettings_->getYear())
                extra = "<br><br><b>"+i18n("Please note that you are making a "
                        "calendar for<br>the current year or a year in the "
                        "past.")+"</b>";

            KApplication::startServiceByName("KJobViewer");
            QString extra2 = i18n("<br><br>You can see KJobViewer is already started. "
                    "After the plugin has prepared the calendar, it is passed to "
                    "the PDF printer. In the KJobViewer you can see the progress "
                    "of that part of the generation of the calendar.");

            wPrintLabel_->setText(i18n("Click Next to start Printing<br><br>"
                                       "Following months will be printed for year %1:").arg(year)
                                  + QString("<br>")
                             + printList.join(" - ") + extra + extra2);
            wPrintLabel_->setTextFormat(Qt::RichText);

            setNextEnabled(wPrint_, true);
        }
        else {
            wPrintLabel_->setText(i18n("No valid images selected for months<br>"
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
#if KDE_IS_VERSION(3,2,0)
        printer_->setUsePrinterResolution(true);
#endif

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
    KURL image(monthImages_.first());
    monthNumbers_.pop_front();
    monthImages_.pop_front();

    QString yearName = QString::number(cSettings_->getYear());


    formatter_->init(cSettings_->getYear(), wEvents_->ohFileEdit->text(), wEvents_->fhFileEdit->text());

#if KDE_IS_VERSION(3,2,0)
    wFinishLabel_->setText(i18n("Printing Calendar Page for %1 of %2")
                    .arg(KGlobal::locale()->calendar()->monthName(month, cSettings_->getYear(), false))
                    .arg(yearName));
#else
    wFinishLabel_->setText(i18n("Printing Calendar Page for %1 of %2")
                     .arg(KGlobal::locale()->monthName(month)).
                     .arg(yearName));
#endif

    currPage_++;
    if (currPage_ != 0)
        printer_->newPage();
    wFinishProgressTotal_->setProgress(currPage_,totPages_);

    int angle = interface_->info( image ).angle();

    cb_ = new CalBlockPainter(this, cSettings_->getYear(), month,
                              image, angle, formatter_, painter_);

    connect(cb_, SIGNAL(signalCompleted()),
            SLOT(slotPrintOnePage()));
    connect(cb_, SIGNAL(signalProgress(int,int)),
            wFinishProgressCurrent_, SLOT(setProgress(int,int)));
}

}  // NameSpace KIPICalendarPlugin

