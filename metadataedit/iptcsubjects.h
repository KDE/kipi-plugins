/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-15
 * Description : IPTC subjects settings page.
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at gmx dot net>
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

#ifndef IPTC_SUBJECTS_H
#define IPTC_SUBJECTS_H

// Qt includes

#include <QWidget>
#include <QByteArray>

// Local includes

#include "subjects.h"

namespace KIPIMetadataEditPlugin
{

class IPTCSubjects : public Subjects
{
    Q_OBJECT

public:

    IPTCSubjects(QWidget* parent);
    ~IPTCSubjects();

    void applyMetadata(QByteArray& iptcData);
    void readMetadata(QByteArray& iptcData);
};

}  // namespace KIPIMetadataEditPlugin

#endif // IPTC_SUBJECTS_H
