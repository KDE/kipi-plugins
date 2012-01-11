/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-12-23
 * Description : Autodetect binary program and version
 *
 * Copyright (C) 2009-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "binaryiface.h"


// Qt includes

#include <QProcess>
#include <QMessageBox>

// KDE includes

#include <kapplication.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kdebug.h>
#include <klocale.h>
#include <kvbox.h>

namespace KIPIPlugins
{

BinaryIface::BinaryIface()
    : m_available(false), m_isFound(false), m_developmentVersion(false), m_version("N/A"),
    m_pathToBinary("Not Set"), m_binaryBaseName("Not Set"), m_minimalVersion("Not Set"),
    m_configGroup("BinaryIFace Settings")
{
    m_searchPaths.clear();
    constructPathWidget();
}

BinaryIface::~BinaryIface()
{
}

bool BinaryIface::showResults() const
{
    if (!isAvailable() || !versionIsRight())
    {
        KMessageBox::information(
                kapp->activeWindow(),
                i18n("<qt><p>Unable to find <i>%1</i> executable.</p>"
                     "<p>This program is required to continue.<br/>"
                     "Please install it from <b>%2</b> package provided by your distributor<br/>"
                     "or download and install <a href=\"%3\">the source</a>.</p>"
                     "<p>Note: at least, <i>%4</i> version <b>%5</b> is required.</p></qt>",
                     path(),
                     projectName(),
                     url().url(),
                     path(),
                     minimalVersion()),
                QString(),
                QString(),
                KMessageBox::Notify | KMessageBox::AllowLink);

        return false;
    }
    return true;
}

bool BinaryIface::isAvailable() const
{
    return m_available;
}

QString BinaryIface::version() const
{
    return m_version;
}

bool BinaryIface::versionIsRight() const
{
    if (version().isNull() || !isAvailable())
        return false;

    if (version().toFloat() >= minimalVersion().toFloat())
        return true;

    return false;
}

QString BinaryIface::findHeader(const QStringList& output, const QString& header) const
{
    foreach(const QString& s, output)
    {
        if (s.startsWith(header))
            return s;
    }
    return QString();
}

void BinaryIface::setBinaryFound(bool f)
{
    if (f)
    {
        if (developmentVersion())
        {
            m_statusIcon->setPixmap(SmallIcon("dialog-warning"));
        }
        else
        {
            m_statusIcon->setPixmap(SmallIcon("dialog-ok-apply"));
        }
        m_isFound = true;
        m_pathButton->hide();
        m_downloadButton->hide();
    }
    else
    {
        m_statusIcon->setPixmap(SmallIcon("dialog-cancel"));
        m_isFound = false;
        m_pathButton->show();
        m_downloadButton->show();
    }
    emit signalBinaryValid(m_isFound);
}

QString BinaryIface::slotNavigateToBinary()
{
    QString f = KFileDialog::getOpenFileName(KUrl(),
                                             QString(m_binaryBaseName),
                                             0,
                                             QString(i18n("Navigate to %1",m_binaryBaseName)));
    m_pathToBinary = f;
    if (checkPath(m_pathToBinary))
    {
        QDir d(m_pathToBinary);
        int lastIndex = m_pathToBinary.lastIndexOf(QDir::separator());
        emit signalSearchDirectoryAdded( m_pathToBinary.left(lastIndex + 1) );
    }
    else
    {
        setBinaryFound(false);
    }
    return f;
}

void BinaryIface::slotAddSearchDirectory(const QString& dir)
{
    if (!m_searchPaths.contains(dir))
    {
        m_searchPaths << dir;
        checkSystem();
    }
}

QWidget * BinaryIface::constructPathWidget()
{
    m_pathWidget        = new KHBox();
    m_pathWidget->setContentsMargins(0,0,0,0);
    m_statusIcon        = new QLabel(m_pathWidget);
    m_binaryLabel       = new QLabel(m_pathWidget);
    m_versionLabel      = new QLabel(m_pathWidget);
    m_pathButton        = new QPushButton(m_pathWidget);
    m_downloadButton    = new QLabel(m_pathWidget);

    connect(m_pathButton, SIGNAL(clicked()), this, SLOT(slotNavigateAndCheck()));

    return m_pathWidget;
}

QWidget * BinaryIface::binaryFileStatusWidget(QWidget* p, QGridLayout* l, int r)
{
    checkSystem();
    m_pathWidget->setParent(p);
    m_binaryLabel->setText(baseName());
    m_versionLabel->setText(i18n("version: %1", version()));
    m_pathButton->setText(i18n("Find"));
    m_pathButton->hide();
    m_downloadButton->setText(i18n(" or <a href=\"%1\">download</a>", url().url()));
    m_downloadButton->setMargin(5);
    m_downloadButton->hide();
    setBinaryFound(m_isFound);

    if (l != NULL)
    {
        l->addWidget(m_statusIcon, r, 0);
        l->addWidget(m_binaryLabel, r, 2);
        l->addWidget(m_versionLabel, r, 3);
        l->addWidget(m_pathButton, r, 4);
        l->addWidget(m_downloadButton, r, 5);
    }

    return m_pathWidget;
}

void BinaryIface::readConfig()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(m_configGroup);
    m_pathToBinary = group.readPathEntry(QString("%1Binary").arg(m_binaryBaseName), m_binaryBaseName);
}

void BinaryIface::writeConfig()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(m_configGroup);
    group.writePathEntry(QString("%1Binary").arg(m_binaryBaseName), path());
}

bool BinaryIface::checkPath(const QString& possiblePath)
{
    bool ret = false;
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(possiblePath, m_versionArguments);
    m_available = process.waitForFinished();

    if (process.error() == QProcess::FailedToStart)
    {
        m_isFound = false;
    }
    else
    {
        m_pathToBinary = possiblePath;
        m_isFound = true;

        QString stdOut(process.readAllStandardOutput());
        if (parseHeader(stdOut))
        {
            kDebug() << "Found " << path() << " version: " << version();
            setBinaryFound(true);
            writeConfig();
            ret = true;
            m_isFound = true;
        }
    }
    emit signalBinaryValid(m_isFound);
    return ret;
}

bool BinaryIface::checkSystem(const QStringList& /* binaryArgs */)
{
    bool ret = false;
    // first check the PATH, then check m_searchPaths
    if (checkPath(baseName()))
    {
        ret = true;
    }
    else
    {
        foreach(const QString& dir, m_searchPaths)
        {
            QString testPath = QString("%1%2%3").arg(dir).arg(QDir::separator()).arg(baseName());
            if (checkPath(testPath))
            {
                ret = true;
                break;
            }
        }
    }
    m_versionLabel->setText(i18n("version: %1", version()));
    return ret;
}

}  // namespace KIPIPlugins
