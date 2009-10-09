/*****************************************************************************/
// Copyright 2006-2007 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

/* $Id: //mondo/dng_sdk_1_2/dng_sdk/source/dng_bottlenecks.cpp#1 $ */ 
/* $DateTime: 2008/03/09 14:29:54 $ */
/* $Change: 431850 $ */
/* $Author: tknoll $ */

/*****************************************************************************/

#include "dng_bottlenecks.h"
#include "dng_reference.h"

/*****************************************************************************/

dng_suite gDNGSuite =
	{
	RefZeroBytes,
	RefCopyBytes,
	RefSwapBytes16,
	RefSwapBytes32,
	RefSetArea8,
	RefSetArea16,
	RefSetArea32,
	RefCopyArea8,
	RefCopyArea16,
	RefCopyArea32,
	RefCopyArea8_16,
	RefCopyArea8_S16,
	RefCopyArea8_32,
	RefCopyArea16_S16,
	RefCopyArea16_32,
	RefCopyArea8_R32,
	RefCopyArea16_R32,
	RefCopyAreaS16_R32,
	RefCopyAreaR32_8,
	RefCopyAreaR32_16,
	RefCopyAreaR32_S16,
	RefRepeatArea8,
	RefRepeatArea16,
	RefRepeatArea32,
	RefShiftRight16,
	RefBilinearRow16,
	RefBilinearRow32,
	RefBaselineABCtoRGB,
	RefBaselineABCDtoRGB,
	RefBaselineHueSatMap,
	RefBaselineRGBtoGray,
	RefBaselineRGBtoRGB,
	RefBaseline1DTable,
	RefBaselineRGBTone,
	RefResampleDown16,
	RefResampleDown32,
	RefResampleAcross16,
	RefResampleAcross32,
	RefEqualBytes,
	RefEqualArea8,
	RefEqualArea16,
	RefEqualArea32
	};

/*****************************************************************************/
