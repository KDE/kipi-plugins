/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
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

#ifndef BINARYIFACE_H
#define BINARYIFACE_H

// Qt includes

#include <QString>
#include <QStringList>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSet>
#include <QGridLayout>

// KDE includes

#include <kurl.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "kipiplugins_export.h"

#include <QDebug>

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT BinaryIface : public QObject
{
Q_OBJECT;

public:

    BinaryIface();
    virtual ~BinaryIface();

    bool                isAvailable()           const;
    QString             version()               const;
    bool                versionIsRight()        const;
    bool                showResults()           const;
    bool                isValid()               const { return (m_isFound && versionIsRight()); }
    bool                developmentVersion()    const { return m_developmentVersion; }

    virtual bool        checkSystem(const QStringList& binaryArgs);
    virtual bool        checkSystem()                 { return checkSystem(m_versionArguments); }
    virtual bool        checkPath(const QString& path);

    virtual QString     path()                  const { return m_pathToBinary; }
    virtual QString     baseName()              const { return m_binaryBaseName; }
    virtual QString     minimalVersion()        const { return m_minimalVersion; }

    virtual void        setPath(const QString& p)          { m_pathToBinary = p; }
    virtual void        setMinimalVersion(const QString& m){ m_minimalVersion = m; }
    virtual void        setConfigGroup(const QString& g)   { m_configGroup = g; }
    virtual void        setBaseName(const QString& b)      {
                            #ifdef Q_WS_WIN
                                m_binaryBaseName = b+".exe";
                            #else
                                m_binaryBaseName = b;
                            #endif // Q_WS_WIN
                        }

    virtual KUrl        url() const = 0;
    virtual QString     projectName() const = 0;
    virtual QWidget*    binaryFileStatusWidget(QWidget* p, QGridLayout* l, int r);

public Q_SLOTS:
    QString             slotNavigateToBinary();
    virtual void        slotNavigateAndCheck() { slotNavigateToBinary(); checkSystem(); }
    virtual void        slotAddSearchDirectory(const QString& dir);

Q_SIGNALS:
    void                signalSearchDirectoryAdded(const QString& dir);
    void                signalBinaryValid(bool);

protected:
    virtual QWidget*    constructPathWidget();
    virtual void        setBinaryFound(bool f = true);
    QString             findHeader(const QStringList& output, const QString& header) const;
    virtual bool        parseHeader(const QString& output) = 0;

    virtual void        readConfig();
    virtual void        writeConfig();

protected:

    bool            m_available;
    bool            m_isFound;
    bool            m_developmentVersion;

    QString         m_version;
    QString         m_pathToBinary;
    QString         m_binaryBaseName;
    QString         m_headerStarts;
    QString         m_minimalVersion;
    QString         m_configGroup;
    QStringList     m_versionArguments;

    QFrame          *m_pathWidget;
    QLabel          *m_binaryLabel;
    QLabel          *m_versionLabel;
    QPushButton     *m_pathButton;
    QLabel          *m_downloadButton;
    QLineEdit       *m_lineEdit;
    QLabel          *m_statusIcon;

    QSet<QString>   m_searchPaths;
};

} // namespace KIPIPlugins

#endif  // BINARYIFACE_H
