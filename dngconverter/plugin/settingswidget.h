/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-24
 * Description : save settings widgets
 *
 * Copyright (C) 2008-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

// Qt includes

#include <QWidget>

namespace KIPIDNGConverterPlugin
{

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:

    enum ConflictRule 
    {
        OVERWRITE = 0,
        ASKTOUSER
    };

public:

    SettingsWidget(QWidget* parent);
    ~SettingsWidget();

    ConflictRule conflictRule();
    void setConflictRule(ConflictRule r);

    void setCompressLossLess(bool b);
    bool compressLossLess() const;

    void setUpdateFileDate(bool b);
    bool updateFileDate() const;

    void setPreviewMode(int mode);
    int  previewMode() const;

    void setBackupOriginalRawFile(bool b);
    bool backupOriginalRawFile() const;

    void setDefaultSettings();

private Q_SLOTS:

    void processUrl(const QString& url);

private:

    class SettingsWidgetPriv;
    SettingsWidgetPriv* const d;
};

} // namespace KIPIDNGConverterPlugin

#endif /* SETTINGSWIDGET_H */
