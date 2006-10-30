/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-15
 * Description : IPTC subjects settings page.
 * 
 * Copyright 2006 by Gilles Caulier
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#ifndef IPTC_SUBJECTS_H
#define IPTC_SUBJECTS_H

// Qt includes.

#include <qwidget.h>
#include <qcstring.h>

namespace KIPIMetadataEditPlugin
{

class IPTCSubjectsPriv;

class IPTCSubjects : public QWidget
{
    Q_OBJECT
    
public:

    IPTCSubjects(QWidget* parent, QByteArray& iptcData);
    ~IPTCSubjects();

    void applyMetadata(QByteArray& iptcData);

private:

    void readMetadata(QByteArray& iptcData);

private slots:

    void slotSubjectSelectionChanged();
    void slotAddSubject();
    void slotDelSubject();

private:

    IPTCSubjectsPriv* d;
};

}  // namespace KIPIMetadataEditPlugin

#endif // IPTC_SUBJECTS_H 
