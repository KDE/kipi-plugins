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
//    Foundation, Inc., 51 Franklin Steet, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////

#include "fuzzycompare.h"
#include "actions.h"
#include <qstringlist.h>
#include <qapplication.h>
#include <kdebug.h>
#include "imagesimilaritydata.h"
#include <qdatetime.h>
#include <qfileinfo.h>
#include "finddupplicateimages.h"
#include <qimage.h>
#include <kimageio.h>
#include <kimageeffect.h>
#include <kstandarddirs.h>
#include <math.h>

KIPIFindDupplicateImagesPlugin::FuzzyCompare::FuzzyCompare( QObject* parent, const QString& cacheDir )
    :parent_( parent ), m_cacheDir( cacheDir )
{
}

QDict < QPtrVector < QFile > >  KIPIFindDupplicateImagesPlugin::FuzzyCompare::doFuzzyCompare( const QStringList& filesList, float approximateLevel)
{
    KIPIFindDupplicateImagesPlugin::EventData *d = new KIPIFindDupplicateImagesPlugin::EventData;
    d->action = KIPIFindDupplicateImagesPlugin::Progress;
    d->total = filesList.count()*2;
    d->starting = true;
    QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, d));

    kdDebug( 51000 ) << filesList.count() << " images to parse with Almost method..." << endl;
    QDict < QPtrVector < QFile > > res;

    QPtrVector < ImageSimilarityData > *listRatW = new QPtrVector < ImageSimilarityData >;
    QPtrVector < ImageSimilarityData > *listRatH = new QPtrVector < ImageSimilarityData >;
    QPtrVector < ImageSimilarityData > *list;
    listRatW->setAutoDelete(true);
    listRatH->setAutoDelete(true);

    QTime debut=QTime::currentTime ();
    ImageSimilarityData *is;

    for ( QStringList::ConstIterator item = filesList.begin() ; item != filesList.end() ; ++item )
    {
        QString itemName(*item);
        QFileInfo fi(itemName);
        QString Temp = fi.dirPath();
        QString albumName = Temp.section('/', -1);

        d = new KIPIFindDupplicateImagesPlugin::EventData;
        d->action = KIPIFindDupplicateImagesPlugin::Matrix;
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

            d = new KIPIFindDupplicateImagesPlugin::EventData;
            d->action = KIPIFindDupplicateImagesPlugin::Matrix;
            d->fileName = itemName;
            d->starting = false;
            d->success = true;
            d->errString = "";
            QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, d));
        }
        else
        {
            d = new KIPIFindDupplicateImagesPlugin::EventData;
            d->action = KIPIFindDupplicateImagesPlugin::Matrix;
            d->fileName = itemName;
            d->starting = false;
            d->success = false;
            d->errString = "";
            QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, d));
        }
    }

    kdDebug( 51000 ) << "Matrix creation time:" << debut.msecsTo(QTime::currentTime()) << endl;
    debut = QTime::currentTime ();

    QDict < QFile > *fait = new QDict < QFile >;
    list = listRatW;
    bool done = false;

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
                    d = new KIPIFindDupplicateImagesPlugin::EventData;
                    d->action = KIPIFindDupplicateImagesPlugin::Similar;
                    d->fileName = i1->filename;
                    d->starting = true;
                    d->success = false;
                    QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, d));

                    for (unsigned int j = i + 1; j < list->size (); j++)
                    {
                        // Create the 'ImageSimilarityData' data for the second image.
                        ImageSimilarityData *i2 = list->at(j);

                        // Real images file comparison calculation.
                        float eq = image_sim_compare_fast(i1, i2, approximateLevel);

                        if (eq >= approximateLevel)   // the files are the same !
                        {
                            QPtrVector < QFile > *vect;

                            // Add file to the list.
                            if (!res.find (i1->filename))
                            {
                                vect = new QPtrVector < QFile >;
                                vect->setAutoDelete(true);
                                res.insert (i1->filename, vect);
                            }
                            else
                                vect = (QPtrVector < QFile > *)res.find(i1->filename);

                            vect->resize (vect->size () + 1);
                            vect->insert (vect->size () - 1, new QFile(i2->filename));
                            fait->insert(i2->filename, new QFile(i2->filename));
                        }
                    }
                }

                d = new KIPIFindDupplicateImagesPlugin::EventData;
                d->action = KIPIFindDupplicateImagesPlugin::Similar;
                d->fileName = i1->filename;
                d->starting = false;
                d->success = true;
                QApplication::postEvent(parent_, new QCustomEvent(QEvent::User, d));
            }
        }

        if(!done)
        {
            list = listRatH;
            done = true;
        }
        else
            list = NULL;
    }

    kdDebug( 51000 ) << "Comparison time: " << debut.msecsTo(QTime::currentTime()) << endl;

    // End of comparison process.

    delete(fait);
    delete(listRatH);
    delete(listRatW);

    return res;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Nota: original source code from ShowImg !

KIPIFindDupplicateImagesPlugin::ImageSimilarityData* KIPIFindDupplicateImagesPlugin::FuzzyCompare::image_sim_fill_data(QString filename)
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

    QFileInfo info(m_cacheDir + QFileInfo(filename).absFilePath()+".dat");

    if(info.exists())
    {
        QFile f(m_cacheDir+QFileInfo(filename).absFilePath()+".dat");
        if ( f.open(IO_ReadOnly) )
        {
            QDataStream s( &f );
            s >> sd->ratio;
            for(int i=0 ; i<PAS*PAS ; i++) s >> sd->avg_r[i];
            for(int i=0 ; i<PAS*PAS ; i++) s >> sd->avg_g[i];
            for(int i=0 ; i<PAS*PAS ; i++) s >> sd->avg_b[i];
            f.close();
        }

        sd->filled = true;
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
                    r += getRed(pixbuf, x, y);
                    g += getGreen(pixbuf, x, y);
                    b += getBlue(pixbuf, x, y);
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

    sd->filled = true;
    sd->ratio=((float)w)/h;
    delete(pixbuf);

    // Saving the data.

    QFile f(m_cacheDir+QFileInfo(filename).absFilePath()+".dat");
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

float KIPIFindDupplicateImagesPlugin::FuzzyCompare::image_sim_compare_fast(ImageSimilarityData *a, ImageSimilarityData *b, float min)
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

char KIPIFindDupplicateImagesPlugin::FuzzyCompare::getRed(QImage* im, int x, int y)
{
    return qRed(im->pixel(x, y));
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Nota: original source code from ShowImg !

char KIPIFindDupplicateImagesPlugin::FuzzyCompare::getGreen(QImage* im, int x, int y)
{
    return qGreen(im->pixel(x, y));
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Nota: original source code from ShowImg !

char KIPIFindDupplicateImagesPlugin::FuzzyCompare::getBlue(QImage* im, int x, int y)
{
    return qBlue(im->pixel(x, y));
}


