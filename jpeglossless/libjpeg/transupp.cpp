/*
 * transupp.cpp
 *
 * Copyright (C) 1997-2009, Thomas G. Lane, Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying libjpeg62.README file.
 *
 * This file contains image transformation routines and other utility code
 * used by the jpegtran sample application.  These are NOT part of the core
 * JPEG library.  But we keep these routines separate from jpegtran.c to
 * ease the task of maintaining jpegtran-like programs that have other user
 * interfaces.
 */

/*
 * Define libjpeg_EXPORTS: kde-win emerged jpeg lib uses this define to 
 * decide wether to make dllimport (by default) or dllexport. We need to 
 * export. 
 */

#define libjpeg_EXPORTS

/* Although this file really shouldn't have access to the library internals,
 * it's helpful to let it call jround_up() and jcopy_block_row().
 */
#define JPEG_INTERNALS

// LibJPEG includes

extern "C"
{
#include "jinclude.h"
#include "jpeglib.h"
}

// transupp.c appears to be tied rather tightly to the library version.
// For example, the one from libjpeg v6b will not work properly with
// libjpeg v8.  So include the correct one for the current version of
// libjpeg.

#if JPEG_LIB_VERSION < 70
#include "transupp.v6b.cpp"
#elif JPEG_LIB_VERSION < 80
#include "transupp.v7.cpp"
#else
#include "transupp.v8a.cpp"
#endif

