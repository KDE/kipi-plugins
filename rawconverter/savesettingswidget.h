/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-09-13
 * Description : save settings widgets
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

#ifndef SAVESETTINGSWIDGET_H
#define SAVESETTINGSWIDGET_H

// Qt includes.

#include <qgroupbox.h>

// Local includes.

#include "dcrawutils.h"

namespace KIPIRawConverterPlugin
{

class SaveSettingsWidgetPriv;

class SaveSettingsWidget : public QGroupBox
{
    Q_OBJECT
    
public:

    enum ConflictRule 
    {
        OVERWRITE = 0,
        ASKTOUSER
    };
    
public:

    SaveSettingsWidget(QWidget *parent);
    ~SaveSettingsWidget();

    RawDecodingSettings::OutputFormat fileFormat();
    ConflictRule conflictRule();

    void setFileFormat(RawDecodingSettings::OutputFormat f);
    void setConflictRule(ConflictRule r);

signals:

    void signalSaveFormatChanged();

private:

    SaveSettingsWidgetPriv* d;
};

} // NameSpace KIPIRawConverterPlugin

#endif /* SAVESETTINGSWIDGET_H */
