/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-01-31
 * Description : a kipi plugin to convert Raw file in single 
 *               or batch mode.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PLUGIN_RAWCONVERTER_H
#define PLUGIN_RAWCONVERTER_H

// Qt includes

#include <QVariant>

// LibKIPI includes

#include <libkipi/plugin.h>

class KAction;

namespace KIPIRawConverterPlugin
{
    class SingleDialog;
    class BatchDialog;
}

class Plugin_RawConverter : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_RawConverter(QObject *parent, const QVariantList& args);
    ~Plugin_RawConverter();

    KIPI::Category category(KAction* action) const;
    void setup(QWidget* widget);

private:

    bool checkBinaries();
    bool isRAWFile(const QString& filePath);

private Q_SLOTS:

    void slotActivateSingle();
    void slotActivateBatch();

private:

    KAction                              *m_singleAction;
    KAction                              *m_batchAction;

    KIPIRawConverterPlugin::SingleDialog *m_singleDlg;
    KIPIRawConverterPlugin::BatchDialog  *m_batchDlg;
};

#endif /* PLUGIN_RAWCONVERTER_H */
