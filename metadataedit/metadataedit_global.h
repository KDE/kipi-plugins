/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-1-06
 * Copyright (C) 2009 by Laurent Montel <montel@kde.org>
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

#ifndef METADATAEDIT_GLOBAL_H
#define METADATAEDIT_GLOBAL_H

namespace KIPIMetadataEditPlugin
{

class SubjectData
{
public:

    SubjectData(const QString& n, const QString& m, const QString& d)
    {
        name        = n;
        matter      = m;
        detail      = d;
    }

    QString name;         // English and Ascii Name of subject.
    QString matter;       // English and Ascii Matter Name of subject.
    QString detail;       // English and Ascii Detail Name of subject.
};
}

#endif /* METADATAEDIT_GLOBAL_H */

