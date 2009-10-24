// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/*
A KIPI plugin to generate HTML image galleries
Copyright 2006 by Aurelien Gateau <aurelien dot gateau at free.fr>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.

*/
// Self
#include "plugin.moc"

// Qt
#include <QPointer>

// KDE
#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>
#include <krun.h>

// KIPIPlugins
#include <batchprogressdialog.h>
#include <libkipi/interface.h>

// Local
#include "galleryinfo.h"
#include "generator.h"
#include "wizard.h"

K_PLUGIN_FACTORY(HTMLExportFactory, registerPlugin<KIPIHTMLExport::Plugin>();)
K_EXPORT_PLUGIN( HTMLExportFactory("kipiplugin_htmlexport"))

namespace KIPIHTMLExport {


struct Plugin::Private {
    KAction* mAction;
};


Plugin::Plugin(QObject *parent, const QVariantList&)
: KIPI::Plugin(HTMLExportFactory::componentData(), parent, "HTMLExport")
{
    d=new Private;
    d->mAction=0;
}


Plugin::~Plugin() {
    delete d;
}


void Plugin::setup( QWidget* widget ) {
    KIPI::Plugin::setup( widget );
    d->mAction = actionCollection()->addAction("htmlexport");
    d->mAction->setText(i18n("Export to &HTML..."));
    d->mAction->setIcon(KIcon("applications-internet"));
    d->mAction->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_H));
    connect(d->mAction, SIGNAL(triggered()),
        SLOT(slotActivate()) );
    addAction(d->mAction);
}


void Plugin::slotActivate() {
    KIPI::Interface* interface = dynamic_cast< KIPI::Interface* >( parent() );
    Q_ASSERT(interface);

    GalleryInfo info;
    info.readConfig();
    QWidget* parent=QApplication::activeWindow();
    QPointer<Wizard> wizard = new Wizard(parent, &info, interface);
    if (wizard->exec()==QDialog::Rejected) return;
    info.writeConfig();

    KIPIPlugins::BatchProgressDialog* progressDialog=new KIPIPlugins::BatchProgressDialog(parent, i18n("Generating gallery..."));

    Generator generator(interface, &info, progressDialog);
    progressDialog->show();
    if (!generator.run()) return;

    if (generator.warnings()) {
        progressDialog->addedAction(i18n("Finished, but some warnings occurred."), KIPIPlugins::WarningMessage);
        progressDialog->setButtons(KDialog::Close);
    } else {
        progressDialog->close();
    }

    if (info.openInBrowser()) {
        KUrl url=info.destUrl();
        url.addPath("index.html");
        KRun::runUrl(url, "text/html", parent);
    }

    delete wizard;
}


KIPI::Category Plugin::category(KAction* action) const {
    if (action == d->mAction) {
        return KIPI::ExportPlugin;
    }

    kWarning() << "Unrecognized action for plugin category identification";
    return KIPI::ExportPlugin; // no warning from compiler, please
}

} // namespace
