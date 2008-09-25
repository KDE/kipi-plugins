/*****************************************************************************/
// Copyright 2006-2007 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

/* $Id: //mondo/dng_sdk_1_2/dng_sdk/source/dng_assertions.h#1 $ */ 
/* $DateTime: 2008/03/09 14:29:54 $ */
/* $Change: 431850 $ */
/* $Author: tknoll $ */

/** \file
 * Conditionally compiled assertion check support.
 */

/*****************************************************************************/

#ifndef __dng_assertions__
#define __dng_assertions__

/*****************************************************************************/

#include "dng_flags.h"

/*****************************************************************************/

#ifndef DNG_ASSERT

/// \def qDNGDebug defined for debug builds only. Enables assertiosn and other
/// debug checks in exchange for slower processing.

#if qDNGDebug

/// Platform specific function to display an assert.
void dng_show_message (const char *s);

// Show a formatted error message.

void dng_show_message_f (const char *fmt, ...);

/// Conditionally compiled macro to check an assertion and display a message if
/// it fails and assertions are compiled in via qDNGDebug
/// \param x Predicate which must be true.
/// \param y String to display if x is not true.

#define DNG_ASSERT(x,y) { if (!(x)) dng_show_message (y); }

#else

/// Conditionally compiled macro to check an assertion and display a message if
/// it fails and assertions are compiled in via qDNGDebug
/// \param x Predicate which must be true.
/// \param y String to display if x is not true.

#define DNG_ASSERT(x,y)

#endif
#endif

/*****************************************************************************/

#ifndef DNG_REPORT
#define DNG_REPORT(x) DNG_ASSERT (false, x)
#endif

/*****************************************************************************/

#endif

/*****************************************************************************/
