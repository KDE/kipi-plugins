/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-10-18
 * Description : a widget to edit a tag with multiple alternative
 *               language string entries.
 *
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef ALTLANG_STRINGS_EDIT_H
#define ALTLANG_STRINGS_EDIT_H

// Qt includes.

#include <QWidget>
#include <QList>

namespace KIPIMetadataEditPlugin
{

class AltLangStringsEditPriv;

class AltLangData
{
public:

    AltLangData(const QString& l, const QString& t)
    {
        lang = l;
        text = t;
    }

    QString lang;  
    QString text;  
};

class AltLangStringsEdit : public QWidget
{
    Q_OBJECT
    
public:

    typedef QList<AltLangData> AltLangDataList; 

public:

    AltLangStringsEdit(QWidget* parent, const QString& title, const QString& desc);
    ~AltLangStringsEdit();

    void setValues(const AltLangDataList& values);
    bool getValues(AltLangDataList& oldValues, AltLangDataList& newValues);

    void setValid(bool v);
    bool isValid() const;

signals:

    void signalToggled(bool);
    void signalModified();

private slots:

    void slotSelectionChanged();
    void slotAddValue();
    void slotDeleteValue();
    void slotReplaceValue();

private:

    AltLangStringsEditPriv* d;
};

}  // namespace KIPIMetadataEditPlugin

#endif // ALTLANG_STRINGS_EDIT_H
