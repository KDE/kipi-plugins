/*****************************************************************************/
// Copyright 2006-2007 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

/* $Id: //mondo/dng_sdk_1_2/dng_sdk/source/dng_resample.h#1 $ */ 
/* $DateTime: 2008/03/09 14:29:54 $ */
/* $Change: 431850 $ */
/* $Author: tknoll $ */

/*****************************************************************************/

#ifndef __dng_resample__
#define __dng_resample__

/*****************************************************************************/

#include "dng_assertions.h"
#include "dng_auto_ptr.h"
#include "dng_classes.h"
#include "dng_memory.h"
#include "dng_types.h"

/*****************************************************************************/

class dng_resample_function
	{
	
	public:
	
		dng_resample_function ()
			{
			}
			
		virtual ~dng_resample_function ()
			{
			}
	
		virtual real64 Extent () const = 0;
		
		virtual real64 Evaluate (real64 x) const = 0;
	
	};

/*****************************************************************************/

class dng_resample_bicubic: public dng_resample_function
	{
	
	public:
	
		virtual real64 Extent () const;
		
		virtual real64 Evaluate (real64 x) const;
		
		static const dng_resample_function & Get ();
		
	};
	
/******************************************************************************/

const uint32 kResampleSubsampleBits  = 7;
const uint32 kResampleSubsampleCount = 1 << kResampleSubsampleBits;
const uint32 kResampleSubsampleMask  = kResampleSubsampleCount - 1;

/*****************************************************************************/

class dng_resample_coords
	{
	
	protected:
	
		int32 fOrigin;
	
		AutoPtr<dng_memory_block> fCoords;
		
	public:
	
		dng_resample_coords ();
		
		virtual ~dng_resample_coords ();
		
		void Initialize (int32 srcOrigin,
						 int32 dstOrigin,
						 uint32 srcCount,
						 uint32 dstCount,
						 dng_memory_allocator &allocator);
						 
		const int32 * Coords (int32 index) const
			{
			return fCoords->Buffer_int32 () + (index - fOrigin);
			}
						 
		const int32 Pixel (int32 index) const
			{
			return Coords (index) [0] >> kResampleSubsampleBits;
			}
	
	};

/*****************************************************************************/

class dng_resample_weights
	{
	
	protected:
	
		uint32 fRadius;
		
		uint32 fWeightStep;
		
		AutoPtr<dng_memory_block> fWeights32;
		AutoPtr<dng_memory_block> fWeights16;
	
	public:
	
		dng_resample_weights ();
		
		virtual ~dng_resample_weights ();
			
		void Initialize (real64 scale,
						 const dng_resample_function &kernel,
						 dng_memory_allocator &allocator);
						 
		uint32 Radius () const
			{
			return fRadius;
			}
						 
		uint32 Width () const
			{
			return fRadius * 2;
			}
			
		int32 Offset () const
			{
			return 1 - (int32) fRadius;
			}
			
		uint32 Step () const
			{
			return fWeightStep;
			}
			
		const real32 *Weights32 (uint32 fract) const
			{
			
			DNG_ASSERT (fWeights32->Buffer (), "Weights32 is NULL");
			
			return fWeights32->Buffer_real32 () + fract * fWeightStep;
			
			}

		const int16 *Weights16 (uint32 fract) const
			{
			
			DNG_ASSERT (fWeights16->Buffer (), "Weights16 is NULL");
			
			return fWeights16->Buffer_int16 () + fract * fWeightStep;
			
			}

	};

/*****************************************************************************/

void ResampleImage (dng_host &host,
					const dng_image &srcImage,
					dng_image &dstImage,
					const dng_rect &srcBounds,
					const dng_rect &dstBounds,
					const dng_resample_function &kernel);
						
/*****************************************************************************/

#endif
	
/*****************************************************************************/
