/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-11-03
 * Description : main dialog.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006      by Tom Albers <tomalbers@kde.nl>
 * Copyright (C) 2007-2008 by Orgad Shaneh <orgads at gmail dot com>
 * Copyright (C) 2011      by Andi Clemens <andi dot clemens at googlemail dot com>
 * Copyright (C) 2012      by Angelo Naselli <anaselli at linux dot it>
 * Copyright (C) 2012-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "calwizard.h"

// Qt includes

#include <QDate>
#include <QPrintDialog>
#include <QPrinter>
#include <QStringList>
#include <QMenu>
#include <QPushButton>

// KDE includes

#include <kcalendarsystem.h>
#include <kglobal.h>
#include <klocalizedstring.h>

// Libkipi includes

#include <KIPI/Interface>

// Libkdcraw includes

#include <KDCRAW/RWidgetUtils>

// Local includes

#include "calprinter.h"
#include "calsettings.h"
#include "caltemplate.h"
#include "kpaboutdata.h"
#include "kipiplugins_debug.h"

namespace KIPICalendarPlugin
{

CalWizard::CalWizard(QWidget* const parent)
    : KPWizardDialog(parent)
{
    setWindowTitle(i18n("Create Calendar"));
    setMaximumSize(800, 600);
    cSettings_   = CalSettings::instance(this);

    // ---------------------------------------------------------------

    wTemplate_   = new CalTemplate(iface(), this);
    addPage(wTemplate_, i18n("Create Template for Calendar"));

    // ---------------------------------------------------------------

    wEvents_     = new QWidget(this);
    calEventsUI.setupUi(wEvents_);
    addPage(wEvents_, i18n("Choose events to show on the Calendar"));

    // ---------------------------------------------------------------

    wPrintLabel_ = new QLabel(this);
    wPrintLabel_->setIndent(20);
    wPrintLabel_->setWordWrap(true);

    wPrintPage_ = addPage(wPrintLabel_, i18n("Print"));

    // ---------------------------------------------------------------

    wFinish_     = new QWidget(this);
    calProgressUI.setupUi(wFinish_);
    wFinishPage_ = addPage(wFinish_, i18n("Printing"));
    
    // ---------------------------------------------------------------

    ohUrlRequester = new RFileSelector(calEventsUI.ohUrlWidget);
    ohUrlRequester->fileDialog()->setNameFilter(i18n("%1|Calendar Data File", QLatin1String("*.ics")));
    ohUrlRequester->fileDialog()->setWindowTitle(i18n("Select Calendar Data File"));
    ohUrlRequester->fileDialog()->setFileMode(QFileDialog::ExistingFile);

    fhUrlRequester = new RFileSelector(calEventsUI.ohUrlWidget);
    fhUrlRequester->fileDialog()->setNameFilter(i18n("%1|Calendar Data File", QLatin1String("*.ics")));
    fhUrlRequester->fileDialog()->setWindowTitle(i18n("Select Calendar Data File"));
    fhUrlRequester->fileDialog()->setFileMode(QFileDialog::ExistingFile);

    // ---------------------------------------------------------------

    // About data and help button.

    KPAboutData* const about = new KPAboutData(ki18n("Calendar"),
                                               0,
                                               KAboutLicense::GPL,
                                               ki18n("A Kipi plugin to create a calendar"),
                                               ki18n("(c) 2003-2005, Renchi Raju\n"
                                                     "(c) 2006 Tom Albers\n"
                                                     "(c) 2007-2008 Orgad Shaneh"));

    about->addAuthor(ki18n("Orgad Shaneh").toString(),
                     ki18n("Developer").toString(),
                     QStringLiteral("orgads@gmail.com"));

    about->addAuthor(ki18n("Tom Albers").toString(),
                     ki18n("Developer").toString(),
                     QStringLiteral("tomalbers@kde.nl"));

    about->addAuthor(ki18n("Renchi Raju").toString(),
                     ki18n("Author").toString(),
                     QStringLiteral("renchi dot raju at gmail dot com"));

    about->setHandbookEntry(QStringLiteral("calendar"));
    setAboutData(about);

    // ------------------------------------------

    printThread_ = 0;
    printer_     = 0;

    connect(this, SIGNAL(currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)),
            this, SLOT(slotPageSelected(KPageWidgetItem*,KPageWidgetItem*)));
}

CalWizard::~CalWizard()
{
    if (printThread_)
    {
        printThread_->cancel();
        printThread_->wait();
        delete printThread_;
    }

    delete printer_;
}

void CalWizard::slotPageSelected(KPageWidgetItem* current, KPageWidgetItem* before)
{
    Q_UNUSED(before);

    if (current == wPrintPage_)
    {
        months_.clear();
        QUrl image;
        QString month;
        QStringList printList;
        QDate d;
        KLocale::global()->calendar()->setDate(d, cSettings_->year(), 1, 1);

        for (int i = 1; i <= KLocale::global()->calendar()->monthsInYear(d); ++i)
        {
            month = KLocale::global()->calendar()->monthName(i, cSettings_->year(), KCalendarSystem::LongName);
            image = cSettings_->image(i);

            if (!image.isEmpty())
            {
                months_.insert(i, image);
                printList.append(month);
            }
        }

        if (months_.empty())
        {
            wPrintLabel_->setText(QStringLiteral("<qt>") +
                                  i18n("No valid images selected for months<br/>"
                                       "Click Back to select images") + QStringLiteral("</qt>"));
            setValid(wFinishPage_, false);
        }
        else
        {
            int year = cSettings_->year();

            QString extra;

            if ((KLocale::global()->calendar()->month(QDate::currentDate()) >= 6 &&
                 KLocale::global()->calendar()->year(QDate::currentDate()) == year) ||
                 KLocale::global()->calendar()->year(QDate::currentDate()) > year)
                extra = QStringLiteral("<br/><br/><b>") +
                    i18n("Please note that you are making a "
                         "calendar for<br/>the current year or a year in the "
                         "past.") + QStringLiteral("</b>");

            QString year_locale = KLocale::global()->calendar()->formatDate(d, KLocale::Year, KLocale::LongNumber);

            wPrintLabel_->setText(i18n("Click Next to start Printing<br/><br/>"
                                       "Following months will be printed for year %1:<br/>", year_locale)
                                  + printList.join(QStringLiteral(" - ")) + extra);
            wPrintLabel_->setTextFormat(Qt::RichText);

            setValid(wFinishPage_, true);
        }
    }

    else if (current == wFinishPage_)
    {
        calProgressUI.finishLabel->clear();
        calProgressUI.currentProgress->reset();
        calProgressUI.totalProgress->reset();

        backButton()->setEnabled(false);
        nextButton()->setEnabled(false);

        // Set printer settings ---------------------------------------

        if (!printer_)
        {
            printer_ = new QPrinter(cSettings_->resolution());
        }

        CalParams& params = cSettings_->params;

        // Orientation
        switch (params.imgPos)
        {
            case (CalParams::Top):
                printer_->setOrientation(QPrinter::Portrait);
                break;

            default:
                printer_->setOrientation(QPrinter::Landscape);
                break;
        }

        qCDebug(KIPIPLUGINS_LOG) << "printing...";

        // PageSize
        printer_->setPageSize(params.pageSize);
        
        QPrintDialog* const printDialog = new QPrintDialog(printer_, this);
        printDialog->setWindowTitle(i18n("Print Calendar"));

        if (printDialog->exec() == QDialog::Accepted)
        {
            print();
        }
        else
        {
            calProgressUI.finishLabel->setText(i18n("Printing Cancelled"));
            backButton()->setEnabled(true);
        }

        delete printDialog;
    }
}

void CalWizard::print()
{
    calProgressUI.totalProgress->setMaximum(months_.count());
    calProgressUI.totalProgress->setValue(0);
    calProgressUI.totalProgress->progressScheduled(i18n("Making calendar"), false, true);
    calProgressUI.totalProgress->progressThumbnailChanged(
        QIcon::fromTheme(QStringLiteral("kipi")).pixmap(22, 22));

    if (printThread_)
    {
        printThread_->cancel();
        printThread_->wait();
        delete printThread_;
    }

    cSettings_->clearSpecial();
    cSettings_->loadSpecial(QUrl::fromLocalFile(ohUrlRequester->lineEdit()->text()), Qt::red);
    cSettings_->loadSpecial(QUrl::fromLocalFile(fhUrlRequester->lineEdit()->text()), Qt::darkGreen);

    printThread_ = new CalPrinter(printer_, months_, iface(), this);

    connect(printThread_, SIGNAL(pageChanged(int)),
            this,         SLOT(updatePage(int)));

    connect(printThread_, SIGNAL(pageChanged(int)),
            calProgressUI.totalProgress, SLOT(setValue(int)));

    connect(printThread_, SIGNAL(totalBlocks(int)),
            calProgressUI.currentProgress, SLOT(setMaximum(int)));

    connect(printThread_, SIGNAL(blocksFinished(int)),
            calProgressUI.currentProgress, SLOT(setValue(int)));

    calProgressUI.totalProgress->setMaximum(months_.count());
    printThread_->start();
}

void CalWizard::updatePage(int page)
{
    const int year = cSettings_->year();
    QDate date(year, 1, 1);

    if (page >= months_.count())
    {
        printComplete();
        return;
    }

    int month = months_.keys().at(page);

    calProgressUI.finishLabel->setText(i18n(
        "Printing calendar page for %1 of %2",
        KLocale::global()->calendar()->monthName(month, year, KCalendarSystem::LongName),
        KLocale::global()->calendar()->formatDate(date, QStringLiteral("%Y"))));
}

void CalWizard::printComplete()
{
    calProgressUI.totalProgress->progressCompleted();
    backButton()->setEnabled(true);
    nextButton()->setEnabled(true);
    calProgressUI.finishLabel->setText(i18n("Printing Complete"));
}

}  // nameSpace KIPICalendarPlugin
