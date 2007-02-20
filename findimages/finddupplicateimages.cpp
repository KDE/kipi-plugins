//////////////////////////////////////////////////////////////////////////////
//
//    FINDDUPPLICATEIMAGES.CPP
//
//    Copyright (C) 2001 Richard Groult <rgroult at jalix.org> (from ShowImg project)
//    Copyright (C) 2004 Gilles Caulier <caulier dot gilles at gmail dot com>
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
//    Foundation, Inc., 51 Franklin Steet, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////

// Include files for C ansi

extern "C"
{
#include <stdlib.h>
#include <math.h>
}

// Include files for Qt

#include <qfile.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qimage.h>
#include <qprogressdialog.h>
#include <qmutex.h>

// Include files for KDE

#include <klocale.h>
#include <kinstance.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kimageeffect.h>
#include <kdebug.h>
#include <kprogress.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <kio/job.h>
#include <kio/jobclasses.h>
#include <kio/netaccess.h>
#include <kio/global.h>
#include <kimageio.h>

// Local include files

#include "finddupplicateimages.h"
#include "finddupplicatedialog.h"
#include "displaycompare.h"
#include "actions.h"
#include <qcursor.h>
#include "imagesimilaritydata.h"
#include "fuzzycompare.h"
#include "fastcompare.h"

namespace KIPIFindDupplicateImagesPlugin
{


//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

FindDuplicateImages::FindDuplicateImages( KIPI::Interface* interface, QObject *parent)
    : QObject(parent), QThread(), m_interface( interface ),
      m_cacheDir(KGlobal::dirs()->saveLocation("cache", "kipi-findduplicate/")),
      m_compareOp( 0 )
{
    KImageIO::registerFormats();
    parent_ = parent;
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

FindDuplicateImages::~FindDuplicateImages()
{
    delete m_findDuplicateDialog;
    wait();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateImages::writeSettings(void)
{
    config = new KConfig("kipirc");
    config->setGroup("FindDuplicateImages Settings");

    // Method dialogbox setup tab

    config->writeEntry("FindMethod", m_findDuplicateDialog->getFindMethod());
    config->writeEntry("ApproximateThreeshold", m_findDuplicateDialog->getApproximateThreeshold());

    config->sync();
    delete config;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateImages::readSettings(void)
{
    config = new KConfig("kipirc");
    config->setGroup("FindDuplicateImages Settings");

    // Method dialogbox setup tab

    m_findDuplicateDialog->setFindMethod( config->readNumEntry("FindMethod", FindDuplicateDialog::MethodAlmost ) );
    m_findDuplicateDialog->setApproximateThreeshold( config->readNumEntry("ApproximateThreeshold", 88) );

    delete config;

    // Get the image files filters from the hosts app.

    m_imagesFileFilter = m_interface->fileExtensions();
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool FindDuplicateImages::execDialog()
{
    qApp->setOverrideCursor( QCursor(Qt::WaitCursor) );
    m_findDuplicateDialog = new FindDuplicateDialog( m_interface, kapp->activeWindow() );
    qApp->restoreOverrideCursor();

    readSettings();

    connect( m_findDuplicateDialog, SIGNAL(updateCache(QStringList)),
             this, SLOT(slotUpdateCache(QStringList)) );

    connect( m_findDuplicateDialog, SIGNAL(clearCache(QStringList)),
             this, SLOT(slotClearCache(QStringList)) );

    connect( m_findDuplicateDialog, SIGNAL(clearAllCache()),
             this, SLOT(slotClearAllCache()) );

    if ( m_findDuplicateDialog->exec() == QDialog::Accepted )
    {
        // This is the value for approximate comparison level between 2 images.
        m_approximateLevel = (float) m_findDuplicateDialog->getApproximateThreeshold() / (float)100;

        writeSettings();
        return true;
    }

    return false;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateImages::showResult()
{
    if( !m_res.isEmpty() )
        DisplayCompare((QWidget *)(kapp->activeWindow()), m_interface, m_res).exec();
    else
        KMessageBox::information(kapp->activeWindow(), i18n("No identical files found"));
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateImages::compareAlbums(void)
{
    qApp->setOverrideCursor( QCursor(Qt::WaitCursor) );

    writeSettings();

    // Prepare the data for the threaded operations.

    QValueList<KIPI::ImageCollection> ListAlbums(m_findDuplicateDialog->getSelectedAlbums());
    filesList.clear();

    for( QValueList<KIPI::ImageCollection>::Iterator it = ListAlbums.begin(); it != ListAlbums.end(); ++it )
    {
        KURL::List Files = (*it).images();

        for( KURL::List::Iterator it2 = Files.begin(); it2 != Files.end(); ++it2 )
        {
            if ( !filesList.contains( (*it2).path() ) )
            {
                filesList.append( (*it2).path() ); // PENDING(blackie) handle remote URLS
            }
        }

        kapp->processEvents();
    }

    if ( m_findDuplicateDialog->getFindMethod() == FindDuplicateDialog::MethodAlmost )
    {
        FuzzyCompare *op = new FuzzyCompare( parent_, m_cacheDir );
        op->setApproximateThreeshold( m_approximateLevel );
        m_compareOp = op;
    }
    else
        m_compareOp = new FastCompare( parent_ );

    start();      // Starting the thread.

    qApp->restoreOverrideCursor();
}


/////////////////////////////////////////////////////////////////////////////////////////////
// List of threaded operations.

void FindDuplicateImages::run()
{
    m_res = m_compareOp->compare(filesList );
    sendMessage( parent_, KIPIFindDupplicateImagesPlugin::Progress, QString::null, 0, false, true );
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Nota: original source code from ShowImg !

void FindDuplicateImages::slotClearCache(QStringList fromDirs)
{
    bool delOk = true;

    for ( QStringList::Iterator it = fromDirs.begin(); it != fromDirs.end(); ++it )
    {
        QString deleteImage = m_cacheDir + *it ;

        if ( DeleteDir(deleteImage) == false )
            delOk = false;
    }

    if ( delOk == true )
        KMessageBox::information(m_findDuplicateDialog, i18n("Selected Albums cache purged successfully!"));
    else
        KMessageBox::error(m_findDuplicateDialog, i18n("Cannot purge selected Albums cache!"));
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateImages::slotClearAllCache(void)
{
    bool delOk = DeleteDir(m_cacheDir);

    if ( delOk == true )
        KMessageBox::information(m_findDuplicateDialog, i18n("All cache purged successfully!"));
    else
        KMessageBox::error(m_findDuplicateDialog, i18n("Cannot purge all cache!"));
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Nota: original source code from ShowImg !

void FindDuplicateImages::slotUpdateCache(QStringList fromDirs)
{
    pdCache = new QProgressDialog (m_findDuplicateDialog, "tmppb", true);
    pdCache->setLabelText(i18n("Updating in progress..."));
    pdCache->setTotalSteps(2);
    pdCache->show();
    pdCache->setProgress(2);

    for ( QStringList::Iterator it = fromDirs.begin(); it != fromDirs.end(); ++it )
        updateCache(*it);

    pdCache->close();
    delete(pdCache);
    KMessageBox::information(m_findDuplicateDialog, i18n("Selected Albums cache updated successfully!"));
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Nota: original source code from ShowImg !

void FindDuplicateImages::updateCache(QString fromDir)
{
    // PENDING(blackie) this method doesn't seem to work.

    kdDebug( 51000 ) << fromDir.ascii() << endl;
    pdCache->setLabelText(i18n("Updating in progress for:\n") + fromDir);
    QDir d(m_cacheDir + fromDir);
    int len = m_cacheDir.length()-1; // Remove trailing /
    bool delDir = false;

    kdDebug( 51000 ) << m_cacheDir + fromDir.latin1() << endl;

    if ( !QFileInfo(fromDir).exists() )
        delDir = true;      // If the source folder have been removed, remove also the cache...

    d.setFilter( QDir::All | QDir::Hidden | QDir::NoSymLinks );
    const QFileInfoList *list = d.entryInfoList();

    if ( !list )
        return;

    QFileInfoListIterator it( *list );
    QFileInfo *fi;

    while ( (fi = it.current()) != 0 )
    {
        kapp->processEvents();
        QString fCache=fi->absFilePath();
        QString orgFile=fCache.right(fCache.length()-len);

        if ( fi->isDir() && !fromDir.startsWith(orgFile) )
        {
            updateCache(orgFile);
        }
        else
        {
            if ( !QFileInfo(orgFile).exists() && QFileInfo(orgFile).extension(false) != "dat" )
            {
                QDir().remove(fCache);
                QDir().remove(fCache + ".dat");
            }
        }
        ++it;
    }

    if (delDir)
        QDir().rmdir(m_cacheDir + fromDir);
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Nota: original source code from ShowImg !

float FindDuplicateImages::image_sim_compare(ImageSimilarityData *a, ImageSimilarityData *b)
{
    float sim;
    int i;

    if ( !a || !b || !a->filled || !b->filled )
        return 0.0;

    sim = 0.0;

    for( i = 0; i < PAS*PAS; i++ )
    {
        sim += (float)abs(a->avg_r[i] - b->avg_r[i]) / 255.0;
        sim += (float)abs(a->avg_g[i] - b->avg_g[i]) / 255.0;
        sim += (float)abs(a->avg_b[i] - b->avg_b[i]) / 255.0;
    }

    sim /= (1024.0 * 3.0);
    return 1.0 - sim;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////

bool FindDuplicateImages::DeleteDir(QString dirname)
{
    if ( !dirname.isEmpty() )
    {
        QDir dir;

        if (dir.exists ( dirname ) == true)
        {
            if (deldir(dirname) == false)
                return false;

            if (dir.rmdir( dirname ) == false )
                return false;
        }
        else
            return false;
    }
    else
        return false;

    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool FindDuplicateImages::deldir(QString dirname)
{
    QDir *dir = new QDir(dirname);
    dir->setFilter ( QDir::Dirs | QDir::Files | QDir::NoSymLinks );

    const QFileInfoList* fileinfolist = dir->entryInfoList();
    QFileInfoListIterator it(*fileinfolist);
    QFileInfo* fi;

    while( (fi = it.current() ) )
    {
        if(fi->fileName() == "." || fi->fileName() == ".." )
        {
            ++it;
            continue;
        }

        if( fi->isDir() )
        {
            if (deldir( fi->absFilePath() ) == false)
                return false;
            if (dir->rmdir( fi->absFilePath() ) == false)
                return false;
        }
        else
            if( fi->isFile() )
                if (dir->remove(fi->absFilePath() ) == false)
                    return false;

        kapp->processEvents();
        ++it;
    }

    return true;
}

}  // NameSpace KIPIFindDupplicateImagesPlugin

void KIPIFindDupplicateImagesPlugin::FindDuplicateImages::stopPlease()
{
    if ( m_compareOp )
        m_compareOp->stopPlease();
}

#include "finddupplicateimages.moc"
