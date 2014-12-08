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

#ifndef THEME_H
#define THEME_H   

// Qt
#include <QString>
#include <QList>

// KDE
#include <ksharedptr.h>

namespace KIPIHTMLExport
{

class AbstractThemeParameter;


/**
 * An HTML theme. This class gives access to the various theme properties,
 * including the theme parameters.
 */
class Theme : public KShared
{
public:

    typedef KSharedPtr<Theme> Ptr;
    typedef QList<Ptr> List;
    typedef QList<AbstractThemeParameter*> ParameterList;

    ~Theme();
    /**
     * Internal theme name == name of theme folder
     */
    QString internalName() const;
    QString name() const;
    QString comment() const;

    QString authorName() const;
    QString authorUrl() const;

    QString previewName() const;
    QString previewUrl() const;
    bool allowNonsquareThumbnails() const;

    /**
     * Theme directory on hard disk
     */
    QString directory() const;

    ParameterList parameterList() const;

    /**
     * Returns the list of available themes
     */
    static const List& getList();
    static Ptr findByInternalName(const QString& internalName);

private:

    Theme();

    struct Private;
    Private* d;
};

} // namespace

#endif /* THEME_H */
