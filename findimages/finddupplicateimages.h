//////////////////////////////////////////////////////////////////////////////
//
//    FINDDUPPLICATEIMAGES.H
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


#ifndef FINDDUPPLICATEIMAGES_H
#define FINDDUPPLICATEIMAGES_H

// Include files for Qt

#include <qobject.h>
#include <qstring.h>
#include <qthread.h>
#include <qstringlist.h>
#include <qptrvector.h>
#include <qdict.h>

// Includes file for libKIPI.

#include <libkipi/interface.h>

class QProgressDialog;
class QFile;
class QCustomEvent;
class QMutex;

class KConfig;

namespace KIPIFindDupplicateImagesPlugin
{

class ImageSimilarityData;
class FindDuplicateDialog;

class FindDuplicateImages : public QObject, public QThread
{
    Q_OBJECT

public:
    FindDuplicateImages( KIPI::Interface* interface, QObject *parent=0);
    ~FindDuplicateImages();

    virtual void run();

    bool execDialog();
    void showResult();
    void compareAlbums();                                   // Launch the dialog box for Albums selection
    // before comparison.

public slots:
    void slotUpdateCache(QStringList fromDirs);
    void slotClearCache(QStringList fromDir);
    void slotClearAllCache(void);

protected:
    KConfig              *config;
    QString               m_imagesFileFilter;
    QProgressDialog      *pdCache;
    FindDuplicateDialog  *m_findDuplicateDialog;
    float                 m_approximateLevel;

    QDict < QPtrVector < QFile > > compareFast(QStringList filesList );// Launch the exact comparison.
    QDict < QPtrVector < QFile > > compareAlmost( const QStringList& filesList); // Launch the approximative comparison.

    float image_sim_compare(ImageSimilarityData *a, ImageSimilarityData *b);
    void writeSettings(void);
    void readSettings(void);
    void updateCache(QString fromDir);
    bool DeleteDir(QString dirname);
    bool deldir(QString dirname);

    QStringList filesList;
    bool isCompareAlmost;
    QObject *parent_;
    QDict < QPtrVector < QFile > > m_res;
    KIPI::Interface* m_interface;
    QString m_cacheDir;

    friend class FuzzyCompare;
};

}  // NameSpace KIPIFindDupplicateImagesPlugin

#endif  // FINDDUPPLICATEIMAGES_H

