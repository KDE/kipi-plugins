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

#ifndef IMAGESIMILARITYDATA_H
#define IMAGESIMILARITYDATA_H

#define PAS 32

extern "C"
{
#include <stdlib.h>
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Nota: original source code from ShowImg !
// A class to store datas to look for similaties of 2 images.

namespace KIPIFindDupplicateImagesPlugin
{

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

}


#endif /* IMAGESIMILARITYDATA_H */

