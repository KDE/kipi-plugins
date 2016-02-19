/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-12-23
 * Description : Autodetect binary program and version
 *
 * Copyright (C) 2009-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#include "kpbinaryiface.h"

// Qt includes

#include <QProcess>
#include <QMessageBox>
#include <QFileDialog>

// KDE includes

#include <klocalizedstring.h>
#include <kconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "kipiplugins_debug.h"

namespace KIPIPlugins
{

KPBinaryIface::KPBinaryIface(const QString& binaryName, const QString& projectName, const QString& url,
                             const QString& pluginName, const QStringList& args)
    : m_checkVersion(false),
      m_headerStarts(QLatin1String("")),
      m_headerLine(0),
      m_minimalVersion(QLatin1String("")),
      m_configGroup(pluginName + QLatin1String(" Settings")),
      m_binaryBaseName(goodBaseName(binaryName)),
      m_binaryArguments(args),
      m_projectName(projectName),
      m_url(QUrl(url)),
      m_isFound(false),
      m_developmentVersion(false),
      m_version(QLatin1String("")),
      m_pathDir(QLatin1String("")),
      m_pathWidget(0),
      m_binaryLabel(0),
      m_versionLabel(0),
      m_pathButton(0),
      m_downloadButton(0),
      m_lineEdit(0),
      m_statusIcon(0)
{
}

KPBinaryIface::KPBinaryIface(const QString& binaryName, const QString& minimalVersion, const QString& header,
                             const int headerLine, const QString& projectName, const QString& url,
                             const QString& pluginName, const QStringList& args)
    : m_checkVersion(true),
      m_headerStarts(header),
      m_headerLine(headerLine),
      m_minimalVersion(minimalVersion),
      m_configGroup(pluginName + QLatin1String(" Settings")), 
      m_binaryBaseName(goodBaseName(binaryName)),
      m_binaryArguments(args), 
      m_projectName(projectName), 
      m_url(QUrl(url)),
      m_isFound(false), 
      m_developmentVersion(false),
      m_version(QLatin1String("")), 
      m_pathDir(QLatin1String("")),
      m_pathWidget(0),
      m_binaryLabel(0),
      m_versionLabel(0),
      m_pathButton(0),
      m_downloadButton(0),
      m_lineEdit(0),
      m_statusIcon(0)
{
}

KPBinaryIface::~KPBinaryIface()
{
}

const QString& KPBinaryIface::version() const
{
    return m_version;
}

bool KPBinaryIface::versionIsRight() const
{
    if (!m_checkVersion)
        return true;

    QRegExp reg(QLatin1String("^(\\d*[.]\\d*)"));
    version().indexOf(reg);
    float floatVersion = reg.capturedTexts()[0].toFloat();

    return (!version().isNull() &&
            isFound()           &&
            floatVersion >= minimalVersion().toFloat());
}

QString KPBinaryIface::findHeader(const QStringList& output, const QString& header) const
{
    foreach(const QString& s, output)
    {
        if (s.startsWith(header))
            return s;
    }

    return QString();
}

bool KPBinaryIface::parseHeader(const QString& output)
{
    QString firstLine = output.section(QLatin1Char('\n'), m_headerLine, m_headerLine);
    qCDebug(KIPIPLUGINS_LOG) << path() << " help header line: \n" << firstLine;

    if (firstLine.startsWith(m_headerStarts))
    {
        QString version = firstLine.remove(0, m_headerStarts.length());

        if (version.startsWith(QLatin1String("Pre-Release ")))
        {
            version.remove(QLatin1String("Pre-Release "));            // Special case with Hugin beta.
            m_developmentVersion = true;
        }

        setVersion(version);
        return true;
    }

    return false;
}

void KPBinaryIface::setVersion(QString& version)
{
    QRegExp versionRegExp(QLatin1String("\\d*(\\.\\d+)*"));
    version.indexOf(versionRegExp);
    m_version = versionRegExp.capturedTexts()[0];
}

void KPBinaryIface::slotNavigateAndCheck()
{
    QUrl start;

    if (isValid() && !m_pathDir.isEmpty())
    {
        start = QUrl(m_pathDir);
    }
    else
    {
#if defined Q_OS_MAC
        start = QUrl(QLatin1String("/Applications/"));
#elif defined Q_OS_WIN
        start = QUrl(QLatin1String("C:/Program Files/"));
#else
        start = QUrl(QLatin1String("/usr/bin/"));
#endif
    }

    QString f = QFileDialog::getOpenFileName(0, i18n("Navigate to %1", m_binaryBaseName),
                                             start.path(),
                                             m_binaryBaseName);

    QString dir = QUrl(f).adjusted(QUrl::RemoveFilename).path();
    m_searchPaths << dir;

    if (checkDir(dir))
    {
        emit signalSearchDirectoryAdded(dir);
    }
}

void KPBinaryIface::slotAddPossibleSearchDirectory(const QString& dir)
{
    if (!isValid())
    {
        m_searchPaths << dir;
        checkDir(dir);
    }
    else
    {
        m_searchPaths << dir;
    }
}

void KPBinaryIface::slotAddSearchDirectory(const QString& dir)
{
    m_searchPaths << dir;
    checkDir(dir);       // Forces the use of that directory
}

QString KPBinaryIface::readConfig()
{
    KConfig config(QLatin1String("kipirc"));
    KConfigGroup group = config.group(m_configGroup);
    return group.readPathEntry(QString::fromUtf8("%1Binary").arg(m_binaryBaseName), QLatin1String(""));
}

void KPBinaryIface::writeConfig()
{
    KConfig config(QLatin1String("kipirc"));
    KConfigGroup group = config.group(m_configGroup);
    group.writePathEntry(QString::fromUtf8("%1Binary").arg(m_binaryBaseName), m_pathDir);
}

QString KPBinaryIface::path(const QString& dir) const
{
    if (dir.isEmpty())
    {
        return baseName();
    }

    return QString::fromUtf8("%1%2%3").arg(dir).arg(QLatin1Char('/')).arg(baseName());
}

void KPBinaryIface::setup()
{
    QString previous_dir = readConfig();
    m_searchPaths << previous_dir;
    checkDir(previous_dir);

    if ((!previous_dir.isEmpty()) && !isValid())
    {
        m_searchPaths << QLatin1String("");
        checkDir(QLatin1String(""));
    }
}

bool KPBinaryIface::checkDir(const QString& possibleDir)
{
    bool ret             = false;
    QString possiblePath = path(possibleDir);

    qCDebug(KIPIPLUGINS_LOG) << "Testing " << possiblePath << "...";
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(possiblePath, m_binaryArguments);
    bool val = process.waitForFinished();

    if (val && (process.error() != QProcess::FailedToStart))
    {
        m_isFound = true;

        if (m_checkVersion)
        {
            QString stdOut = QString::fromUtf8(process.readAllStandardOutput());

            if (parseHeader(stdOut))
            {
                m_pathDir = possibleDir;
                writeConfig();

                qCDebug(KIPIPLUGINS_LOG) << "Found " << path() << " version: " << version();
                ret = true;
            }
            else
            {
                // TODO: do something if the version is not right or not found
            }
        }
        else
        {
            m_pathDir = possibleDir;
            writeConfig();

            qCDebug(KIPIPLUGINS_LOG) << "Found " << path();
            ret = true;
        }
    }

    emit signalBinaryValid();
    return ret;
}

bool KPBinaryIface::recheckDirectories()
{
    if (isValid())
    {
        // No need for recheck if it is already valid...
        return true;
    }

    foreach(const QString& dir, m_searchPaths)
    {
        checkDir(dir);

        if (isValid())
        {
            return true;
        }
    }

    return false;
}

}  // namespace KIPIPlugins
