/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-04-04
 * Description : A KIPI plugin to generate HTML image galleries
 *
 * Copyright (C) 2006-2010 by Aurelien Gateau <aurelien dot gateau at free.fr>
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

// vim: set tabstop=4 shiftwidth=4 noexpandtab:

// Self

#include "theme.h"

// Qt

#include <QFile>
#include <QTextStream>

// KDE

#include <kconfiggroup.h>
#include <kdebug.h>
#include <kdesktopfile.h>
#include <kstandarddirs.h>
#include <kurl.h>

// Local

#include "colorthemeparameter.h"
#include "intthemeparameter.h"
#include "listthemeparameter.h"
#include "stringthemeparameter.h"

namespace KIPIHTMLExport
{

static const char* AUTHOR_GROUP           = "X-HTMLExport Author";
static const char* PARAMETER_GROUP_PREFIX = "X-HTMLExport Parameter ";
static const char* PARAMETER_TYPE_KEY     = "Type";
static const char* PREVIEW_GROUP          = "X-HTMLExport Preview";
static const char* OPTIONS_GROUP          = "X-HTMLExport Options";
static const char* STRING_PARAMETER_TYPE  = "string";
static const char* LIST_PARAMETER_TYPE    = "list";
static const char* COLOR_PARAMETER_TYPE   = "color";
static const char* INT_PARAMETER_TYPE     = "int";

static Theme::List sList;

struct Theme::Private
{
    Private()
        : mDesktopFile(0)
    {
    }

    KDesktopFile* mDesktopFile;
    KUrl          mUrl;
    ParameterList mParameterList;

    /**
     * Return the list of parameters defined in the desktop file. We need to
     * parse the file ourself to preserve parameter order.
     */
    QStringList readParameterNameList(const QString& desktopFileName)
    {
        QStringList list;
        QFile file(desktopFileName);

        if (!file.open(QIODevice::ReadOnly))
        {
            return QStringList();
        }

        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        QString prefix = QString("[") + PARAMETER_GROUP_PREFIX;

        while (!stream.atEnd())
        {
            QString line = stream.readLine();
            line         = line.trimmed();

            if (!line.startsWith(prefix))
            {
                continue;
            }

            // Remove opening bracket and group prefix
            line         = line.mid(prefix.length());

            // Remove closing bracket
            line.truncate(line.length() - 1);

            list.append(line);
        }

        return list;
    }

    void init(const QString& desktopFileName)
    {
        delete mDesktopFile;
        mDesktopFile = new KDesktopFile(desktopFileName);
        mUrl.setPath(desktopFileName);

        QStringList parameterNameList = readParameterNameList(desktopFileName);
        readParameters(parameterNameList);
    }

    void readParameters(const QStringList& list)
    {
        QStringList::ConstIterator it=list.constBegin(), end=list.constEnd();

        for (;it!=end; ++it)
        {
            QString groupName                 = PARAMETER_GROUP_PREFIX + *it;
            QByteArray internalName           = it->toUtf8();
            KConfigGroup group                = mDesktopFile->group(groupName);
            QString type                      = group.readEntry(PARAMETER_TYPE_KEY);
            AbstractThemeParameter* parameter = 0;

            if (type == STRING_PARAMETER_TYPE)
            {
                parameter = new StringThemeParameter();
            }
            else if (type == LIST_PARAMETER_TYPE)
            {
                parameter = new ListThemeParameter();
            }
            else if (type == COLOR_PARAMETER_TYPE)
            {
                parameter = new ColorThemeParameter();
            }
            else if (type == INT_PARAMETER_TYPE)
            {
                parameter = new IntThemeParameter();
            }
            else
            {
                kWarning() << "Parameter '" << internalName << "' has unknown type '" << type << "'. Falling back to string type\n";
                parameter = new StringThemeParameter();
            }

            parameter->init(internalName, &group);
            mParameterList << parameter;
        }
    }
};

Theme::Theme()
{
    d = new Private;
}

Theme::~Theme()
{
    delete d->mDesktopFile;
    delete d;
}

const Theme::List& Theme::getList()
{
    if (sList.isEmpty())
    {
        QStringList internalNameList;
        const QStringList list        = KGlobal::dirs()->findAllResources("data", "kipiplugin_htmlexport/themes/*/*.desktop");
        QStringList::ConstIterator it = list.constBegin(), end=list.constEnd();

        for (;it!=end; ++it)
        {
            Theme* const theme   = new Theme;
            theme->d->init(*it);
            QString internalName = theme->internalName();

            if (!internalNameList.contains(internalName))
            {
                sList << Theme::Ptr(theme);
                internalNameList << internalName;
            }
        }
    }

    return sList;
}

Theme::Ptr Theme::findByInternalName(const QString& internalName)
{
    const Theme::List& lst        = getList();
    Theme::List::ConstIterator it = lst.constBegin(), end=lst.constEnd();

    for (; it!=end; ++it)
    {
        Theme::Ptr theme = *it;

        if (theme->internalName() == internalName)
        {
            return theme;
        }
    }

    return Theme::Ptr(0);
}

QString Theme::internalName() const
{
    KUrl url = d->mUrl;
    url.setFileName("");
    return url.fileName();
}

QString Theme::name() const
{
    return d->mDesktopFile->readName();
}

QString Theme::comment() const
{
    return d->mDesktopFile->readComment();
}

QString Theme::directory() const
{
    return d->mUrl.directory();
}

QString Theme::authorName() const
{
    return d->mDesktopFile->group(AUTHOR_GROUP).readEntry("Name");
}

QString Theme::authorUrl() const
{
    return d->mDesktopFile->group(AUTHOR_GROUP).readEntry("Url");
}

QString Theme::previewName() const
{
        return d->mDesktopFile->group(PREVIEW_GROUP).readEntry("Name");
}

QString Theme::previewUrl() const
{
        return d->mDesktopFile->group(PREVIEW_GROUP).readEntry("Url");
}

bool Theme::allowNonsquareThumbnails() const
{
    return d->mDesktopFile->group(OPTIONS_GROUP).readEntry("Allow non-square thumbnails", false);
}

Theme::ParameterList Theme::parameterList() const
{
    return d->mParameterList;
}

} // namespace KIPIHTMLExport
