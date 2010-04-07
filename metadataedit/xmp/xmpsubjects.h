/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-15
 * Description : XMP subjects settings page.
 *
 * Copyright (C) 2006-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef XMP_SUBJECTS_H
#define XMP_SUBJECTS_H

// Qt includes

#include <QWidget>
#include <QByteArray>

// LibKExiv2 includes

#include <libkexiv2/subjectwidget.h>

using namespace KExiv2Iface;

namespace KIPIMetadataEditPlugin
{

class XMPSubjects : public SubjectWidget
{
    Q_OBJECT

public:

    XMPSubjects(QWidget* parent);
    ~XMPSubjects();

    void applyMetadata(QByteArray& xmpData);
    void readMetadata(QByteArray& xmpData);
};

}  // namespace KIPIMetadataEditPlugin

#endif // XMP_SUBJECTS_H
