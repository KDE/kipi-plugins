//////////////////////////////////////////////////////////////////////////////
//
//    PLUGIN_FINDIMAGES.CPP
//
//    Copyright (C) 2004 Gilles CAULIER <caulier dot gilles at free.fr>
//    Copyright (C) 2004 Richard Groult <rgroult at jalix.org>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////////

// Qt includes.

#include <qprogressdialog.h>
#include <qfileinfo.h>

// KDE includes.

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kapplication.h>

// Lib KIPI includes.

#include <libkipi/interface.h>

// Local include files

#include "plugin_findimages.h"
#include "finddupplicateimages.h"
#include "actions.h"

/////////////////////////////////////////////////////////////////////////////////////////////

typedef KGenericFactory<Plugin_FindImages> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_findimages,
                            Factory("kipiplugin_findimages"));

Plugin_FindImages::Plugin_FindImages(QObject *parent, const char*, const QStringList&)
    : KIPI::Plugin( Factory::instance(), parent, "FindImages")
{
    kdDebug( 51001 ) << "Plugin_FindImages plugin loaded" << endl;
}

void Plugin_FindImages::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );
    m_action_findImages = new KActionMenu(i18n("&Find images..."),
                           actionCollection(),
                           "findimages");

    m_action_findDuplicateImagesAlbums= new KAction(i18n("Find duplicate images"),
                                                    "finddupplicateimages",
                                                    0,
                                                    this,
                                                    SLOT(slotFindDuplicateImages()),
                                                    actionCollection(),
                                                    "findduplicateimagesalbums");

    addAction( m_action_findImages );
    m_action_findImages->insert(m_action_findDuplicateImagesAlbums);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

Plugin_FindImages::~Plugin_FindImages()
{
}


/////////////////////////////////////// SLOTS ///////////////////////////////////////////////////////

void Plugin_FindImages::slotFindDuplicateImages()
{
    m_progressDlg=0;

    KIPI::Interface* interface = static_cast<KIPI::Interface*>( parent() );
    findDuplicateOperation = new FindDuplicateImages( interface, this);
    if(findDuplicateOperation->showDialog())
    {
    	findDuplicateOperation->compareAlbums();
    }
    return;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_FindImages::slotCancel()
{
    findDuplicateOperation->terminate();
    findDuplicateOperation->wait();
    if (m_progressDlg) {
        m_progressDlg->reset();
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_FindImages::customEvent(QCustomEvent *event)
{
    if (!event) return;
    if(!m_progressDlg)
    {
    	m_progressDlg = new QProgressDialog (i18n("Comparisons"), i18n("&Cancel"), 0,
                                                     0, 0, true);
        connect(m_progressDlg, SIGNAL(cancelled()),
                SLOT(slotCancel()));

    	m_current=0;
    	m_progressDlg->show();
    }

    FindImages::EventData *d = (FindImages::EventData*) event->data();
    if (!d) return;
    if (d->starting) {
        QString text;
        switch (d->action) {
        case(FindImages::Similar): {
            text = i18n("Similar comparison for\n%1").arg(QFileInfo(d->fileName).fileName() );
            break;
        }
        case(FindImages::Exact): {
            text = i18n("Exact comparison for\n%1").arg(QFileInfo(d->fileName).fileName());
            break;
        }
        case(FindImages::Matrix): {
            text = i18n("Creating fingerprint for\n%1").arg(QFileInfo(d->fileName).fileName());
            break;
        }
        case(FindImages::Progress): {
            m_current=0;
	    m_total = d->total;
	    text = i18n("Initialising %1 image(s)...").arg(d->total);
    	    m_progressDlg->show();
            break;
        }
        default: {
            kdWarning( 51000 ) << "Plugin_FindImages: Unknown event: " << d->action << endl;
        }
        }
        m_progressDlg->setLabelText(text);
    }
    else {

        if (!d->success) {

            QString text;
            switch (d->action) {
            case(FindImages::Similar): {
                text = i18n("Failed to find similar images");
                break;
            }
            case(FindImages::Exact): {
                text = i18n("Failed to find exact image");
                break;
            }
            default: {
                kdWarning( 51000 ) << "Plugin_FindImages: Unknown event: " << d->action << endl;
            }
            }

            //Plugin_FindImages::MessageBox::showMsg(d->fileName, text);
        }

        m_current++;
        m_progressDlg->setProgress(m_current, m_total);
	if(d->action==FindImages::Progress)
	{
		m_current     = 0;
        	m_progressDlg->reset();
		findDuplicateOperation->showResult();
	}

    }
    kapp->processEvents();
    delete d;
}

KIPI::Category Plugin_FindImages::category() const
{
    return KIPI::COLLECTIONSPLUGIN;
}


#include "plugin_findimages.moc"
