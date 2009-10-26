/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-11-03
 * Description : main dialog.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006 by Tom Albers <tomalbers@kde.nl>
 * Copyright (C) 2007-2008 by Orgad Shaneh <orgads at gmail dot com>
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
#include "calwizard.moc"

// Qt includes

#include <QDate>
#include <QPrintDialog>
#include <QPrinter>
#include <QStringList>

// KDE includes

#include <kcalendarsystem.h>
#include <kdebug.h>
#include <kdeprintdialog.h>
#include <kglobal.h>
#include <khelpmenu.h>
#include <klocale.h>
#include <kmenu.h>
#include <ktoolinvocation.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "calprinter.h"
#include "calselect.h"
#include "calsettings.h"
#include "caltemplate.h"
#include "kpaboutdata.h"

namespace KIPICalendarPlugin
{

CalWizard::CalWizard( KIPI::Interface* interface, QWidget *parent )
         : KAssistantDialog(parent),
           interface_( interface )
{
    setMaximumSize( 800, 600 );
    cSettings_ = CalSettings::instance();

    // ---------------------------------------------------------------

    wTemplate_ = new CalTemplate( this );
    addPage(wTemplate_, i18n( "Create Template for Calendar" ));

    // ---------------------------------------------------------------

    wEvents_ = new QWidget( this );
    calEventsUI.setupUi( wEvents_ );
    addPage(wEvents_, i18n( "Choose events to show on the Calendar" ));

    // ---------------------------------------------------------------

    wSelect_ = new CalSelect( interface, this );
    addPage(wSelect_, i18n( "Select Year & Images" ));

    // ---------------------------------------------------------------

    wPrintLabel_ = new QLabel( this );
    wPrintLabel_->setIndent( 20 );
    wPrintLabel_->setWordWrap( true );

    wPrintPage_ = addPage( wPrintLabel_, i18n( "Print" ) );

    // ---------------------------------------------------------------

    wFinish_ = new QWidget( this );
    calProgressUI.setupUi( wFinish_ );
    wFinishPage_ = addPage(wFinish_, i18n( "Printing" ));

    // ---------------------------------------------------------------

    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(ki18n("Calendar"),
                                           0,
                                           KAboutData::License_GPL,
                                           ki18n("A Kipi plugin to create a calendar"),
                                           ki18n("(c) 2003-2005, Renchi Raju\n"
                                                 "(c) 2006 Tom Albers\n"
                                                 "(c) 2007-2008 Orgad Shaneh"));

    m_about->addAuthor(ki18n("Orgad Shaneh"), ki18n("Author and maintainer"),
                       "orgads@gmail.com");

    m_about->addAuthor(ki18n("Tom Albers"), ki18n("Former author and maintainer"),
                       "tomalbers@kde.nl");

    m_about->addAuthor(ki18n("Renchi Raju"), ki18n("Former author and maintainer"),
                       "renchi dot raju at gmail dot com");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered()),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    // ------------------------------------------

    printThread_ = 0;
    printer_     = 0;

    connect(this, SIGNAL(currentPageChanged(KPageWidgetItem *, KPageWidgetItem *)),
            this, SLOT(slotPageSelected(KPageWidgetItem *, KPageWidgetItem *)));

    setCaption(i18n("Create Calendar"));
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

    delete m_about;
}

void CalWizard::slotHelp()
{
    KToolInvocation::invokeHelp("calendar", "kipi-plugins");
}

void CalWizard::slotPageSelected(KPageWidgetItem *current, KPageWidgetItem *before)
{
    Q_UNUSED(before);

    if (current == wPrintPage_)
    {
        months_.clear();
        KUrl image;
        QString month;
        QStringList printList;
        QDate d;
        KGlobal::locale()->calendar()->setDate(d, cSettings_->year(), 1, 1);
        for (int i=1; i<=KGlobal::locale()->calendar()->monthsInYear(d); i++)
        {
            month = KGlobal::locale()->calendar()->monthName(i, cSettings_->year(), KCalendarSystem::LongName);
            image = cSettings_->image(i);
            if (!image.isEmpty())
            {
                months_.insert(i, image);
                printList.append(month);
            }
        }

        if (months_.empty())
        {
            wPrintLabel_->setText("<qt>" +i18n("No valid images selected for months<br/>"
                    "Click Back to select images")+"</qt>");
            setValid(wFinishPage_, false);
        }
        else
        {
            int year = cSettings_->year();

            QString extra;
            if ((KGlobal::locale()->calendar()->month(QDate::currentDate()) >= 6 &&
                 KGlobal::locale()->calendar()->year(QDate::currentDate()) == year) ||
                 KGlobal::locale()->calendar()->year(QDate::currentDate()) > year)
                extra = "<br/><br/><b>"+i18n("Please note that you are making a "
                        "calendar for<br/>the current year or a year in the "
                        "past.")+"</b>";

	    KLocale tmpLocale(*KGlobal::locale());
	    tmpLocale.setDateFormat("%Y");
	    QString year_locale = tmpLocale.formatDate(d);

            wPrintLabel_->setText(i18n("Click Next to start Printing<br/><br/>"
                    "Following months will be printed for year %1:<br/>", year_locale)
                    + printList.join(" - ") + extra);
            wPrintLabel_->setTextFormat(Qt::RichText);

            setValid(wFinishPage_, true);
        }
    }

    else if (current == wFinishPage_)
    {
        calProgressUI.finishLabel->clear();
        calProgressUI.currentProgress->reset();
        calProgressUI.totalProgress->reset();

        enableButton(KDialog::User3, false); // disable 'Back' button
        enableButton(KDialog::User1, false); // disable 'Finish' button

        // Set printer settings ---------------------------------------

        if (!printer_)
            printer_ = new QPrinter( QPrinter::HighResolution );

        // TODO: Let user choose resolutions

        CalParams& params = cSettings_->params;

        // Orientation
        switch (params.imgPos)
        {
            case(CalParams::Top):
            {
                printer_->setOrientation(QPrinter::Portrait);
                break;
            }
            default:
            {
                printer_->setOrientation(QPrinter::Landscape);
            }
        }

        kDebug() << "printing...";
        // PageSize
        printer_->setPageSize(params.pageSize);
        QPrintDialog *printDialog = KdePrint::createPrintDialog( printer_, this );

        if ( printDialog->exec() == QDialog::Accepted )
        {
            print();
        }
        else
        {
            calProgressUI.finishLabel->setText(i18n( "Printing Cancelled" ));
            enableButton(KDialog::User3, true); // enable 'Back' button
        }
        delete printDialog;

    }
}

void CalWizard::print()
{
    calProgressUI.totalProgress->setMaximum( months_.count() );
    calProgressUI.totalProgress->setValue( 0 );

    if (printThread_)
    {
        printThread_->cancel();
        printThread_->wait();
        delete printThread_;
    }

    cSettings_->clearSpecial();
    cSettings_->loadSpecial( calEventsUI.ohUrlRequester->url(), Qt::red );
    cSettings_->loadSpecial( calEventsUI.fhUrlRequester->url(), Qt::darkGreen );

    printThread_ = new CalPrinter( printer_,
                                   months_,
                                   interface_,
                                   this );

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

    if (page >= months_.count())
    {
        printComplete();
        return;
    }
    int month = months_.keys().at( page );

    calProgressUI.finishLabel->setText(i18n("Printing Calendar Page for %1 of %2",
                                       KGlobal::locale()->calendar()->monthName(month, year, KCalendarSystem::LongName),
                                       year));
}

void CalWizard::printComplete()
{
    enableButton(KDialog::User3, true); // enable 'Back' button
    enableButton(KDialog::User1, true); // enable 'Finish' button
    calProgressUI.finishLabel->setText(i18n( "Printing Complete" ));
}

}  // nameSpace KIPICalendarPlugin
