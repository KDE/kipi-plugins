//////////////////////////////////////////////////////////////////////////////
//
//    FINDDUPPLICATEIMAGES.CPP
//
//    Copyright (C) 2001 Richard Groult <rgroult at jalix.org> (from ShowImg project)
//    Copyright (C) 2004 Gilles Caulier <caulier dot gilles at free.fr>
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

namespace KIPIFindDupplicateImagesPlugin
{

/////////////////////////////////////////////////////////////////////////////////////////////
// Nota: original source code from ShowImg !
// A class to store datas to look for similaties of 2 images.

class ImageSimilarityData
{
public:
   ImageSimilarityData()
      {
      avg_r = (uchar*)malloc(PAS*PAS*sizeof(uchar));
      avg_g = (uchar*)malloc(PAS*PAS*sizeof(uchar));
      avg_b = (uchar*)malloc(PAS*PAS*sizeof(uchar));
      }

   ~ImageSimilarityData()
      {
      delete(avg_r);
      delete(avg_g);
      delete(avg_b);
      }

   QString filename;

   uchar *avg_r;
   uchar *avg_g;
   uchar *avg_b;

   int filled;
   float ratio;
};


//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

FindDuplicateImages::FindDuplicateImages( KIPI::Interface* interface, QObject *parent)
                    : QObject(parent), QThread(), m_interface( interface )
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

    m_findDuplicateDialog->setFindMethod( config->readEntry("FindMethod", i18n("Almost")) );
    m_findDuplicateDialog->setApproximateThreeshold( config->readNumEntry("ApproximateThreeshold", 88) );

    delete config;

    // Get the image files filters from the hosts app.
     
    m_imagesFileFilter = m_interface->fileExtensions();
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool FindDuplicateImages::showDialog()
{
    m_findDuplicateDialog = new FindDuplicateDialog( m_interface );
    readSettings();

    // This is the value for approximate comparison level between 2 images.
    m_approximateLevel = (float) m_findDuplicateDialog->getApproximateThreeshold() / (float)100;

    connect( m_findDuplicateDialog, SIGNAL(updateCache(QStringList)),
             this, SLOT(slotUpdateCache(QStringList)) );

    connect( m_findDuplicateDialog, SIGNAL(clearCache(QStringList)),
             this, SLOT(slotClearCache(QStringList)) );

    connect( m_findDuplicateDialog, SIGNAL(clearAllCache()),
             this, SLOT(slotClearAllCache()) );

    return m_findDuplicateDialog->exec() == QDialog::Accepted;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateImages::showResult()
{
    if( !res->isEmpty() )
        DisplayCompare(0, m_interface, res).exec();
    else
        KMessageBox::information(0, i18n("No identical files found"));
    delete(res);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateImages::compareAlbums(void)
{
       writeSettings();
       QValueList<KIPI::ImageCollection> ListAlbums(m_findDuplicateDialog->getAlbumsSelection());
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

       if ( m_findDuplicateDialog->getFindMethod() == i18n("Almost") )
           isCompareAlmost = true;
       else
           isCompareAlmost = false;

    start();
    return;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateImages::run()
{
       if ( isCompareAlmost )
           compareAlmost(filesList);
       else
           compareFast(filesList);
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Nota: original source code from ShowImg !

char FindDuplicateImages::getRed(QImage* im, int x, int y)
{
    return qRed(im->pixel(x, y));
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Nota: original source code from ShowImg !

char FindDuplicateImages::getGreen(QImage* im, int x, int y)
{
    return qGreen(im->pixel(x, y));
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Nota: original source code from ShowImg !

char FindDuplicateImages::getBlue(QImage* im, int x, int y)
{
    return qBlue(im->pixel(x, y));
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Nota: original source code from ShowImg !

void FindDuplicateImages::slotClearCache(QStringList fromDirs)
{
    bool delOk = true;

    for ( QStringList::Iterator it = fromDirs.begin(); it != fromDirs.end(); ++it )
        {
        KURL deleteImage( QDir::homeDirPath() + "/.findduplicate/cache/" + *it );

        if ( KIO::NetAccess::del(deleteImage) == false )
           delOk = false;
        }

    if ( delOk == true )
       KMessageBox::information(0, i18n("Selected Albums cache purged successfully!"));
    else
       KMessageBox::error(0, i18n("Cannot purge selected Albums cache!"));
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FindDuplicateImages::slotClearAllCache(void)
{
    bool delOk = DeleteDir(QDir::homeDirPath() + "/.findduplicate/cache/");

    if ( delOk == true )
       KMessageBox::information(0, i18n("All cache purged successfully!"));
    else
       KMessageBox::error(0, i18n("Cannot purge all cache!"));
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Nota: original source code from ShowImg !

void FindDuplicateImages::slotUpdateCache(QStringList fromDirs)
{
    pdCache = new QProgressDialog (0, "tmppb", true);
    pdCache->setLabelText(i18n("Updating in progress..."));
    pdCache->setTotalSteps(2);
    pdCache->show();
    pdCache->setProgress(2);

    for ( QStringList::Iterator it = fromDirs.begin(); it != fromDirs.end(); ++it )
        updateCache(*it);

    pdCache->close();
    delete(pdCache);
    KMessageBox::information(0, i18n("Selected Albums cache updated successfully!"));
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Nota: original source code from ShowImg !

void FindDuplicateImages::updateCache(QString fromDir)
{
    // PENDING(blackie) this method doesn't seem to work.
    
    kdDebug( 51000 ) << fromDir.ascii() << endl;
    pdCache->setLabelText(i18n("Updating in progress for:\n") + fromDir);
    QDir d(QDir::homeDirPath() + "/.findduplicate/cache/" + fromDir);
    int len = QString(QDir::homeDirPath() + "/.findduplicate/cache").length();
    bool delDir = false;

    kdDebug( 51000 ) << QDir::homeDirPath() + "/.findduplicate/cache/" + fromDir.latin1() << endl;

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
        QDir().rmdir(QDir::homeDirPath()+"/.findduplicate/cache/" + fromDir);
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


/////////////////////////////////////////////////////////////////////////////////////////////
// Nota: original source code from ShowImg !

float FindDuplicateImages::image_sim_compare_fast(ImageSimilarityData *a, ImageSimilarityData *b, float min)
{
    float sim;
    int i, j;

    if ( !a || !b || !a->filled || !b->filled )
       return 0.0;

    if( fabs(a->ratio - b->ratio) > 0.1 )
       return 0.0;

    min = 1.0 - min;
    sim = 0.0;

    for ( j = 0; j < PAS*PAS; j+= PAS )
       {
       for ( i = j; i < j + PAS; i++ )
          {
          sim += (float)abs(a->avg_r[i] - b->avg_r[i]) / 255.0;
          sim += (float)abs(a->avg_g[i] - b->avg_g[i]) / 255.0;
          sim += (float)abs(a->avg_b[i] - b->avg_b[i]) / 255.0;
          }

       // check for abort, if so return 0.0

       if ( j > PAS*PAS/3 && 1-sim/((j+1) * 3.0) < min )
          return 0.0;
       }

    sim /= (PAS*PAS * 3.0);

    return 1.0 - sim;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Nota: original source code from ShowImg !

bool FindDuplicateImages::equals(QFile * f1, QFile * f2)
{
    if ( QFileInfo (*f1).size () != QFileInfo (*f2).size () )
       return false;

    f1->open (IO_ReadOnly);
    f2->open (IO_ReadOnly);

    QDataStream s1 (f1);
    QDataStream s2 (f2);

    Q_INT8 b1, b2;
    bool eq = true;

    while ( !s1.atEnd () && eq )
       {
       s1 >> b1;
       s2 >> b2;
       eq = (b1 == b2);
       }

    f1->close ();
    f2->close ();
    return eq;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Nota: original source code from ShowImg !

ImageSimilarityData* FindDuplicateImages::image_sim_fill_data(QString filename)
{
    int w, h;
    uchar *pix;
    int has_alpha;
    int p_step;

    int i,j;
    int x_inc, y_inc;
    int xs, ys;
    const int INC=1;

    QImage *pixbuf;
    ImageSimilarityData *sd = new ImageSimilarityData();
    sd->filename=filename;

    QFileInfo info(QDir::homeDirPath()+"/.findduplicate/cache/" + QFileInfo(filename).absFilePath()+".dat");

    if(info.exists())
        {
        QFile f(QDir::homeDirPath () + "/.findduplicate/cache/"+QFileInfo(filename).absFilePath()+".dat");
        if ( f.open(IO_ReadOnly) )
            {
            QDataStream s( &f );
            s >> sd->ratio;
            for(int i=0 ; i<PAS*PAS ; i++) s >> sd->avg_r[i];
            for(int i=0 ; i<PAS*PAS ; i++) s >> sd->avg_g[i];
            for(int i=0 ; i<PAS*PAS ; i++) s >> sd->avg_b[i];
            f.close();
            }

        sd->filled = TRUE;
        return sd;
        }

    pixbuf = new QImage(filename);

    if ( !sd || !pixbuf )
       return 0L;

    KImageEffect::equalize(*pixbuf);

    w = pixbuf->width();
    h = pixbuf->height();
    pix = pixbuf->bits();
    has_alpha = pixbuf->hasAlphaBuffer();
    p_step = has_alpha ? 4 : 3;

    x_inc = w / PAS;
    y_inc = h / PAS;

    if ( x_inc < 1 || y_inc < 1 )
        return 0L;

    j = 0;

    for (ys = 0; ys < PAS; ys++)
        {
        i = 0;

        for (xs = 0; xs < PAS; xs++)
            {
            int x, y;
            int r, g, b;
            r = g = b = 0;

            for (y = j; y < j + y_inc; y+=INC)
                {
                for (x = i; x < i + x_inc; x+=INC)
                    {
                    r +=getRed(pixbuf, x, y);
                    g +=getGreen(pixbuf, x, y);
                    b +=getBlue(pixbuf, x, y);
                    }
                }

            r /= x_inc * y_inc;
            g /= x_inc * y_inc;
            b /= x_inc * y_inc;

            sd->avg_r[ys * PAS + xs] = r;
            sd->avg_g[ys * PAS + xs] = g;
            sd->avg_b[ys * PAS + xs] = b;

            i += x_inc;
            }
        j += y_inc;
        }

    sd->filled = TRUE;
    sd->ratio=((float)w)/h;
    delete(pixbuf);

    // Saving the data.

    QFile f(QDir::homeDirPath () + "/.findduplicate/cache/"+QFileInfo(filename).absFilePath()+".dat");
    KStandardDirs::makeDir(QFileInfo(f).dirPath(true));

    if ( f.open(IO_WriteOnly) )
        {
        QDataStream s( &f );
        s << sd->ratio;
        for(int i=0 ; i<PAS*PAS ; i++) s << sd->avg_r[i];
        for(int i=0 ; i<PAS*PAS ; i++) s << sd->avg_g[i];
        for(int i=0 ; i<PAS*PAS ; i++) s << sd->avg_b[i];
        f.close();
        }

    return sd;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Nota: original source code from ShowImg !

void FindDuplicateImages::compareAlmost(QStringList filesList)
{
    KIPIFindDupplicateImagesPlugin::EventData *p = new KIPIFindDupplicateImagesPlugin::EventData;
    p->action   = KIPIFindDupplicateImagesPlugin::Progress;
    p->total = filesList.count();
    p->starting = true;
    QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, p));

    res = new QDict < QPtrVector < QFile > >;

    QPtrVector < ImageSimilarityData > *listRatW = new QPtrVector < ImageSimilarityData >;
    QPtrVector < ImageSimilarityData > *listRatH = new QPtrVector < ImageSimilarityData >;
    QPtrVector < ImageSimilarityData > *list;
    listRatW->setAutoDelete(true);
    listRatH->setAutoDelete(true);

    QTime debut=QTime::currentTime ();
    ImageSimilarityData *is;

    for ( QStringList::Iterator item = filesList.begin(); item != filesList.end(); ++item )
        {
        QString itemName(*item);
        QFileInfo fi(itemName);
        QString Temp = fi.dirPath();
        QString albumName = Temp.section('/', -1);

        KIPIFindDupplicateImagesPlugin::EventData *d = new KIPIFindDupplicateImagesPlugin::EventData;
        d->action   = KIPIFindDupplicateImagesPlugin::Matrix;
        d->fileName = itemName;
        d->starting = true;
        QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, d));

        if( (is = image_sim_fill_data( itemName )) != NULL )
           {
           if ( is->ratio > 1 )
              list = listRatW;
           else
              list = listRatH;

           list->resize (list->size () + 1);
           list->insert (list->size () - 1, is );
           }

        KIPIFindDupplicateImagesPlugin::EventData *r = new KIPIFindDupplicateImagesPlugin::EventData;
        r->action    = KIPIFindDupplicateImagesPlugin::Matrix;
        r->fileName  = itemName;
        r->success   = true;
        r->errString = "";
        QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, r));
        }

    kdDebug( 51000 ) << "Matrix creation time:" << debut.msecsTo(QTime::currentTime()) << endl;
    debut=QTime::currentTime ();

    QDict < QFile > *fait = new QDict < QFile >;
    list = listRatW;
    bool done=false;

    p = new KIPIFindDupplicateImagesPlugin::EventData;
    p->action   = KIPIFindDupplicateImagesPlugin::Progress;
    p->total = filesList.count();
    p->starting = true;
    QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, p));

    while( list != NULL )
        {
        if (list->size () != 1)
            {
            for (unsigned int i = 0; i < list->size (); i++)
                {
                // Create the 'ImageSimilarityData' data for the first image.
                ImageSimilarityData *i1 = list->at(i);

                if (i1 && !fait->find(i1->filename))
                {
                KIPIFindDupplicateImagesPlugin::EventData *d = new KIPIFindDupplicateImagesPlugin::EventData;
                d->action   = KIPIFindDupplicateImagesPlugin::Similar;
                d->fileName = i1->filename;
                d->starting = true;
                QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, d));

                for (unsigned int j = i + 1; j < list->size (); j++)
                        {
                        // Create the 'ImageSimilarityData' data for the second image.
                        ImageSimilarityData *i2 = list->at(j);

                        // Real images file comparison calculation.
                        float eq = image_sim_compare_fast(i1, i2, m_approximateLevel);

                        if (eq >= m_approximateLevel)   // the files are the same !
                            {
                            QPtrVector < QFile > *vect;

                            // Add file to the list.
                            if (!res->find (i1->filename))
                                {
                                vect = new QPtrVector < QFile >;
                                vect->setAutoDelete(TRUE);
                                res->insert (i1->filename, vect);
                                }
                            else
                                vect = (QPtrVector < QFile > *)res->find(i1->filename);

                            vect->resize (vect->size () + 1);
                            vect->insert (vect->size () - 1, new QFile(i2->filename));
                            fait->insert(i2->filename, new QFile(i2->filename));
                            }
                        }
                    }

               KIPIFindDupplicateImagesPlugin::EventData *e = new KIPIFindDupplicateImagesPlugin::EventData;
               e->action   = KIPIFindDupplicateImagesPlugin::Similar;
               e->fileName = i1->filename;
               e->starting = false;
               QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, e));
               }
           }

        if(!done)
           {
           list = listRatH;
           done=true;
           }
        else
            list=NULL;
        }

    kdDebug( 51000 ) << "Comparison time: " << debut.msecsTo(QTime::currentTime()) << endl;

    // End of comparison process.

    delete(fait);
    delete(listRatH);
    delete(listRatW);

    KIPIFindDupplicateImagesPlugin::EventData *e = new KIPIFindDupplicateImagesPlugin::EventData;
    e->action   = KIPIFindDupplicateImagesPlugin::Progress;
    e->starting = false;
    QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, e));
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Nota: original source code from ShowImg !

void FindDuplicateImages::compareFast(QStringList filesList)
{
    QDict < QPtrVector < QFile > >*dict = new QDict < QPtrVector < QFile > >;
    dict->setAutoDelete(true);
    res = new QDict < QPtrVector < QFile > >;
    QPtrVector < QFile > *list;

    QString size;
    QFile *file;
    int nbrF=0;

    for ( QStringList::Iterator item = filesList.begin(); item != filesList.end(); ++item )
        {
        QString itemName(*item);
        nbrF++;

        // Create a file
        file = new QFile( itemName );

        // Read the file size
        size = QString::number(QFileInfo (*file).size ());

        // if not in the table, we do it
        if ( !dict->find (size) )
            {
            list = new QPtrVector < QFile >;
            list->setAutoDelete(true);
            dict->insert (size, list);
            }

        // Read the list
        list = (QPtrVector < QFile > *)dict->find (size);

        //Add the file
        list->resize (list->size () + 1);
        list->insert (list->size () - 1, file);
        }

    // Files comparison
    QDictIterator < QPtrVector < QFile > >it (*dict);        // iterator for dict

    KIPIFindDupplicateImagesPlugin::EventData *p = new KIPIFindDupplicateImagesPlugin::EventData;
    p->action   = KIPIFindDupplicateImagesPlugin::Progress;
    p->total = filesList.count();
    p->starting = true;
    QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, p));

    while (it.current ())
        {
        QDict < QFile > *fait = new QDict < QFile >;
        list = (QPtrVector < QFile > *)it.current ();

        if (list->size () != 1)
            {
            for (unsigned int i = 0; i < list->size (); i++)
                {
                QFile *file1 = (QFile *) (list->at (i));

                KIPIFindDupplicateImagesPlugin::EventData *d = new KIPIFindDupplicateImagesPlugin::EventData;
                d->action   = KIPIFindDupplicateImagesPlugin::Exact;
                d->fileName = file1->name();
                d->starting = true;
                QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, d));

                if (!fait->find (file1->name()))
                    {
                    for (unsigned int j = i + 1; j < list->size (); j++)
                        {
                        QFile *file2 = (QFile *) (list->at (j));
                        // The files are equals

                        if (equals (file1, file2))
                            {
                            QPtrVector < QFile > *vect;

                            // Add the file

                            if (!res->find (file1->name ()))
                                {
                                vect = new QPtrVector < QFile >;
                                vect->setAutoDelete(true);
                                res->insert (file1->name (), vect);
                                }
                            else
                                vect = (QPtrVector < QFile > *)res->find (file1->name ());

                            vect->resize (vect->size () + 1);
                            vect->insert (vect->size () - 1, file2);

                            fait->insert(file2->name(), file2);
                            }
                        }
                    }
                KIPIFindDupplicateImagesPlugin::EventData *e = new KIPIFindDupplicateImagesPlugin::EventData;
                e->action   = KIPIFindDupplicateImagesPlugin::Exact;
                e->fileName = file1->name();
                e->starting = false;
                QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, e));
                }
            }
        delete(fait);
        ++it;
        }

    delete (it);

    KIPIFindDupplicateImagesPlugin::EventData *e = new KIPIFindDupplicateImagesPlugin::EventData;
    e->action   = KIPIFindDupplicateImagesPlugin::Progress;
    e->starting = false;
    QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, e));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool FindDuplicateImages::DeleteDir(QString dirname)
{
if (dirname != "")
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

#include "finddupplicateimages.moc"
