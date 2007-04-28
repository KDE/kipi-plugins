//////////////////////////////////////////////////////////////////////////////
//
//    FUZZYCOMPARE.CPP
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
//    Foundation, Inc., 51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef FUZZYCOMPARE_H
#define FUZZYCOMPARE_H

class QImage;
class QObject;
class QStringList;
#include <qdict.h>
#include <qptrvector.h>
#include <qfile.h>
#include "compareoperation.h"

namespace KIPIFindDupplicateImagesPlugin
{
class FindDuplicateImages;
class ImageSimilarityData;

class FuzzyCompare :public CompareOperation
{
public:
    FuzzyCompare( QObject* parent, const QString& cacheDir );
    void setApproximateThreeshold( float approximateLevel ) { m_approximateLevel = approximateLevel; }
    QDict < QPtrVector < QFile > > compare(const QStringList& filesList );

protected:
    ImageSimilarityData* image_sim_fill_data(QString filename);
    float image_sim_compare_fast(ImageSimilarityData *a, ImageSimilarityData *b, float min);
    char getRed(QImage *im, int x, int y);
    char getGreen(QImage *im, int x, int y);
    char getBlue(QImage *im, int x, int y);

private:
    QObject* m_parent;
    const QString m_cacheDir;
    float m_approximateLevel;
};

}

#endif /* FUZZYCOMPARE_H */

