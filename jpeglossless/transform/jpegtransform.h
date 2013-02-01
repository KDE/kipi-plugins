/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-06-08
 * Description : Lossless JPEG files transformations.
 *
 * Copyright (C) 2004      by Ralf Hoelzer <kde at ralfhoelzer.com>
 * Copyright (C) 2004-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef JPEGTRANSFORM_H
#define JPEGTRANSFORM_H

// Local includes

#include "kpmetadata.h"

using namespace KIPIPlugins;

namespace KIPIJPEGLossLessPlugin
{

/**
   If the picture is displayed according to the exif orientation tag,
   the user will request rotating operations relative to what he sees,
   and that is the picture rotated according to the EXIF tag.
   So the operation requested and the given EXIF angle must be combined.
   E.g. if orientation is "6" (rotate 90 clockwiseto show correctly) 
   and the user selects 180 clockwise, the operation is 270.
   If the user selected 270, the operation would be None (and clearing the exif tag).

   This requires to describe the transformations in a model which 
   cares for both composing (180+90=270) and eliminating (180+180=no action),
   as well as the non-commutative nature of the operations (vflip+90 is not 90+vflip)

   All 2D transformations can be described by a 2x3 matrix, see QWMatrix.
   All transformations needed here - rotate 90, 180, 270, flipV, flipH - 
   can be described in a 2x2 matrix with the values 0,1,-1
   (because flipping is expressed by changing the sign only,
    and sine and cosine of 90, 180 and 270 are either 0,1 or -1).

    x' = m11 x + m12 y
    y' = m21 x + m22 y

   Moreover, all combinations of these rotate/flip operations result in one of the eight
   matrices defined below.
   (I did not proof that mathematically, but empirically)
*/

class Matrix 
{

public:

    Matrix()
    {
            set( 1, 0, 0, 1 );
    }

    Matrix &operator*=(const Matrix &ma)
    {
        set(ma.m[0][0]*m[0][0] + ma.m[0][1]*m[1][0],  ma.m[0][0]*m[0][1] + ma.m[0][1]*m[1][1],
            ma.m[1][0]*m[0][0] + ma.m[1][1]*m[1][0],  ma.m[1][0]*m[0][1] + ma.m[1][1]*m[1][1] );
        return *this;
    }

    bool operator==(const Matrix &ma) const
    {
        return m[0][0]==ma.m[0][0] &&
               m[0][1]==ma.m[0][1] &&
               m[1][0]==ma.m[1][0] &&
               m[1][1]==ma.m[1][1];
    }

    bool operator!=(const Matrix &ma) const
    {
        return !(*this==ma);
    }

    static const Matrix none;                   //( 1,  0,  0,  1)
    static const Matrix rotate90;               //( 0, -1,  1,  0)
    static const Matrix rotate180;              //(-1,  0,  0, -1)
    static const Matrix rotate270;              //( 0,  1, -1,  0)
    static const Matrix flipHorizontal;         //(-1,  0,  0,  1)
    static const Matrix flipVertical;           //( 1,  0,  0, -1)
    static const Matrix rotate90flipHorizontal; //( 0,  1,  1,  0), first rotate, then flip
    static const Matrix rotate90flipVertical;   //( 0, -1, -1,  0), first rotate, then flip

protected:

    Matrix(int m11, int m12, int m21, int m22)
    {
        set(m11, m12, m21, m22);
    }

    void set(int m11, int m12, int m21, int m22)
    {
        m[0][0]=m11;
        m[0][1]=m12;
        m[1][0]=m21;
        m[1][1]=m22;
    }

    int m[2][2];
};

bool transformJPEG(const QString& src, const QString& dest, Matrix &action, QString& err);

void convertTransform(Matrix& action, JXFORM_CODE& flip, JXFORM_CODE& rotate);

void getExifAction(Matrix& action, KPMetadata::ImageOrientation exifOrientation);

}  // namespace KIPIJPEGLossLessPlugin

#endif  // JPEGTRANSFORM_H
