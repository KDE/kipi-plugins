/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-02-08
 * Description : a kipi plugin to print images
 *
 * Copyright 2009-2012 by Angelo Naselli <anaselli at linux dot it>
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

#include "plugin_printimages.h"

// Qt includes

#include <QApplication>
#include <QMessageBox>
#include <QPrinter>
#include <QWidget>
#include <QAction>

// KDE includes

#include <kactioncollection.h>
#include <klocalizedstring.h>
#include <kpluginfactory.h>

// Libkipi includes

#include <KIPI/ImageCollection>
#include <KIPI/Interface>
#include <KIPI/Plugin>

// Local includes

#include "kputil.h"
#include "wizard.h"
#include "printhelper.h"
#include "kipiplugins_debug.h"

namespace KIPIPrintImagesPlugin
{

K_PLUGIN_FACTORY (PrintImagesFactory, registerPlugin<Plugin_PrintImages>();)

Plugin_PrintImages::Plugin_PrintImages (QObject* const parent, const QVariantList& /*args*/)
    : Plugin(parent, "PrintImages")
{
    m_printImagesAction    = 0;
    m_printAssistantAction = 0;
    m_interface            = 0;

    qCDebug(KIPIPLUGINS_LOG) << "Plugin_PrintImages plugin loaded";

    setUiBaseName("kipiplugin_printimagesui.rc");
    setupXML();
}

Plugin_PrintImages::~Plugin_PrintImages()
{
    removeTemporaryDir("printassistant");
}

void Plugin_PrintImages::setup(QWidget* const widget)
{
    Plugin::setup (widget);

    setupActions();

    m_interface = interface();

    if (!m_interface)
    {
        qCCritical(KIPIPLUGINS_LOG) << "Kipi interface is null!";
        return;
    }

    ImageCollection selection = m_interface->currentSelection();
    m_printImagesAction->setEnabled (selection.isValid() && !selection.images().isEmpty());
    m_printAssistantAction->setEnabled(selection.isValid() && !selection.images().isEmpty());

    connect(m_interface, SIGNAL(selectionChanged(bool)),
            m_printImagesAction, SLOT(setEnabled(bool)));

    connect(m_interface, SIGNAL(selectionChanged(bool)),
            m_printAssistantAction, SLOT(setEnabled(bool)));
}

void Plugin_PrintImages::setupActions()
{
    setDefaultCategory(ImagesPlugin);

    m_printImagesAction = new QAction(this);
    m_printImagesAction->setText(i18n("Print images"));
    m_printImagesAction->setIcon(QIcon::fromTheme(QLatin1String("document-print")));
    m_printImagesAction->setEnabled(false);

    connect(m_printImagesAction, SIGNAL(triggered(bool)),
            this, SLOT(slotPrintImagesActivate()));

    addAction(QLatin1String("printimages"), m_printImagesAction);

    m_printAssistantAction = new QAction(this);
    m_printAssistantAction->setText(i18n("Print Assistant..."));
    m_printAssistantAction->setIcon(QIcon::fromTheme(QLatin1String("document-print")));
    m_printAssistantAction->setEnabled(false);

    connect(m_printAssistantAction, SIGNAL(triggered(bool)),
            this, SLOT(slotPrintAssistantActivate()));

    addAction(QLatin1String("printassistant"), m_printAssistantAction);
}

void Plugin_PrintImages::slotPrintImagesActivate()
{
    ImageCollection album = m_interface->currentSelection();

    if (!album.isValid())
        return;

    QList<QUrl> fileList = album.images();

    if (fileList.isEmpty())
    {
        QMessageBox::information(QApplication::activeWindow(), i18n("Print Images"), i18n("Please select one or more photos to print."));
        return;
    }

    QWidget* const parent = QApplication::activeWindow();
    PrintHelper printPlugin(parent);
    printPlugin.print(fileList);
}

void Plugin_PrintImages::slotPrintAssistantActivate()
{
    ImageCollection album = m_interface->currentSelection();

    if (!album.isValid())
        return;

    QList<QUrl> fileList = album.images();

    if (fileList.isEmpty())
    {
        QMessageBox::information(QApplication::activeWindow(), i18n("Print Assistant"), i18n("Please select one or more photos to print."));
        return;
    }

    QWidget* const parent = QApplication::activeWindow();
    Wizard printAssistant(parent);

    QString tmp = makeTemporaryDir("printassistant").absolutePath() + QLatin1Char('/');

    printAssistant.print(fileList, tmp);

    if (printAssistant.exec() == QDialog::Rejected)
        return;
}

} // namespace KIPIPrintImagesPlugin

#include "plugin_printimages.moc"
