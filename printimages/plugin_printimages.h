/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-02-08
 * Description : a kipi plugin to print images
 *
 * Copyright 2009 by Angelo Naselli <anaselli at linux dot it>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef PLUGIN_PRINTIMAGES_H
#define PLUGIN_PRINTIMAGES_H

// Qt includes

#include <QVariant>

// LibKIPI includes

#include <libkipi/plugin.h>

class KAction;

namespace KIPI
{
  class Interface;
} // namespace KIPI

class Plugin_PrintImages : public KIPI::Plugin
{
    Q_OBJECT

  public:
    Plugin_PrintImages ( QObject *parent, const QVariantList &args );

    ~Plugin_PrintImages();
    virtual KIPI::Category category ( KAction* action ) const;
    virtual void setup ( QWidget* widget );

  public Q_SLOTS:

    void slotPrintImagesActivate();
    void slotPrintAssistantActivate();

  private:
    KAction         *m_printImagesAction;
    KAction         *m_printAssistantAction;
    KIPI::Interface *m_interface;
};

#endif // PLUGIN_PRINTIMAGES_H
