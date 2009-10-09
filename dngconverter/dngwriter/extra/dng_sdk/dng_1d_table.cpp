/*****************************************************************************/
// Copyright 2006 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

/* $Id: //mondo/dng_sdk_1_2/dng_sdk/source/dng_1d_table.cpp#1 $ */ 
/* $DateTime: 2008/03/09 14:29:54 $ */
/* $Change: 431850 $ */
/* $Author: tknoll $ */

/*****************************************************************************/

#include "dng_1d_table.h"
#include "dng_1d_function.h"
#include "dng_memory.h"
#include "dng_utils.h"

/*****************************************************************************/

dng_1d_table::dng_1d_table ()

	:	fBuffer ()
	,	fTable  (NULL)
	
	{
	
	}

/*****************************************************************************/

dng_1d_table::~dng_1d_table ()
	{
	
	}
		
/*****************************************************************************/

void dng_1d_table::Initialize (dng_memory_allocator &allocator,
							   const dng_1d_function &function,
							   bool subSample)
	{
	
	uint32 j;
	uint32 k;
	
	fBuffer.Reset (allocator.Allocate ((kTableSize + 2) * sizeof (real32)));
	
	fTable = fBuffer->Buffer_real32 ();
	
	uint32 step = subSample ? (kTableSize >> 8) : 1;
	
	for (j = 0; j <= kTableSize; j += step)
		{
		
		real64 x = j * (1.0 / (real64) kTableSize);
		
		real64 y = function.Evaluate (x);
		
		fTable [j] = (real32) y;
		
		}
		
	fTable [kTableSize + 1] = fTable [kTableSize];
	
	if (step != 1)
		{
		
		real64 invStep = 1.0 / (real64) step;
		
		for (j = 0; j < kTableSize; j += step)
			{
			
			real64 y0 = fTable [j       ];
			real64 y1 = fTable [j + step];
			
			real64 delta = (y1 - y0) * invStep;
			
			for (k = 1; k < step; k++)
				{
				
				y0 += delta;
				
				fTable [j + k] = (real32) y0;
				
				}
						
			}
		
		}
	
	}

/*****************************************************************************/

void dng_1d_table::Expand16 (uint16 *table16) const
	{
	
	real64 step = (real64) kTableSize / 65535.0;
	
	real64 y0 = fTable [0];
	real64 y1 = fTable [1];
	
	real64 base  = y0 * 65535.0 + 0.5;
	real64 slope = (y1 - y0) * 65535.0;
	
	uint32 index = 1;
	real64 fract = 0.0;
	
	for (uint32 j = 0; j < 0x10000; j++)
		{
		
		table16 [j] = (uint16) (base + slope * fract);
		
		fract += step;
		
		if (fract > 1.0)
			{
			
			index += 1;
			fract -= 1.0;
			
			y0 = y1;
			y1 = fTable [index];
			
			base  = y0 * 65535.0 + 0.5;
			slope = (y1 - y0) * 65535.0;
			
			}
		
		}
	
	}

/*****************************************************************************/
