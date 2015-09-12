/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-08-08
 * Description : Blob analysis package
 *               http://opencvlibrary.sourceforge.net/cvBlobsLib
 *               Input: IplImage* binary image
 *               Output: attributes of each connected region
 *
 * Copyright (C) 2003 by Dave Grossman <dgrossman@cdr.stanford.edu>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef BLOBEXTRACTION_H
#define BLOBEXTRACTION_H

// OpenCV includes

#include "libopencv.h"

// Local includes

#include "BlobResult.h"

namespace KIPIRemoveRedEyesPlugin
{

class CBlob;

//! Extreu els blobs d'una imatge
bool BlobAnalysis(IplImage* inputImage, uchar threshold, IplImage* maskImage,
                  bool borderColor, bool findmoments, blob_vector& RegionData );

// FUNCIONS AUXILIARS

//! Fusiona dos blobs
void Subsume(blob_vector& RegionData, int, int*, CBlob*, CBlob*, bool, int, int );
//! Reallocata el vector auxiliar de blobs subsumats
int* NewSubsume(int* SubSumedRegion, int elems_inbuffer);
//! Retorna el perimetre extern d'una run lenght
double GetExternPerimeter( int start, int end, int row, int width, int height, IplImage* maskImage );

} // namespace KIPIRemoveRedEyesPlugin

#endif //BLOBEXTRACTION_H
