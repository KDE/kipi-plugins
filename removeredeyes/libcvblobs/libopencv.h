/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-06-16
 * @brief  Wrapper for OpenCV header files
 *
 * @author Copyright (C) 2012 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef LIB_OPEN_CV_H
#define LIB_OPEN_CV_H

// Local includes

#include "BlobLibraryConfiguration.h"

// OpenCV includes

// To avoid broken compilation with OpenCV >= 2.0
#undef HAVE_CONFIG_H

#if defined(__APPLE__)
#include <opencv2/opencv.hpp>
#include <opencv2/legacy/compat.hpp>
#include <opencv/cvaux.h>
#else
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/cxcore.h>
#include <opencv2/highgui/highgui_c.h>
#endif

#ifdef MATRIXCV_ACTIU
#include <opencv/matrixCV.h>
#else
//! Vector de doubles
typedef std::vector<double> double_stl_vector;
#endif // MATRIXCV_ACTIU

#endif // LIB_OPEN_CV_H
