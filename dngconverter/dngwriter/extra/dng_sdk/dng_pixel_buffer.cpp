/*****************************************************************************/
// Copyright 2006-2007 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

/* $Id: //mondo/dng_sdk_1_2/dng_sdk/source/dng_pixel_buffer.cpp#1 $ */ 
/* $DateTime: 2008/03/09 14:29:54 $ */
/* $Change: 431850 $ */
/* $Author: tknoll $ */

/*****************************************************************************/

#include "dng_pixel_buffer.h"
#include "dng_bottlenecks.h"
#include "dng_exceptions.h"
#include "dng_flags.h"
#include "dng_tag_types.h"
#include "dng_utils.h"

/*****************************************************************************/

void OptimizeOrder (const void *&sPtr,
					void *&dPtr,
					uint32 sPixelSize,
					uint32 dPixelSize,
					uint32 &count0,
					uint32 &count1,
					uint32 &count2,
					int32 &sStep0,
					int32 &sStep1,
					int32 &sStep2,
					int32 &dStep0,
					int32 &dStep1,
					int32 &dStep2)
	{
	
	uint32 step0;
	uint32 step1;
	uint32 step2;
	
	// Optimize the order for the data that is most spread out.
							   
	uint32 sRange = Abs_int32 (sStep0) * (count0 - 1) +
					Abs_int32 (sStep1) * (count1 - 1) +
					Abs_int32 (sStep2) * (count2 - 1);
							   
	uint32 dRange = Abs_int32 (dStep0) * (count0 - 1) +
					Abs_int32 (dStep1) * (count1 - 1) +
					Abs_int32 (dStep2) * (count2 - 1);
							   
	if (dRange >= sRange)
		{						
	
		if (dStep0 < 0)
			{
			
			sPtr = (const void *)
				   (((const uint8 *) sPtr) + (int32)(count0 - 1) * sStep0 * (int32)sPixelSize);
				   
			dPtr = (void *)
				   (((uint8 *) dPtr) + (int32)(count0 - 1) * dStep0 * (int32)dPixelSize);
				   
			sStep0 = -sStep0;
			dStep0 = -dStep0;
			
			}
		
		if (dStep1 < 0)
			{
			
			sPtr = (const void *)
				   (((const uint8 *) sPtr) + (int32)(count1 - 1) * sStep1 * (int32)sPixelSize);
				   
			dPtr = (void *)
				   (((uint8 *) dPtr) + (int32)(count1 - 1) * dStep1 * (int32)dPixelSize);
				   
			sStep1 = -sStep1;
			dStep1 = -dStep1;
			
			}
		
		if (dStep2 < 0)
			{
			
			sPtr = (const void *)
				   (((const uint8 *) sPtr) + (int32)(count2 - 1) * sStep2 * (int32)sPixelSize);
				   
			dPtr = (void *)
				   (((uint8 *) dPtr) + (int32)(count2 - 1) * dStep2 * (int32)dPixelSize);
				   
			sStep2 = -sStep2;
			dStep2 = -dStep2;
			
			}
		
		step0 = (uint32) dStep0;
		step1 = (uint32) dStep1;
		step2 = (uint32) dStep2;
		
		}
		
	else
		{						
	
		if (sStep0 < 0)
			{
			
			sPtr = (const void *)
				   (((const uint8 *) sPtr) + (int32)(count0 - 1) * sStep0 * (int32)sPixelSize);
				   
			dPtr = (void *)
				   (((uint8 *) dPtr) + (int32)(count0 - 1) * dStep0 * (int32)dPixelSize);
				   
			sStep0 = -sStep0;
			dStep0 = -dStep0;
			
			}
		
		if (sStep1 < 0)
			{
			
			sPtr = (const void *)
				   (((const uint8 *) sPtr) + (int32)(count1 - 1) * sStep1 * (int32)sPixelSize);
				   
			dPtr = (void *)
				   (((uint8 *) dPtr) + (int32)(count1 - 1) * dStep1 * (int32)dPixelSize);
				   
			sStep1 = -sStep1;
			dStep1 = -dStep1;
			
			}
		
		if (sStep2 < 0)
			{
			
			sPtr = (const void *)
				   (((const uint8 *) sPtr) + (int32)(count2 - 1) * sStep2 * (int32)sPixelSize);
				   
			dPtr = (void *)
				   (((uint8 *) dPtr) + (int32)(count2 - 1) * dStep2 * (int32)dPixelSize);
				   
			sStep2 = -sStep2;
			dStep2 = -dStep2;
			
			}
		
		step0 = (uint32) sStep0;
		step1 = (uint32) sStep1;
		step2 = (uint32) sStep2;
		
		}
	
	if (count0 == 1) step0 = 0xFFFFFFFF;
	if (count1 == 1) step1 = 0xFFFFFFFF;
	if (count2 == 1) step2 = 0xFFFFFFFF;
	
	uint32 index0;
	uint32 index1;
	uint32 index2;
	
	if (step0 >= step1)
		{
		
		if (step1 >= step2)
			{
			index0 = 0;
			index1 = 1;
			index2 = 2;
			}
			
		else if (step2 >= step0)
			{
			index0 = 2;
			index1 = 0;
			index2 = 1;
			}
			
		else
			{
			index0 = 0;
			index1 = 2;
			index2 = 1;
			}
		
		}
		
	else
		{
		
		if (step0 >= step2)
			{
			index0 = 1;
			index1 = 0;
			index2 = 2;
			}
			
		else if (step2 >= step1)
			{
			index0 = 2;
			index1 = 1;
			index2 = 0;
			}
			
		else
			{
			index0 = 1;
			index1 = 2;
			index2 = 0;
			}
		
		}
		
	uint32 count [3];
	
	count [0] = count0;
	count [1] = count1;
	count [2] = count2;
	
	count0 = count [index0];
	count1 = count [index1];
	count2 = count [index2];
	
	int32 step [3];
	
	step [0] = sStep0;
	step [1] = sStep1;
	step [2] = sStep2;
	
	sStep0 = step [index0];
	sStep1 = step [index1];
	sStep2 = step [index2];
	
	step [0] = dStep0;
	step [1] = dStep1;
	step [2] = dStep2;
	
	dStep0 = step [index0];
	dStep1 = step [index1];
	dStep2 = step [index2];
	
	if (sStep0 == ((int32) count1) * sStep1 &&
		dStep0 == ((int32) count1) * dStep1)
		{
		count1 *= count0;
		count0 = 1;
		}
	
	if (sStep1 == ((int32) count2) * sStep2 &&
		dStep1 == ((int32) count2) * dStep2)
		{
		count2 *= count1;
		count1 = 1;
		}
	
	}

/*****************************************************************************/

void OptimizeOrder (const void *&sPtr,
					uint32 sPixelSize,
					uint32 &count0,
					uint32 &count1,
					uint32 &count2,
					int32 &sStep0,
					int32 &sStep1,
					int32 &sStep2)
	{
	
	void *dPtr = NULL;
	
	int32 dStep0 = sStep0;
	int32 dStep1 = sStep1;
	int32 dStep2 = sStep2;
	
	OptimizeOrder (sPtr,
				   dPtr,
				   sPixelSize,
				   sPixelSize,
				   count0,
				   count1,
				   count2,
				   sStep0,
				   sStep1,
				   sStep2,
				   dStep0,
				   dStep1,
				   dStep2);
	
	}

/*****************************************************************************/

void OptimizeOrder (void *&dPtr,
					uint32 dPixelSize,
					uint32 &count0,
					uint32 &count1,
					uint32 &count2,
					int32 &dStep0,
					int32 &dStep1,
					int32 &dStep2)
	{
	
	const void *sPtr = NULL;
	
	int32 sStep0 = dStep0;
	int32 sStep1 = dStep1;
	int32 sStep2 = dStep2;
	
	OptimizeOrder (sPtr,
				   dPtr,
				   dPixelSize,
				   dPixelSize,
				   count0,
				   count1,
				   count2,
				   sStep0,
				   sStep1,
				   sStep2,
				   dStep0,
				   dStep1,
				   dStep2);
	
	}

/*****************************************************************************/

dng_pixel_buffer::dng_pixel_buffer ()

	:	fArea       ()
	,	fPlane      (0)
	,	fPlanes     (1)
	,	fRowStep    (1)
	,	fColStep    (1)
	,	fPlaneStep  (1)
	,	fPixelType  (ttUndefined)
	,	fPixelSize  (0)
	,	fPixelRange (0)
	,	fData       (NULL)
	,	fDirty      (true)
	
	{
	
	}
							
/*****************************************************************************/

dng_pixel_buffer::dng_pixel_buffer (const dng_pixel_buffer &buffer)

	:	fArea       (buffer.fArea)
	,	fPlane      (buffer.fPlane)
	,	fPlanes     (buffer.fPlanes)
	,	fRowStep    (buffer.fRowStep)
	,	fColStep    (buffer.fColStep)
	,	fPlaneStep  (buffer.fPlaneStep)
	,	fPixelType  (buffer.fPixelType)
	,	fPixelSize  (buffer.fPixelSize)
	,	fPixelRange (buffer.fPixelRange)
	,	fData       (buffer.fData)
	,	fDirty      (buffer.fDirty)
	
	{
	
	}
							
/*****************************************************************************/

dng_pixel_buffer & dng_pixel_buffer::operator= (const dng_pixel_buffer &buffer)
	{
	
	fArea       = buffer.fArea;
	fPlane      = buffer.fPlane;
	fPlanes     = buffer.fPlanes;
	fRowStep    = buffer.fRowStep;
	fColStep    = buffer.fColStep;
	fPlaneStep  = buffer.fPlaneStep;
	fPixelType  = buffer.fPixelType;
	fPixelSize  = buffer.fPixelSize;
	fPixelType  = buffer.fPixelType;
	fPixelRange = buffer.fPixelRange;
	fData       = buffer.fData;
	fDirty      = buffer.fDirty;
	
	return *this;
	
	}

/*****************************************************************************/

dng_pixel_buffer::~dng_pixel_buffer ()
	{
	
	}
							
/*****************************************************************************/

#if qDebugPixelType


void dng_pixel_buffer::CheckPixelType (uint32 pixelType) const
	{
	
	if (fPixelType != pixelType)
		{
		
		DNG_REPORT ("Pixel type access mismatch");
		
		}
	
	}

#endif

/*****************************************************************************/

uint32 dng_pixel_buffer::PixelRange () const
	{
	
	if (fPixelRange)
		{
		return fPixelRange;
		}
		
	switch (fPixelType)
		{
		
		case ttByte:
		case ttSByte:
			{
			return 0x0FF;
			}
			
		case ttShort:
		case ttSShort:
			{
			return 0x0FFFF;
			}
			
		case ttLong:
		case ttSLong:
			{
			return 0xFFFFFFFF;
			}
			
		default:
			break;
			
		}
	
	return 0;
	
	}
							
/*****************************************************************************/

void dng_pixel_buffer::SetConstant (const dng_rect &area,
									uint32 plane,
									uint32 planes,
									uint32 value)
	{
	
	uint32 rows = area.H ();
	uint32 cols = area.W ();
	
	void *dPtr = DirtyPixel (area.t,
					    	 area.l,
					    	 plane);
					    
	int32 dRowStep   = fRowStep;
	int32 dColStep   = fColStep;
	int32 dPlaneStep = fPlaneStep;
	
	OptimizeOrder (dPtr,
				   fPixelSize,
				   rows,
				   cols,
				   planes,
				   dRowStep,
				   dColStep,
				   dPlaneStep);
				   
	switch (fPixelSize)
		{
		
		case 1:
			{
			
			if (rows == 1 && cols == 1 && dPlaneStep == 1 && value == 0)
				{
				
				DoZeroBytes (dPtr, planes);
				
				}
				
			else
				{
				
				DoSetArea8 ((uint8 *) dPtr,
							(uint8) value,
							rows,
							cols,
							planes,
							dRowStep,
							dColStep,
							dPlaneStep);
						 
				}
				
			break;
			
			}
				   
		case 2:
			{
			
			if (rows == 1 && cols == 1 && dPlaneStep == 1 && value == 0)
				{
				
				DoZeroBytes (dPtr, planes << 1);
				
				}
				
			else
				{
				
				DoSetArea16 ((uint16 *) dPtr,
							 (uint16) value,
							 rows,
							 cols,
							 planes,
							 dRowStep,
							 dColStep,
							 dPlaneStep);
						 
				}
				
			break;
			
			}
				   
		case 4:
			{
			
			if (rows == 1 && cols == 1 && dPlaneStep == 1 && value == 0)
				{
				
				DoZeroBytes (dPtr, planes << 2);
				
				}
				
			else
				{
				
				DoSetArea32 ((uint32 *) dPtr,
							 value,
							 rows,
							 cols,
							 planes,
							 dRowStep,
							 dColStep,
							 dPlaneStep);
						 
				}
				
			break;
			
			}
			
		default:
			{
			
			ThrowNotYetImplemented ();
			
			}
			
		}
				   
	}
		
/*****************************************************************************/

void dng_pixel_buffer::SetZero (const dng_rect &area,
					   			uint32 plane,
					   			uint32 planes)
	{
					   
	uint32 value = 0;
				   
	switch (fPixelType)
		{
		
		case ttByte:
		case ttShort:
		case ttLong:
		case ttFloat:
			{
			break;
			}
			
		case ttSShort:
			{
			value = 0x8000;
			break;
			}

		default:
			{
			
			ThrowNotYetImplemented ();
			
			}
			
		}
		
	SetConstant (area,
				 plane,
				 planes,
				 value);
				   
	}
		
/*****************************************************************************/

void dng_pixel_buffer::CopyArea (const dng_pixel_buffer &src,
					   			 const dng_rect &area,
					   			 uint32 srcPlane,
					   			 uint32 dstPlane,
					   			 uint32 planes)
	{
	
	uint32 rows = area.H ();
	uint32 cols = area.W ();
	
	const void *sPtr = src.ConstPixel (area.t,
								  	   area.l,
								  	   srcPlane);
								  
	void *dPtr = DirtyPixel (area.t,
					   		 area.l,
					    	 dstPlane);
	
	int32 sRowStep   = src.fRowStep;
	int32 sColStep   = src.fColStep;
	int32 sPlaneStep = src.fPlaneStep;
	
	int32 dRowStep   = fRowStep;
	int32 dColStep   = fColStep;
	int32 dPlaneStep = fPlaneStep;
	
	OptimizeOrder (sPtr,
				   dPtr,
				   src.fPixelSize,
				   fPixelSize,
				   rows,
				   cols,
				   planes,
				   sRowStep,
				   sColStep,
				   sPlaneStep,
				   dRowStep,
				   dColStep,
				   dPlaneStep);
				   
	if (fPixelType == src.fPixelType)
		{
		
		if (rows == 1 && cols == 1 && sPlaneStep == 1 && dPlaneStep == 1)
			{
			
			DoCopyBytes (sPtr,
						 dPtr, 
						 planes * fPixelSize);
			
			}
			
		else switch (fPixelSize)
			{
			
			case 1:
				{
				
				DoCopyArea8 ((const uint8 *) sPtr,
							 (uint8 *) dPtr,
							 rows,
							 cols,
							 planes,
							 sRowStep,
							 sColStep,
							 sPlaneStep,
							 dRowStep,
							 dColStep,
							 dPlaneStep);
				
				break;
				
				}
				
			case 2:
				{
				
				DoCopyArea16 ((const uint16 *) sPtr,
							  (uint16 *) dPtr,
							  rows,
							  cols,
							  planes,
							  sRowStep,
							  sColStep,
							  sPlaneStep,
							  dRowStep,
							  dColStep,
							  dPlaneStep);
				
				break;
				
				}
				
			case 4:
				{
				
				DoCopyArea32 ((const uint32 *) sPtr,
							  (uint32 *) dPtr,
							  rows,
							  cols,
							  planes,
							  sRowStep,
							  sColStep,
							  sPlaneStep,
							  dRowStep,
							  dColStep,
							  dPlaneStep);
				
				break;
				
				}
				
			default:
				{
				
				ThrowNotYetImplemented ();
				
				}
				
			}
		
		}
		
	else if (src.fPixelType == ttByte)
		{
		
		switch (fPixelType)
			{
			
			case ttShort:
				{
				
				DoCopyArea8_16 ((const uint8 *) sPtr,
							    (uint16 *) dPtr,
							    rows,
							    cols,
							    planes,
							    sRowStep,
							    sColStep,
							    sPlaneStep,
							    dRowStep,
							    dColStep,
							    dPlaneStep);
				
				break;
				
				}
				
			case ttSShort:
				{
				
				DoCopyArea8_S16 ((const uint8 *) sPtr,
							     (int16 *) dPtr,
							     rows,
							     cols,
							     planes,
							     sRowStep,
							     sColStep,
							     sPlaneStep,
							     dRowStep,
							     dColStep,
							     dPlaneStep);
				
				break;
				
				}
				
			case ttLong:
				{
				
				DoCopyArea8_32 ((const uint8 *) sPtr,
							    (uint32 *) dPtr,
							    rows,
							    cols,
							    planes,
							    sRowStep,
							    sColStep,
							    sPlaneStep,
							    dRowStep,
							    dColStep,
							    dPlaneStep);
				
				break;
				
				}
				
			case ttFloat:
				{
				
				DoCopyArea8_R32 ((const uint8 *) sPtr,
							     (real32 *) dPtr,
							     rows,
							     cols,
							     planes,
							     sRowStep,
							     sColStep,
							     sPlaneStep,
							     dRowStep,
							     dColStep,
							     dPlaneStep,
								 src.PixelRange ());
				
				break;
				
				}
				
			default:
				{
				
				ThrowNotYetImplemented ();
				
				}
				
			}
		
		}
		
	else if (src.fPixelType == ttShort)
		{
		
		switch (fPixelType)
			{
			
			case ttByte:
				{
				
				DoCopyArea8 (((const uint8 *) sPtr) + (qDNGBigEndian ? 1 : 0),
							 (uint8 *) dPtr,
							 rows,
							 cols,
							 planes,
							 sRowStep << 1,
							 sColStep << 1,
							 sPlaneStep << 1,
							 dRowStep,
							 dColStep,
							 dPlaneStep);
						
				break;
				
				}

			case ttSShort:
				{
				
				DoCopyArea16_S16 ((const uint16 *) sPtr,
							      (int16 *) dPtr,
							      rows,
							      cols,
							      planes,
							      sRowStep,
							      sColStep,
							      sPlaneStep,
							      dRowStep,
							      dColStep,
							      dPlaneStep);
				
				break;
				
				}
				
			case ttLong:
				{
				
				DoCopyArea16_32 ((const uint16 *) sPtr,
							     (uint32 *) dPtr,
							     rows,
							     cols,
							     planes,
							     sRowStep,
							     sColStep,
							     sPlaneStep,
							     dRowStep,
							     dColStep,
							     dPlaneStep);
				
				break;
				
				}
				
			case ttFloat:
				{
				
				DoCopyArea16_R32 ((const uint16 *) sPtr,
							      (real32 *) dPtr,
								  rows,
							      cols,
							      planes,
							      sRowStep,
							      sColStep,
							      sPlaneStep,
							      dRowStep,
							      dColStep,
							      dPlaneStep,
								  src.PixelRange ());
				
				break;
				
				}
				
			default:
				{
				
				ThrowNotYetImplemented ();
				
				}
				
			}
			
		}
		
	else if (src.fPixelType == ttSShort)
		{
		
		switch (fPixelType)
			{
			
			case ttByte:
				{
				
				DoCopyArea8 (((const uint8 *) sPtr) + (qDNGBigEndian ? 1 : 0),
							 (uint8 *) dPtr,
							 rows,
							 cols,
							 planes,
							 sRowStep << 1,
							 sColStep << 1,
							 sPlaneStep << 1,
							 dRowStep,
							 dColStep,
							 dPlaneStep);
						
				break;
				
				}

			case ttShort:
				{
				
				// Moving between signed 16 bit values and unsigned 16
				// bit values just requires toggling the sign bit.  So
				// we can use the "backwards" bottleneck.
				
				DoCopyArea16_S16 ((const uint16 *) sPtr,
							      (int16 *) dPtr,
							      rows,
							      cols,
							      planes,
							      sRowStep,
							      sColStep,
							      sPlaneStep,
							      dRowStep,
							      dColStep,
							      dPlaneStep);
				
				break;
				
				}
				
			case ttFloat:
				{
				
				DoCopyAreaS16_R32 ((const int16 *) sPtr,
								   (real32 *) dPtr,
								   rows,
							       cols,
							       planes,
								   sRowStep,
							       sColStep,
								   sPlaneStep,
							       dRowStep,
							       dColStep,
							       dPlaneStep,
								   src.PixelRange ());
				
				break;
				
				}
				
			default:
				{
				
				ThrowNotYetImplemented ();
				
				}
				
			}
			
		}
		
	else if (src.fPixelType == ttLong)
		{
		
		switch (fPixelType)
			{
			
			case ttByte:
				{
				
				DoCopyArea8 (((const uint8 *) sPtr) + (qDNGBigEndian ? 3 : 0),
							 (uint8 *) dPtr,
							 rows,
							 cols,
							 planes,
							 sRowStep << 2,
							 sColStep << 2,
							 sPlaneStep << 2,
							 dRowStep,
							 dColStep,
							 dPlaneStep);
						
				break;
				
				}

			case ttShort:
				{
				
				DoCopyArea16 (((const uint16 *) sPtr) + (qDNGBigEndian ? 1 : 0),
							  (uint16 *) dPtr,
							  rows,
							  cols,
							  planes,
							  sRowStep << 1,
							  sColStep << 1,
							  sPlaneStep << 1,
							  dRowStep,
							  dColStep,
							  dPlaneStep);
				
				break;
				
				}
				
			default:
				{
				
				ThrowNotYetImplemented ();
				
				}
				
			}
			
		}
		
	else if (src.fPixelType == ttFloat)
		{
		
		switch (fPixelType)
			{
			
			case ttByte:
				{
				
				DoCopyAreaR32_8 ((const real32 *) sPtr,
							     (uint8 *) dPtr,
							     rows,
							     cols,
							     planes,
							     sRowStep,
							     sColStep,
							     sPlaneStep,
							     dRowStep,
							     dColStep,
							     dPlaneStep,
								 PixelRange ());
						
				break;
				
				}

			case ttShort:
				{
				
				DoCopyAreaR32_16 ((const real32 *) sPtr,
							      (uint16 *) dPtr,
							      rows,
							      cols,
							      planes,
							      sRowStep,
							      sColStep,
							      sPlaneStep,
							      dRowStep,
							      dColStep,
							      dPlaneStep,
								  PixelRange ());
				
				break;
				
				}
				
			case ttSShort:
				{
				
				DoCopyAreaR32_S16 ((const real32 *) sPtr,
							       (int16 *) dPtr,
							       rows,
							       cols,
							       planes,
							       sRowStep,
							       sColStep,
							       sPlaneStep,
							       dRowStep,
							       dColStep,
							       dPlaneStep,
								   PixelRange ());
				
				break;
				
				}
				
			default:
				{
				
				ThrowNotYetImplemented ();
				
				}
				
			}
		
		}
		
	else
		{
		
		ThrowNotYetImplemented ();
		
		}
		
	}
		
/*****************************************************************************/

dng_point dng_pixel_buffer::RepeatPhase (const dng_rect &srcArea,
					   			   		 const dng_rect &dstArea)
	{
	
	int32 repeatV = srcArea.H ();
	int32 repeatH = srcArea.W ();
	
	int32 phaseV;
	int32 phaseH;
	
	if (srcArea.t >= dstArea.t)
		{
		phaseV = (repeatV - ((srcArea.t - dstArea.t) % repeatV)) % repeatV;
		}
	else
		{
		phaseV = (dstArea.t - srcArea.t) % repeatV;
		}
		
	if (srcArea.l >= dstArea.l)
		{
		phaseH = (repeatH - ((srcArea.l - dstArea.l) % repeatH)) % repeatH;
		}
	else
		{
		phaseH = (dstArea.l - srcArea.l) % repeatH;
		}
		
	return dng_point (phaseV, phaseH);
	
	}
	
/*****************************************************************************/

void dng_pixel_buffer::RepeatArea (const dng_rect &srcArea,
					   			   const dng_rect &dstArea)
	{
	
	dng_point repeat = srcArea.Size ();
	
	dng_point phase = RepeatPhase (srcArea,
								   dstArea);
			
	const void *sPtr = ConstPixel (srcArea.t,
							  	   srcArea.l,
							  	   fPlane);
							  	   
	void *dPtr = DirtyPixel (dstArea.t,
							 dstArea.l,
							 fPlane);
							 
	uint32 rows = dstArea.H ();
	uint32 cols = dstArea.W ();
		
	switch (fPixelSize)
		{
			
		case 1:
			{
			
			DoRepeatArea8 ((const uint8 *) sPtr,
						   (uint8 *) dPtr,
						   rows,
						   cols,
						   fPlanes,
						   fRowStep,
						   fColStep,
						   fPlaneStep,
						   repeat.v,
						   repeat.h,
						   phase.v,
						   phase.h);
			
			break;
			
			}
			
		case 2:
			{
			
			DoRepeatArea16 ((const uint16 *) sPtr,
					  		(uint16 *) dPtr,
					  		rows,
					  		cols,
					  		fPlanes,
					  		fRowStep,
					  		fColStep,
					  		fPlaneStep,
					  		repeat.v,
						    repeat.h,
						    phase.v,
						    phase.h);
			
			break;
			
			}
				
		case 4:
			{
			
			DoRepeatArea32 ((const uint32 *) sPtr,
					  		(uint32 *) dPtr,
					  		rows,
					  		cols,
					  		fPlanes,
					  		fRowStep,
					  		fColStep,
					  		fPlaneStep,
					  		repeat.v,
						    repeat.h,
						    phase.v,
						    phase.h);
			
			break;
			
			}
			
		default:
			{
			
			ThrowNotYetImplemented ();
			
			}
			
		}
				
	}

/*****************************************************************************/

void dng_pixel_buffer::ShiftRight (uint32 shift)
	{
	
	if (fPixelType != ttShort)
		{
		
		ThrowNotYetImplemented ();
		
		}
	
	uint32 rows = fArea.H ();
	uint32 cols = fArea.W ();
	
	uint32 planes = fPlanes;
	
	void *dPtr = DirtyPixel (fArea.t,
							 fArea.l,
							 fPlane);
	
	const void *sPtr = dPtr;
	
	int32 sRowStep   = fRowStep;
	int32 sColStep   = fColStep;
	int32 sPlaneStep = fPlaneStep;
	
	int32 dRowStep   = fRowStep;
	int32 dColStep   = fColStep;
	int32 dPlaneStep = fPlaneStep;
	
	OptimizeOrder (sPtr,
				   dPtr,
				   fPixelSize,
				   fPixelSize,
				   rows,
				   cols,
				   planes,
				   sRowStep,
				   sColStep,
				   sPlaneStep,
				   dRowStep,
				   dColStep,
				   dPlaneStep);
				   
	DoShiftRight16 ((uint16 *) dPtr,
				    rows,
				    cols,
				    planes,
				    dRowStep,
				    dColStep,
				    dPlaneStep,
				    shift);
	
	}
		
/*****************************************************************************/

void dng_pixel_buffer::FlipH ()
	{

	fData = InternalPixel (fArea.t, fArea.r - 1);

	fColStep = -fColStep;

	}

/*****************************************************************************/

void dng_pixel_buffer::FlipV ()
	{
	
	fData = InternalPixel (fArea.b - 1, fArea.l);

	fRowStep = -fRowStep;

	}

/*****************************************************************************/

void dng_pixel_buffer::FlipZ ()
	{

	fData = InternalPixel (fArea.t, fArea.l, fPlanes - 1);

	fPlaneStep = -fPlaneStep;

	}

/*****************************************************************************/

bool dng_pixel_buffer::EqualArea (const dng_pixel_buffer &src,
								  const dng_rect &area,
								  uint32 plane,
								  uint32 planes) const
	{
	
	uint32 rows = area.H ();
	uint32 cols = area.W ();
	
	const void *sPtr = src.ConstPixel (area.t,
								  	   area.l,
								  	   plane);
								  
	const void *dPtr = ConstPixel (area.t,
								   area.l,
								   plane);
	
	int32 sRowStep   = src.fRowStep;
	int32 sColStep   = src.fColStep;
	int32 sPlaneStep = src.fPlaneStep;
	
	int32 dRowStep   = fRowStep;
	int32 dColStep   = fColStep;
	int32 dPlaneStep = fPlaneStep;

	if (fPixelType == src.fPixelType)
		{
		
		if (rows == 1 && cols == 1 && sPlaneStep == 1 && dPlaneStep == 1)
			{
			
			return DoEqualBytes (sPtr,
								 dPtr, 
								 planes * fPixelSize);
			
			}
			
		else switch (fPixelSize)
			{
			
			case 1:
				{
				
				return DoEqualArea8 ((const uint8 *) sPtr,
									 (const uint8 *) dPtr,
									 rows,
									 cols,
									 planes,
									 sRowStep,
									 sColStep,
									 sPlaneStep,
									 dRowStep,
									 dColStep,
									 dPlaneStep);
				
				break;
				
				}
				
			case 2:
				{
				
				return DoEqualArea16 ((const uint16 *) sPtr,
									  (const uint16 *) dPtr,
									  rows,
									  cols,
									  planes,
									  sRowStep,
									  sColStep,
									  sPlaneStep,
									  dRowStep,
									  dColStep,
									  dPlaneStep);

				break;
				
				}
				
			case 4:
				{
				
				return DoEqualArea32 ((const uint32 *) sPtr,
									  (const uint32 *) dPtr,
									  rows,
									  cols,
									  planes,
									  sRowStep,
									  sColStep,
									  sPlaneStep,
									  dRowStep,
									  dColStep,
									  dPlaneStep);
				
				break;
				
				}
				
			default:
				{
				
				ThrowNotYetImplemented ();

				return false;

				}
				
			}
		
		}
		
	else
		return false;

	}

/*****************************************************************************/
