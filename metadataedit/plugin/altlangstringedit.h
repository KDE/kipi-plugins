/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-10-18
 * Description : a widget to edit a tag with multiple alternative
 *               language string entries.
 *
 * Copyright (C) 2007-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef ALTLANGSTRINGEDIT_H
#define ALTLANGSTRINGEDIT_H

// Qt includes

#include <QWidget>

// Local includes

#include "kpmetadata.h"

using namespace KIPIPlugins;

namespace KIPIMetadataEditPlugin
{

class AltLangStringsEdit : public QWidget
{
    Q_OBJECT

public:

    AltLangStringsEdit(QWidget* const parent, const QString& title, const QString& desc);
    ~AltLangStringsEdit();

    void setValues(const KPMetadata::AltLangMap& values);
    bool getValues(KPMetadata::AltLangMap& oldValues, KPMetadata::AltLangMap& newValues);

    void setValid(bool v);
    bool isValid() const;

    QString defaultAltLang() const;
    bool    asDefaultAltLang() const;

Q_SIGNALS:

    void signalToggled(bool);
    void signalModified();
    void signalDefaultLanguageEnabled(bool);

private Q_SLOTS:

    void slotSelectionChanged(const QString& lang);

private:

    class AltLangStringsEditPriv;
    AltLangStringsEditPriv* const d;
};

}  // namespace KIPIMetadataEditPlugin

#endif // ALTLANGSTRINGEDIT_H
