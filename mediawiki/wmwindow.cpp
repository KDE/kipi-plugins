/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-11
 * Description : a kipi plugin to export images to wikimedia commons
 *
 * Copyright (C) 2011 by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 * Copyright (C) 2011-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "wmwindow.moc"

// Qt includes

#include <QLayout>
#include <QProgressBar>
#include <QCloseEvent>

// KDE includes

#include <kdebug.h>
#include <KConfig>
#include <KLocale>
#include <KMenu>
#include <KUrl>
#include <KHelpMenu>
#include <KLineEdit>
#include <KComboBox>
#include <KPushButton>
#include <KMessageBox>
#include <KToolInvocation>

// Mediawiki includes

#include <libmediawiki/version.h>

// KIPI includes

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "kpaboutdata.h"
#include "kpimageinfo.h"
#include "wmwidget.h"
#include "wikimediajob.h"
#include "imageslist.h"

namespace KIPIWikiMediaPlugin
{

WMWindow::WMWindow(KIPI::Interface* interface, const QString& tmpFolder, QWidget* /*parent*/)
    : KDialog(0)
{
    m_tmpPath.clear();
    m_tmpDir    = tmpFolder;
    m_interface = interface;
    m_widget    = new WmWidget(this, interface);
    m_uploadJob = 0;
    m_login     = QString();
    m_pass      = QString();

    setMainWidget(m_widget);
    setWindowIcon(KIcon("wikimedia"));
    setButtons(Help|User1|Close);
    setDefaultButton(Close);
    setModal(false);
    setWindowTitle(i18n("Export to Wikimedia Commons"));
    setButtonGuiItem(User1,
                     KGuiItem(i18n("Start Upload"), "network-workgroup",
                              i18n("Start upload to Wikimedia Commons")));
    enableButton(User1,false);
    m_widget->setMinimumSize(700, 500);

    m_about = new KIPIPlugins::KPAboutData(ki18n("Wikimedia Commons Export"), 0,
                               KAboutData::License_GPL,
                               ki18n("A Kipi plugin to export image collection "
                                     "to Wikimedia Commons.\n"
                                     "Using libmediawiki version %1").subs(QString(mediawiki_version)),
                               ki18n("(c) 2011, Alexandre Mendes"));

    m_about->addAuthor(ki18n("Alexandre Mendes"), ki18n("Author"),
                       "alex dot mendes1988 at gmail dot com");

    m_about->addAuthor(ki18n("Guillaume Hormiere"), ki18n("Developer"),
                       "hormiere dot guillaume at gmail dot com");

    m_about->addAuthor(ki18n("Gilles Caulier"), ki18n("Developer"),
                       "caulier dot gilles at gmail dot com");

    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction* handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    disconnect(this, SIGNAL(user1Clicked()),
               this, SLOT(slotStartTransfer()));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotStartTransfer()));

    connect(m_widget, SIGNAL(signalChangeUserRequest()),
            this, SLOT(slotChangeUserClicked()));

    connect(m_widget, SIGNAL(signalLoginRequest(QString,QString,QUrl)),
            this, SLOT(slotDoLogin(QString,QString,QUrl)));

    readSettings();
    reactivate();
}

WMWindow::~WMWindow()
{
    delete m_about;
}

void WMWindow::closeEvent(QCloseEvent* e)
{
    if (!e) return;
    saveSettings();
    e->accept();
}

void WMWindow::reactivate()
{
    m_widget->imagesList()->listView()->clear();
    m_widget->imagesList()->loadImagesFromCurrentSelection();
    show();
}

void WMWindow::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Wikimedia Commons settings"));

    m_widget->readSettings(group);

    KConfigGroup group2 = config.group(QString("Wikimedia Commons dialog"));
    restoreDialogSize(group2);
}

void WMWindow::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Wikimedia Commons settings"));

    m_widget->saveSettings(group);

    KConfigGroup group2 = config.group(QString("Wikimedia Commons dialog"));
    saveDialogSize(group2);
    config.sync();
}

void WMWindow::slotHelp()
{
    KToolInvocation::invokeHelp("wikimedia", "kipi-plugins");
}

void WMWindow::slotStartTransfer()
{
    KUrl::List urls = m_interface->currentSelection().images();

    QList<QMap<QString, QString> > imageDesc;
    QString author      = m_widget->author();
    QString licence     = m_widget->licence();
    QString description = m_widget->description();

    for (int i = 0; i < urls.size(); ++i)
    {
        KIPIPlugins::KPImageInfo info(m_interface, urls.at(i));

        QMap<QString, QString> map;

        map["url"]         = urls.at(i).url();
        map["licence"]     = licence;
        map["author"]      = author;
        map["description"] = description;
        map["time"]        = info.date().toString(Qt::ISODate);

        if(info.hasGeolocationInfo())
        {
            map["latitude"]  = QString::number(info.latitude());
            map["longitude"] = QString::number(info.longitude());
            map["altitude"]  = QString::number(info.altitude());
        }

        imageDesc << map;
    }

    m_uploadJob->setImageMap(imageDesc);

    m_widget->progressBar()->setRange(0, 100);
    m_widget->progressBar()->setValue(0);

    connect(m_uploadJob, SIGNAL(uploadProgress(int)),
            m_widget->progressBar(), SLOT(setValue(int)));

    connect(m_uploadJob, SIGNAL(endUpload()),
            this, SLOT(slotEndUpload()));

    m_widget->progressBar()->show();
    m_uploadJob->begin();
}

void WMWindow::slotChangeUserClicked()
{
    enableButton(User1, false);
    m_widget->invertAccountLoginBox();
}

void WMWindow::slotDoLogin(const QString& login, const QString& pass, const QUrl& wiki)
{
    m_login                    = login;
    m_pass                     = pass;
    m_wiki                     = wiki;
    m_mediawiki                = new mediawiki::MediaWiki(wiki);
    mediawiki::Login* loginJob = new mediawiki::Login(*m_mediawiki, login, pass);
    connect(loginJob, SIGNAL(result(KJob*)), 
            this, SLOT(loginHandle(KJob*)));
    loginJob->start();
}

int WMWindow::loginHandle(KJob* loginJob)
{
    kDebug()<< loginJob->error();

    if(loginJob->error())
    {
        m_login.clear();
        m_pass.clear();
        m_uploadJob = NULL;
        //TODO Message d'erreur de login
        KMessageBox::error(this, i18n("Login error\nPlease check your credentials and try again."));
    }
    else
    {
        m_uploadJob = new KIPIWikiMediaPlugin::WikiMediaJob(m_interface, m_mediawiki, this);
        enableButton(User1, true);
        m_widget->invertAccountLoginBox();
        m_widget->updateLabels(m_login, m_wiki.toString());
    }
    return loginJob->error();
}

void WMWindow::slotEndUpload()
{
    disconnect(m_uploadJob, SIGNAL(uploadProgress(int)),
               m_widget->progressBar(),SLOT(setValue(int)));
    disconnect(m_uploadJob, SIGNAL(endUpload()),
               this, SLOT(slotEndUpload()));
 
    KMessageBox::information(this, i18n("Upload finished with no errors."));
    m_widget->progressBar()->hide();
    hide();
}

} // namespace KIPIWikiMediaPlugin
