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

// Local includes

#include "metadataedit_global.h"

class QCheckBox;
class QLabel;
class QPushButton;
class QRadioButton;

class KComboBox;
class KLineEdit;
class KListWidget;

namespace KIPIMetadataEditPlugin
{

class SubjectsPriv
{
public:

    enum EditionMode
    {
        STANDARD = 0,
        CUSTOM
    };

    SubjectsPriv()
    {
        addSubjectButton = 0;
        delSubjectButton = 0;
        repSubjectButton = 0;
        subjectsBox      = 0;
        subjectsCheck    = 0;
        iprEdit          = 0;
        refEdit          = 0;
        nameEdit         = 0;
        matterEdit       = 0;
        detailEdit       = 0;
        iprLabel         = 0;
        refLabel         = 0;
        nameLabel        = 0;
        matterLabel      = 0;
        detailLabel      = 0;
        note             = 0;
        btnGroup         = 0;
        stdBtn           = 0;
        customBtn        = 0;
        refCB            = 0;
        optionsBox       = 0;
    }

    typedef QMap<QString, SubjectData>  SubjectCodesMap;

    SubjectCodesMap                     subMap;

    QStringList                         oldSubjects;

    QWidget                            *optionsBox;

    QPushButton                        *addSubjectButton;
    QPushButton                        *delSubjectButton;
    QPushButton                        *repSubjectButton;

    QCheckBox                          *subjectsCheck;

    QLabel                             *iprLabel;
    QLabel                             *refLabel;
    QLabel                             *nameLabel;
    QLabel                             *matterLabel;
    QLabel                             *detailLabel;
    QLabel                             *note;

    QButtonGroup                       *btnGroup;

    QRadioButton                       *stdBtn;
    QRadioButton                       *customBtn;

    KComboBox                          *refCB;

    KLineEdit                          *iprEdit;
    KLineEdit                          *refEdit;
    KLineEdit                          *nameEdit;
    KLineEdit                          *matterEdit;
    KLineEdit                          *detailEdit;

    KListWidget                        *subjectsBox;
};

class Subjects : public QWidget
{
    Q_OBJECT

public:

    Subjects(QWidget* parent);
    ~Subjects();

    virtual void applyMetadata(QByteArray& metadata) = 0;
    virtual void readMetadata(QByteArray& metadata) = 0;

signals:

    void signalModified();

protected slots:

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

    SubjectsPriv* const d;
};

}  // namespace KIPIMetadataEditPlugin

#endif // SUBJECTS_H
