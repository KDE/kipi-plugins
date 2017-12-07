/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-14-09
 * Description : Kipi-Plugins shared library.
 *
 * Copyright (C) 2006-2010 by Angelo Naselli <anaselli at linux dot it>
 * Copyright (C) 2010-2018 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "kpaboutdata.h"

// Qt includes

#include <QUrl>
#include <QUrlQuery>
#include <QDesktopServices>
#include <QIcon>
#include <QAction>
#include <QMenu>
#include <QPushButton>
#include <QStandardPaths>
#include <QMessageBox>
#include <QMenu>
#include <QApplication>

// Local includes

#include "kipiplugins_debug.h"
#include "kpversion.h"

namespace KIPIPlugins
{

KPAboutData::KPAboutData(const KLocalizedString& tool,
                         const KLocalizedString& description,
                         const KLocalizedString& copyright)
    : QObject()
{
    m_tool        = tool.toString();
    m_description = description.toString();
    m_copyright   = copyright.toString();
}

KPAboutData::KPAboutData(const KPAboutData& other)
    : QObject((QObject*)(&other))
{
    m_tool          = other.m_tool;
    m_description   = other.m_description;
    m_copyright     = other.m_copyright;
    m_handbookEntry = other.m_handbookEntry;
}

KPAboutData::~KPAboutData()
{
}

void KPAboutData::setHandbookEntry(const QString& entry)
{
    m_handbookEntry = entry;
}

void KPAboutData::setHelpButton(QPushButton* const help)
{
    QMenu* const menu = new QMenu(help);
    
    QAction* const book = menu->addAction(QIcon::fromTheme(QString::fromLatin1("help-contents")), i18n("Handbook"));

    connect(book, &QAction::triggered,
            this, &KPAboutData::slotHelp);

    QAction* const about = menu->addAction(QIcon::fromTheme(QString::fromLatin1("help-about")), i18n("About..."));

    connect(about, &QAction::triggered,
            this, &KPAboutData::slotAbout);    
   
    help->setMenu(menu);
}

void KPAboutData::addAuthor(const QString& name, const QString& role, const QString& email)
{
    QString mailUrl = email;
    mailUrl.remove(QLatin1String(" "));
    QString data = QString::fromUtf8("%1 <%2>\n%3").arg(name).arg(mailUrl).arg(role);
    m_authors.append(data);
}

void KPAboutData::slotHelp()
{
    QUrl url = QUrl(QString::fromUtf8("help:/%1/index.html").arg(QString::fromLatin1("digikam")));

    if (!m_handbookEntry.isEmpty())
    {
        QUrlQuery query(url);
        query.addQueryItem(QStringLiteral("anchor"), m_handbookEntry);
        url.setQuery(query);
    }

    QDesktopServices::openUrl(url);
}

void KPAboutData::slotAbout()
{
    QString text;
    
    text.append(m_description);
    text.append(QLatin1String("\n\n"));
    text.append(i18n("Version: %1", kipipluginsVersion()));
    text.append(QLatin1String("\n\n"));
    text.append(m_copyright);
    text.append(QLatin1String("\n\n"));
    
    foreach(QString data, m_authors)
    {
        text.append(data);
        text.append(QLatin1String("\n\n"));
    }
    
    text.remove(text.size()-2, 2);

    QMessageBox::about(qApp->activeWindow(), i18n("About %1", m_tool), text);
}

} // namespace KIPIPlugins
