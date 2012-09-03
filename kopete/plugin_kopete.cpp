/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-12-04
 * Description : a kipi plugin to export images to a Kopete contact
 *
 * Copyright (C) 2010 by Pau Garcia i Quiles <pgquiles at elpauer dot org>
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

// To disable warnings under MSVC2008 about POSIX methods().
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include "plugin_kopete.moc"

// C++ includes

#include <unistd.h>

// Qt includes

#include <QDBusInterface>
#include <QDBusReply>
#include <QSignalMapper>
#include <QVariant>

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kgenericfactory.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <kmenu.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

namespace KIPIKopetePlugin
{

K_PLUGIN_FACTORY( KopeteFactory, registerPlugin<Plugin_Kopete>(); )
K_EXPORT_PLUGIN ( KopeteFactory("kipiplugin_kopete") )

Plugin_Kopete::Plugin_Kopete(QObject* const parent, const QVariantList& /*args*/)
    : Plugin(KopeteFactory::componentData(),
      parent, "Kopete Export"),
      m_kopeteDBus("org.kde.kopete","/Kopete","org.kde.Kopete"),
      m_kopeteDBusTest("org.kde.kopete", "/Kopete", "org.freedesktop.DBus.Introspectable")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_Kopete plugin loaded";

    setUiBaseName("kipiplugin_kopeteui.rc");
    setupXML();
}

Plugin_Kopete::~Plugin_Kopete()
{
}

void Plugin_Kopete::setup(QWidget* const widget)
{
    Plugin::setup(widget);

    KIconLoader::global()->addAppDir("kipiplugin_kopete");

    setDefaultCategory(ExportPlugin);

    m_actionExport = new KAction(this);
    m_actionExport->setText(i18n("&Instant Messaging contact..."));
    m_actionExport->setIcon(KIcon("kopete"));
    m_actionExport->setEnabled(false);

    addAction("kopeteexport", m_actionExport);

    m_signalMapper = new QSignalMapper(widget);
    connect(m_signalMapper, SIGNAL(mapped(QString)),
            this, SLOT(slotTransferFiles(QString)));

    KMenu* contactsMenu = new KMenu(widget);
    m_actionExport->setMenu(contactsMenu);
    connect(contactsMenu, SIGNAL(aboutToShow()),
            this, SLOT(slotAboutToShowMenu()));

    if (!interface())
    {
        kError() << "Kipi interface is null!";
        return;
    }

    m_actionExport->setEnabled(true);
}

void Plugin_Kopete::slotAboutToShowMenu()
{
    if (!interface())
    {
        kError() << "Kipi interface is null!";
        return;
    }

    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", QString("kipi-kopete-") + QString::number(getpid()) + QString("/"));

    m_actionExport->menu()->clear();

    if (!kopeteRunning())
    {
        QAction* action = m_actionExport->menu()->addAction(QIcon::fromTheme("network-offline"), i18n("Please start Kopete to enable this action"), m_signalMapper, SLOT(map()));
        m_signalMapper->setMapping(action, QString());
        return;
    }

    QDBusReply<QStringList> kopeteContacts = m_kopeteDBus.call("contacts");
    if( (!kopeteContacts.isValid()) || (kopeteContacts.value().size() == 0) )
    {
        QAction* action = m_actionExport->menu()->addAction(QIcon::fromTheme("user-offline"), i18n("None of your contacts are online now"), m_signalMapper, SLOT(map()));
        m_signalMapper->setMapping(action, QString());
        return;
    }

    foreach(const QString &contact, kopeteContacts.value())
    {
        // TODO? Check status using contactProperties.value().value("status", QString())
        // and Kopete::OnlineStatus::statusTypeToString() ?
        // - Advantage: one less DBUS call per contact
        // - Disadvantage: dependency on Kopete sources. Using isContactOnline makes code totally
        //   independent from Kopete sources, so not changing the way the check is performed for now.
        QDBusReply<bool> online = m_kopeteDBus.call("isContactOnline", QVariant::fromValue(contact));
        if( !online.isValid() || !online.value() )
        {
            continue;
        }


        QDBusReply<QVariantMap> contactProperties = m_kopeteDBus.call("contactProperties", QVariant::fromValue(contact));
        if( ( !contactProperties.isValid() ) || ( contactProperties.value().size() == 0 ) )
        {
            continue;
        }

        bool canAcceptFiles = contactProperties.value().value("file_reachable", false).toBool();
        kDebug() << "Contact " << contact << (canAcceptFiles ? "can" : "cannot") << " accept files via IM";
        if(!canAcceptFiles)
        {
            continue;
        }

        // Get display name for contact
        QString displayName = contactProperties.value().value("display_name", QString()).toString();
        if(displayName.isEmpty())
        {
            continue;
        }

        // Add contact with avatar, if possible
        QString avatar  = contactProperties.value().value("picture", QString()).toString();
        QAction* action = m_actionExport->menu()->addAction(QIcon(avatar), displayName, m_signalMapper, SLOT(map()));

        m_signalMapper->setMapping(action, contact);
    }
}

void Plugin_Kopete::slotTransferFiles(const QString& contactId)
{
    kDebug() << "Received a request to transfer file(s) to contact " << contactId;

    KUrl::List imgList   = interface()->currentSelection().images();

    // Check if Kopete is still running
    if ( contactId.isEmpty() || !kopeteRunning() )
    {
        // TODO Show KMessageBox::critical or alike
          return;
    }

    // Check if contact is still online
    // TODO Connect to Kopete's contactChanged signal to add/remove contacts dynamically
    QDBusReply<bool> online = m_kopeteDBus.call("isContactOnline", QVariant::fromValue(contactId));
    if( !online.isValid() || !online.value() )
    {
        // TODO Show KMessageBox::critical or alike
        return;
    }

    foreach(const KUrl& imgUrl, imgList)
    {
        kDebug() << "Sending file " << imgUrl.toLocalFile();
        m_kopeteDBus.call("sendFile", QVariant::fromValue(contactId), QVariant::fromValue(imgUrl.toLocalFile()));
    }
}

bool Plugin_Kopete::kopeteRunning()
{
    QDBusReply<QString> kopeteReply = m_kopeteDBusTest.call("Introspect");
    if (kopeteReply.isValid())
    {
          return true;
    }
    return false;
}

} // namespace KIPIKopetePlugin
