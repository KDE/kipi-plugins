/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-04-04
 * Description : A KIPI plugin to generate HTML image galleries
 *
 * Copyright (C) 2006-2010 by Aurelien Gateau <aurelien dot gateau at free.fr>
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

// vim: set tabstop=4 shiftwidth=4 noexpandtab:

#include "plugin_htmlexport.moc"

// Qt includes

#include <QPointer>

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>
#include <krun.h>

// libKipi includes

#include <libkipi/interface.h>

// Local includes

#include "galleryinfo.h"
#include "generator.h"
#include "wizard.h"
#include "kpbatchprogressdialog.h"

using namespace KIPIPlugins;

namespace KIPIHTMLExport
{

K_PLUGIN_FACTORY(HTMLExportFactory, registerPlugin<Plugin_HTMLExport>();)
K_EXPORT_PLUGIN(HTMLExportFactory("kipiplugin_htmlexport"))

struct Plugin_HTMLExport::Private
{
    KAction* mAction;
};

Plugin_HTMLExport::Plugin_HTMLExport(QObject* const parent, const QVariantList&)
    : Plugin(HTMLExportFactory::componentData(), parent, "HTMLExport")
{
    d          = new Private;
    d->mAction = 0;
}

Plugin_HTMLExport::~Plugin_HTMLExport()
{
    delete d;
}

void Plugin_HTMLExport::setup( QWidget* widget )
{
    Plugin::setup( widget );
    d->mAction = actionCollection()->addAction("htmlexport");
    d->mAction->setText(i18n("Export to &HTML..."));
    d->mAction->setIcon(KIcon("text-html"));
    d->mAction->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_H));

    connect(d->mAction, SIGNAL(triggered()),
            this, SLOT(slotActivate()) );

    addAction(d->mAction);
}

void Plugin_HTMLExport::slotActivate()
{
    Interface* interface = dynamic_cast< Interface* >( parent() );
    Q_ASSERT(interface);

    GalleryInfo info;
    info.readConfig();
    QWidget* parent=QApplication::activeWindow();
    QPointer<Wizard> wizard = new Wizard(parent, &info);
    if (wizard->exec() == QDialog::Rejected)
    {
        delete wizard;
        return;
    }
    info.writeConfig();

    KPBatchProgressDialog* progressDialog = new KPBatchProgressDialog(parent, i18n("Generating gallery..."));

    Generator generator(interface, &info, progressDialog);
    progressDialog->show();
    if (!generator.run())
    {
        delete wizard;
        return;
    }

    if (generator.warnings())
    {
        progressDialog->progressWidget()->addedAction(i18n("Finished, but some warnings occurred."), WarningMessage);
        progressDialog->setButtons(KDialog::Close);
    }
    else
    {
        progressDialog->close();
    }

    if (info.openInBrowser())
    {
        KUrl url=info.destUrl();
        url.addPath("index.html");
        KRun::runUrl(url, "text/html", parent);
    }

    delete wizard;
}

Category Plugin_HTMLExport::category(KAction* action) const
{
    if (action == d->mAction)
    {
        return ExportPlugin;
    }

    kWarning() << "Unrecognized action for plugin category identification";
    return ExportPlugin; // no warning from compiler, please
}

} // namespace KIPIHTMLExport
