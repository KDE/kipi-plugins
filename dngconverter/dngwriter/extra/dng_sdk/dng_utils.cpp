/*****************************************************************************/
// Copyright 2006-2007 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

/* $Id: //mondo/dng_sdk_1_2/dng_sdk/source/dng_utils.cpp#1 $ */ 
/* $DateTime: 2008/03/09 14:29:54 $ */
/* $Change: 431850 $ */
/* $Author: tknoll $ */

/*****************************************************************************/

#include "dng_utils.h"
#include "dng_assertions.h"

#if qMacOS
#include <CoreServices/CoreServices.h>
#endif

#if qWinOS
#include <windows.h>
#else
#include <sys/time.h>
#endif

/*****************************************************************************/

#if qDNGDebug

void dng_show_message (const char *s)
	{
	
	#if qDNGPrintMessages
	
		{
	
		fprintf (stderr, "%s\n", s);
		
		}
	
	#elif qMacOS
	
		{

		char ss [256];
		
		strcpy (ss, s);
		
		uint32 len = strlen (ss);
		
		for (uint32 j = len + 1; j >= 1; j--)
			ss [j] = ss [j - 1];
			
		ss [0] = (char) len;
		
		DebugStr ((unsigned char *) ss);
		
		}
	
	#elif qWinOS
	
		{
	
		MessageBoxA (NULL, (LPSTR) s, NULL, MB_OK);
		
		}
	
	#endif

	}
	
#endif

/*****************************************************************************/

#if qDNGDebug

void dng_show_message_f (const char *fmt, ... )
	{
	
	char buffer [1024];
	
	va_list ap;
	va_start (ap, fmt);

	vsnprintf (buffer, sizeof (buffer), fmt, ap);
	
	va_end (ap);
	
	dng_show_message (buffer);
	
	}

#endif

/*****************************************************************************/

real64 TickTimeInSeconds ()
	{
	
	#if qWinOS
	
	// One might think it prudent to cache the frequency here, however
	// low-power CPU modes can, and do, change the value returned.
	// Thus the frequencey needs to be retrieved each time.

	LARGE_INTEGER freq;
	LARGE_INTEGER cycles;

	QueryPerformanceFrequency (&freq);
	QueryPerformanceCounter (&cycles);

	return (real64)cycles.QuadPart / (real64)freq.QuadPart;
	
	#else

	// Perhaps a better call exists. (e.g. avoid adjtime effects)

	struct timeval tv;
	
	gettimeofday (&tv, NULL);

	return tv.tv_sec + tv.tv_usec / 1000000.0;
	
	#endif

	}

/*****************************************************************************/

dng_timer::dng_timer (const char *message)

	:	fMessage   (message             )
	,	fStartTime (TickTimeInSeconds ())
	
	{

	}

/*****************************************************************************/

dng_timer::~dng_timer ()
	{

	real64 totalTime = TickTimeInSeconds () - fStartTime;
	
	fprintf (stderr, "%s: %0.3f sec\n", fMessage, totalTime);

	}

/*****************************************************************************/
