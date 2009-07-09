/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-15
 * Description : common subjects settings page.
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

#ifndef SUBJECTS_H
#define SUBJECTS_H

// Qt includes

#include <QButtonGroup>
#include <QByteArray>
#include <QMap>
#include <QStringList>
#include <QWidget>

// KDE includes

#include <kurl.h>

class QCheckBox;
class QLabel;

class KLineEdit;

namespace KIPIMetadataEditPlugin
{

class SubjectsPriv;

class SubjectData
{
public:

    SubjectData(const QString& n, const QString& m, const QString& d)
    {
        name   = n;
        matter = m;
        detail = d;
    }

    QString name;         // English and Ascii Name of subject.
    QString matter;       // English and Ascii Matter Name of subject.
    QString detail;       // English and Ascii Detail Name of subject.
};

// --------------------------------------------------------------------------------

class Subjects : public QWidget
{
    Q_OBJECT

public:

    Subjects(QWidget* parent);
    ~Subjects();

    void setSubjectsList(const QStringList& list);
    QStringList subjectsList() const;

    virtual void applyMetadata(QByteArray& metadata) = 0;
    virtual void readMetadata(QByteArray& metadata) = 0;

Q_SIGNALS:

    void signalModified();

protected Q_SLOTS:

    virtual void slotSubjectsToggled(bool);
    virtual void slotRefChanged();
    virtual void slotEditOptionChanged(int);
    virtual void slotSubjectSelectionChanged();
    virtual void slotAddSubject();
    virtual void slotDelSubject();
    virtual void slotRepSubject();

protected:

    virtual bool loadSubjectCodesFromXML(const KUrl& url);
    virtual QString buildSubject() const;

protected:

    QLabel    *m_note;

    QCheckBox *m_subjectsCheck;

    KLineEdit *m_iprEdit;
    KLineEdit *m_refEdit;
    KLineEdit *m_nameEdit;
    KLineEdit *m_matterEdit;
    KLineEdit *m_detailEdit;

private:

    SubjectsPriv* const d;
};

}  // namespace KIPIMetadataEditPlugin

#endif // SUBJECTS_H
