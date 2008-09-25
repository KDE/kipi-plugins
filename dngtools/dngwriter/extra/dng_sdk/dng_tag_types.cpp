/*****************************************************************************/
// Copyright 2006 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

/* $Id: //mondo/dng_sdk_1_2/dng_sdk/source/dng_tag_types.cpp#1 $ */ 
/* $DateTime: 2008/03/09 14:29:54 $ */
/* $Change: 431850 $ */
/* $Author: tknoll $ */

/*****************************************************************************/

#include "dng_tag_types.h"

/*****************************************************************************/

uint32 TagTypeSize (uint32 tagType)
	{
	
	switch (tagType)
		{
		
		case ttByte:
		case ttAscii:
		case ttSByte:
		case ttUndefined:
			{
			return 1;
			}

		case ttShort:
		case ttSShort:
		case ttUnicode:
			{
			return 2;
			}

		case ttLong:
		case ttSLong:
		case ttFloat:
		case ttIFD:
			{
			return 4;
			}

		case ttRational:
		case ttDouble:
		case ttSRational:
		case ttComplex:
			{
			return 8;
			}

		default:
			break;
			
		}
		
	return 0;
	
	}

/*****************************************************************************/
