/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-12-23
 * Description : Autodetect binary program and version
 *
 * Copyright (C) 2009-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "kpbinaryiface.moc"

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

KPBinaryIface::KPBinaryIface(const QString& binaryName, const QString& minimalVersion, const QString& header,
                             const int headerLine, const QString& projectName, const QString& url,
                             const QString& pluginName, const QStringList& args)
    : m_headerStarts(header),
      m_headerLine(headerLine),
      m_minimalVersion(minimalVersion),
      m_configGroup(pluginName + " Settings"), 
      m_binaryBaseName(goodBaseName(binaryName)),
      m_binaryArguments(args), 
      m_projectName(projectName), 
      m_url(KUrl(url)),
      m_isFound(false), 
      m_developmentVersion(false),
      m_version(""), 
      m_pathDir(""),
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
    QRegExp reg("^(\\d*[.]\\d*)");
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
    QString firstLine = output.section('\n', m_headerLine, m_headerLine);
    kDebug() << path() << " help header line: \n" << firstLine;

    if (firstLine.startsWith(m_headerStarts))
    {
        QString version = firstLine.remove(0, m_headerStarts.length());

        if (version.startsWith(QLatin1String("Pre-Release ")))
        {
            version.remove("Pre-Release ");            // Special case with Hugin beta.
            m_developmentVersion = true;
        }

        setVersion(version);
        return true;
    }

    return false;
}

void KPBinaryIface::setVersion(QString& version)
{
    QRegExp versionRegExp("\\d*(\\.\\d+)*");
    version.indexOf(versionRegExp);
    m_version = versionRegExp.capturedTexts()[0];
}

void KPBinaryIface::slotNavigateAndCheck()
{
    KUrl start;

    if (isValid() && !m_pathDir.isEmpty())
    {
        start = KUrl(m_pathDir);
    }
    else
    {
#if defined Q_OS_MAC
        start = KUrl(QString("/Applications/"));
#elif defined Q_OS_WIN
        start = KUrl(QString("C:/Program Files/"));
#else
        start = KUrl(QString("/usr/bin/"));
#endif
    }

    QString f = KFileDialog::getOpenFileName(start,
                                             QString(m_binaryBaseName),
                                             0,
                                             QString(i18n("Navigate to %1", m_binaryBaseName)));
    QString dir = KUrl(f).directory();
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
    KConfig config("kipirc");
    KConfigGroup group = config.group(m_configGroup);
    return group.readPathEntry(QString("%1Binary").arg(m_binaryBaseName), "");
}

void KPBinaryIface::writeConfig()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(m_configGroup);
    group.writePathEntry(QString("%1Binary").arg(m_binaryBaseName), m_pathDir);
}

QString KPBinaryIface::path(const QString& dir) const
{
    if (dir.isEmpty())
    {
        return baseName();
    }

    return QString("%1%2%3").arg(dir).arg('/').arg(baseName());
}

void KPBinaryIface::setup()
{
    QString previous_dir = readConfig();
    m_searchPaths << previous_dir;
    checkDir(previous_dir);

    if ((!previous_dir.isEmpty()) && !isValid())
    {
        m_searchPaths << "";
        checkDir("");
    }
}

bool KPBinaryIface::checkDir(const QString& possibleDir)
{
    bool ret             = false;
    QString possiblePath = path(possibleDir);

    kDebug() << "Testing " << possiblePath << "...";
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(possiblePath, m_binaryArguments);
    bool val = process.waitForFinished();

    if (val && (process.error() != QProcess::FailedToStart))
    {
        m_isFound = true;

        QString stdOut(process.readAllStandardOutput());

        if (parseHeader(stdOut))
        {
            m_pathDir = possibleDir;
            writeConfig();

            kDebug() << "Found " << path() << " version: " << version();
            ret = true;
        }
        else
        {
            // TODO: do something if the version is not right or not found
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

    foreach(QString dir, m_searchPaths)
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
