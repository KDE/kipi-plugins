/*****************************************************************************/
// Copyright 2006-2008 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

/* $Id: //mondo/dng_sdk_1_2/dng_sdk/source/dng_negative.cpp#2 $ */ 
/* $DateTime: 2008/04/02 14:06:57 $ */
/* $Change: 440485 $ */
/* $Author: tknoll $ */

/*****************************************************************************/

#include "dng_negative.h"
#include "dng_abort_sniffer.h"
#include "dng_bottlenecks.h"
#include "dng_camera_profile.h"
#include "dng_color_spec.h"
#include "dng_exceptions.h"
#include "dng_host.h"
#include "dng_image.h"
#include "dng_image_writer.h"
#include "dng_info.h"
#include "dng_linearization_info.h"
#include "dng_memory_stream.h"
#include "dng_mosaic_info.h"
#include "dng_simple_image.h"
#include "dng_tag_codes.h"
#include "dng_tag_values.h"
#include "dng_tile_iterator.h"
#include "dng_xmp.h"

/*****************************************************************************/

dng_negative::dng_negative (dng_memory_allocator &allocator)

	:	fAllocator (allocator)
	
	,	fModelName		    		()
	,	fLocalName		    		()
	,	fHasBaseOrientation 		(false)
	,	fBaseOrientation    		()
	,	fDefaultCropSizeH   		()
	,	fDefaultCropSizeV			()
	,	fDefaultCropOriginH 		(0, 1)
	,	fDefaultCropOriginV 		(0, 1)
	,	fDefaultScaleH      		(1, 1)
	,	fDefaultScaleV				(1, 1)
	,	fBestQualityScale			(1, 1)
	,	fRawToFullScaleH			(1.0)
	,	fRawToFullScaleV			(1.0)
	,	fBaselineNoise	    		(100, 100)
	,	fNoiseReductionApplied		(0, 0)
	,	fBaselineExposure			(  0, 100)
	,	fBaselineSharpness			(100, 100)
	,	fChromaBlurRadius			()
	,	fAntiAliasStrength			(100, 100)
	,	fLinearResponseLimit		(100, 100)
	,	fShadowScale				(1, 1)
	,	fColorimetricReference  	(crSceneReferred)
	,	fColorChannels				(0)
	,	fAnalogBalance				()
	,	fCameraNeutral				()
	,	fCameraWhiteXY				()
	,	fCameraCalibration1			()
	,	fCameraCalibration2			()
	,	fCameraCalibrationSignature ()
	,	fCameraProfile				()
	,	fAsShotProfileName			()
	,	fRawImageDigest				()
	,	fRawDataUniqueID    		()
	,	fOriginalRawFileName		()
	,	fHasOriginalRawFileData 	(false)
	,	fOriginalRawFileData    	()
	,	fOriginalRawFileDigest		()
	,	fDNGPrivateData				()
	,	fIsMakerNoteSafe			(false)
	,	fMakerNote					()
	,	fExif			    		()
	,	fOriginalExif				()
	,	fIPTCBlock          		()
	,	fIPTCOffset					(kDNGStreamInvalidOffset)
	,	fXMP			    		()
	,	fValidEmbeddedXMP       	(false)
	,	fXMPinSidecar	    		(false)
	,	fXMPisNewer		    		(false)
	,	fLinearizationInfo  		()
	,	fMosaicInfo         		()
	,	fStage1Image        		()
	,	fStage2Image        		()
	,	fStage3Image        		()
	,	fStage3Gain					(1.0)
	,	fIsPreview					(false)
	,	fIsDamaged					(false)

	{

	}

/*****************************************************************************/

dng_negative::~dng_negative ()
	{
	
	// Delete any camera profiles owned by this negative.
	
	ClearProfiles ();
		
	}

/******************************************************************************/

void dng_negative::Initialize ()
	{
	
	fExif.Reset (MakeExif ());
	
	fXMP.Reset (MakeXMP ());
	
	}

/******************************************************************************/

dng_negative * dng_negative::Make (dng_memory_allocator &allocator)
	{
	
	AutoPtr<dng_negative> result (new dng_negative (allocator));
	
	if (!result.Get ())
		{
		ThrowMemoryFull ();
		}
	
	result->Initialize ();
	
	return result.Release ();
	
	}

/******************************************************************************/

void dng_negative::SetBaseOrientation (const dng_orientation &orientation)
	{
	
	fHasBaseOrientation = true;
	
	fBaseOrientation = orientation;
	
	}
		
/******************************************************************************/

dng_orientation dng_negative::Orientation () const
	{
	
	return BaseOrientation ();
	
	}
		
/******************************************************************************/

void dng_negative::ApplyOrientation (const dng_orientation &orientation)
	{
	
	fBaseOrientation += orientation;
	
	fXMP->SetOrientation (fBaseOrientation);

	}
				  
/******************************************************************************/

void dng_negative::SetAnalogBalance (const dng_vector &b)
	{
	
	real64 minEntry = b.MinEntry ();
	
	if (b.NotEmpty () && minEntry > 0.0)
		{
		
		fAnalogBalance = b;
	
		fAnalogBalance.Scale (1.0 / minEntry);
		
		fAnalogBalance.Round (1000000.0);
		
		}
		
	else
		{
		
		fAnalogBalance.Clear ();
		
		}
		
	}
					  
/*****************************************************************************/

real64 dng_negative::AnalogBalance (uint32 channel) const
	{
	
	DNG_ASSERT (channel < ColorChannels (), "Channel out of bounds");
	
	if (channel < fAnalogBalance.Count ())
		{
		
		return fAnalogBalance [channel];
		
		}
		
	return 1.0;
	
	}
		
/*****************************************************************************/

dng_urational dng_negative::AnalogBalanceR (uint32 channel) const
	{
	
	dng_urational result;
	
	result.Set_real64 (AnalogBalance (channel), 1000000);
	
	return result;
	
	}

/******************************************************************************/

void dng_negative::SetCameraNeutral (const dng_vector &n)
	{
	
	real64 maxEntry = n.MaxEntry ();
		
	if (n.NotEmpty () && maxEntry > 0.0)
		{
		
		fCameraNeutral = n;
	
		fCameraNeutral.Scale (1.0 / maxEntry);
		
		fCameraNeutral.Round (1000000.0);
		
		}
		
	else
		{
		
		fCameraNeutral.Clear ();
		
		}

	}
	  
/*****************************************************************************/

dng_urational dng_negative::CameraNeutralR (uint32 channel) const
	{
	
	dng_urational result;
	
	result.Set_real64 (CameraNeutral () [channel], 1000000);
	
	return result;
	
	}

/******************************************************************************/

void dng_negative::SetCameraWhiteXY (const dng_xy_coord &coord)
	{
	
	if (coord.IsValid ())
		{
		
		fCameraWhiteXY.x = Round_int32 (coord.x * 1000000.0) / 1000000.0;
		fCameraWhiteXY.y = Round_int32 (coord.y * 1000000.0) / 1000000.0;
		
		}
		
	else
		{
		
		fCameraWhiteXY.Clear ();
		
		}
	
	}
		
/*****************************************************************************/

const dng_xy_coord & dng_negative::CameraWhiteXY () const
	{
	
	DNG_ASSERT (HasCameraWhiteXY (), "Using undefined CameraWhiteXY");

	return fCameraWhiteXY;
	
	}
							   
/*****************************************************************************/

void dng_negative::GetCameraWhiteXY (dng_urational &x,
							   		 dng_urational &y) const
	{
	
	dng_xy_coord coord = CameraWhiteXY ();
	
	x.Set_real64 (coord.x, 1000000);
	y.Set_real64 (coord.y, 1000000);
	
	}
		
/*****************************************************************************/

void dng_negative::SetCameraCalibration1 (const dng_matrix &m)
	{
	
	fCameraCalibration1 = m;
	
	fCameraCalibration1.Round (10000);
	
	}

/******************************************************************************/

void dng_negative::SetCameraCalibration2 (const dng_matrix &m)
	{
	
	fCameraCalibration2 = m;
	
	fCameraCalibration2.Round (10000);
		
	}

/******************************************************************************/

void dng_negative::AddProfile (AutoPtr<dng_camera_profile> &profile)
	{
	
	// Make sure we have a profile to add.
	
	if (!profile.Get ())
		{
		
		return;
		
		}
	
	// We must have some profile name.  Use "embedded" if nothing else.
	
	if (profile->Name ().IsEmpty ())
		{
		
		profile->SetName (kProfileName_Embedded);
		
		}
		
	// Special case support for reading older DNG files which did not store
	// the profile name in the main IFD profile.
	
	if (fCameraProfile.size ())
		{
		
		// See the first profile has a default "embedded" name, and has
		// the same data as the profile we are adding.
		
		if (fCameraProfile [0]->NameIsEmbedded () &&
			fCameraProfile [0]->EqualData (*profile.Get ()))
			{
			
			// If the profile we are deleting was read from DNG
			// then the new profile should be marked as such also.
			
			if (fCameraProfile [0]->WasReadFromDNG ())
				{
				
				profile->SetWasReadFromDNG ();
				
				}
				
			// Delete the profile with default name.
			
			delete fCameraProfile [0];
			
			fCameraProfile [0] = NULL;
			
			fCameraProfile.erase (fCameraProfile.begin ());
			
			}
		
		}
		
	// Duplicate detection logic.  We give a preference to last added profile
	// so the profiles end up in a more consistent order no matter what profiles
	// happen to be embedded in the DNG.
	
	for (uint32 index = 0; index < (uint32) fCameraProfile.size (); index++)
		{
		
		if (fCameraProfile [index]->Fingerprint () == profile->Fingerprint ())
			{
			
			// If the profile we are deleting was read from DNG
			// then the new profile should be marked as such also.
			
			if (fCameraProfile [index]->WasReadFromDNG ())
				{
				
				profile->SetWasReadFromDNG ();
				
				}
				
			// Delete the profile with default name.
			
			delete fCameraProfile [index];
			
			fCameraProfile [index] = NULL;
			
			fCameraProfile.erase (fCameraProfile.begin () + index);
			
			break;
			
			}
			
		}
		
	// Now add to profile list.
	
	fCameraProfile.push_back (NULL);
	
	fCameraProfile [fCameraProfile.size () - 1] = profile.Release ();
	
	}
			
/******************************************************************************/

void dng_negative::ClearProfiles ()
	{
	
	// Delete any camera profiles owned by this negative.
	
	for (uint32 index = 0; index < (uint32) fCameraProfile.size (); index++)
		{
		
		if (fCameraProfile [index])
			{
			
			delete fCameraProfile [index];
			
			fCameraProfile [index] = NULL;
			
			}
		
		}
		
	// Now empty list.
	
	fCameraProfile.clear ();
	
	}

/******************************************************************************/

uint32 dng_negative::ProfileCount () const
	{
	
	return (uint32) fCameraProfile.size ();
	
	}
		
/******************************************************************************/

const dng_camera_profile & dng_negative::ProfileByIndex (uint32 index) const
	{
	
	DNG_ASSERT (index < ProfileCount (),
				"Invalid index for ProfileByIndex");
				
	return *fCameraProfile [index];
		
	}
		
/*****************************************************************************/

const dng_camera_profile * dng_negative::ProfileByID (const dng_camera_profile_id &id,
													  bool useDefaultIfNoMatch) const
	{
	
	// If this negative does not have any profiles, we are not going to
	// find a match.
	
	uint32 profileCount = ProfileCount ();
	
	if (profileCount == 0)
		{
		return NULL;
		}
		
	// If we have both a profile name and fingerprint, try matching both.
	
	if (id.Name ().NotEmpty () && id.Fingerprint ().IsValid ())
		{
		
		for (uint32 index = 0; index < profileCount; index++)
			{
			
			const dng_camera_profile &profile = ProfileByIndex (index);
			
			if (id.Name        () == profile.Name        () &&
				id.Fingerprint () == profile.Fingerprint ())
				{
				
				return &profile;
				
				}
			
			}

		}
		
	// If we have a name, try matching that.
	
	if (id.Name ().NotEmpty ())
		{
		
		for (uint32 index = 0; index < profileCount; index++)
			{
			
			const dng_camera_profile &profile = ProfileByIndex (index);
			
			if (id.Name () == profile.Name ())
				{
				
				return &profile;
				
				}
			
			}

		}
		
	// If we have a valid fingerprint, try matching that.
		
	if (id.Fingerprint ().IsValid ())
		{
		
		for (uint32 index = 0; index < profileCount; index++)
			{
			
			const dng_camera_profile &profile = ProfileByIndex (index);
			
			if (id.Fingerprint () == profile.Fingerprint ())
				{
				
				return &profile;
				
				}
			
			}

		}
		
	// Did not find a match either way.  See if we should return a default
	// value.
	
	if (useDefaultIfNoMatch)
		{
		
		return &ProfileByIndex (0);
		
		}
		
	// Found nothing.
	
	return NULL;
		
	}

/*****************************************************************************/

const dng_camera_profile * dng_negative::CameraProfileToEmbed () const
	{
	
	uint32 index;
	
	uint32 count = ProfileCount ();
	
	if (count == 0)
		{
		
		return NULL;
		
		}
		
	// First try to look for the first profile that was already in the DNG
	// when we read it.
	
	for (index = 0; index < count; index++)
		{
		
		const dng_camera_profile &profile (ProfileByIndex (index));
		
		if (profile.WasReadFromDNG ())
			{
			
			return &profile;
			
			}
		
		}
		
	// Next we look for the first profile that is legal to embed.
	
	for (index = 0; index < count; index++)
		{
		
		const dng_camera_profile &profile (ProfileByIndex (index));
		
		if (profile.IsLegalToEmbed ())
			{
			
			return &profile;
			
			}
		
		}
		
	// Else just return the first profile.
	
	return fCameraProfile [0];
	
	}
							   
/*****************************************************************************/

dng_color_spec * dng_negative::MakeColorSpec (const dng_camera_profile_id &id) const
	{

	dng_color_spec *spec = new dng_color_spec (*this, ProfileByID (id));
											   
	if (!spec)
		{
		ThrowMemoryFull ();
		}
		
	return spec;
	
	}
							   
/*****************************************************************************/

void dng_negative::FindRawImageDigest (dng_host &host) const
	{
	
	if (fRawImageDigest.IsNull ())
		{
		
		const dng_image &image = RawImage ();
	
		dng_md5_printer printer;
		
		dng_pixel_buffer buffer;
		
		buffer.fPlane  = 0;
		buffer.fPlanes = image.Planes ();
		
		buffer.fRowStep   = image.Planes () * image.Width ();
		buffer.fColStep   = image.Planes ();
		buffer.fPlaneStep = 1;
		
		buffer.fPixelType = image.PixelType ();
		buffer.fPixelSize = image.PixelSize ();
		
		// Sometimes we expand 8-bit data to 16-bit data while reading or
		// writing, so always compute the digest of 8-bit data as 16-bits.
		
		if (buffer.fPixelType == ttByte)
			{
			buffer.fPixelType = ttShort;
			buffer.fPixelSize = 2;
			}
		
		const uint32 kBufferRows = 16;
		
		uint32 bufferBytes = kBufferRows * buffer.fRowStep * buffer.fPixelSize;
		
		AutoPtr<dng_memory_block> bufferData (host.Allocate (bufferBytes));
		
		buffer.fData = bufferData->Buffer ();
		
		dng_rect area;
		
		dng_tile_iterator iter (dng_point (kBufferRows,
										   image.Width ()),
								image.Bounds ());
								
		while (iter.GetOneTile (area))
			{
			
			host.SniffForAbort ();
			
			buffer.fArea = area;
			
			image.Get (buffer);
			
			uint32 count = buffer.fArea.H () *
						   buffer.fRowStep *
						   buffer.fPixelSize;
						   
			#if qDNGBigEndian
			
			// We need to use the same byte order to compute
			// the digest, no matter the native order.  Little-endian
			// is more common now, so use that.
			
			switch (buffer.fPixelSize)
				{
				
				case 1:
					break;
				
				case 2:
					{
					DoSwapBytes16 ((uint16 *) buffer.fData, count >> 1);
					break;
					}
				
				case 4:
					{
					DoSwapBytes32 ((uint32 *) buffer.fData, count >> 2);
					break;
					}
					
				default:
					{
					DNG_REPORT ("Unexpected pixel size");
					break;
					}
				
				}
			
			#endif

			printer.Process (buffer.fData,
							 count);
			
			}
			
		fRawImageDigest = printer.Result ();
	
		}
	
	}
							   
/*****************************************************************************/

void dng_negative::ValidateRawImageDigest (dng_host &host)
	{
	
	if (Stage1Image () && !IsPreview () && fRawImageDigest.IsValid ())
		{
		
		dng_fingerprint oldDigest = fRawImageDigest;
		
		try
			{
			
			fRawImageDigest.Clear ();
			
			FindRawImageDigest (host);
			
			}
			
		catch (...)
			{
			
			fRawImageDigest = oldDigest;
			
			throw;
			
			}
		
		if (oldDigest != fRawImageDigest)
			{
			
			#if qDNGValidate
			
			ReportError ("RawImageDigest does not match raw image");
			
			#else
			
			#if 0		// FIX_ME_TFK: Disable for now so the warning code get more testing.
			
			// Note that Lightroom 1.4 Windows had a bug that corrupts the
			// first four bytes of the RawImageDigest tag.  So if the last
			// twelve bytes match, this is very likely the result of the
			// bug, and not an actual corrupt file.  So don't report this
			// to the user--just fix it.
			
				{
			
				bool matchLast12 = true;
				
				for (uint32 j = 4; j < 16; j++)
					{
					matchLast12 = matchLast12 && (oldDigest.data [j] == fRawImageDigest.data [j]);
					}
					
				if (matchLast12)
					{
					return;
					}
					
				}
				
			#endif
			
			SetIsDamaged (true);
			
			#endif
			
			}
			
		}
	
	}
							   
/*****************************************************************************/

// If the raw data unique ID is missing, compute one based on a MD5 hash of
// the raw image data and the model name.

void dng_negative::FindRawDataUniqueID (dng_host &host) const
	{
	
	if (fRawDataUniqueID.IsNull ())
		{
		
		FindRawImageDigest (host);
		
		dng_md5_printer printer;
		
		printer.Process (fRawImageDigest.data, 16);
					
		printer.Process (ModelName ().Get    (),
						 ModelName ().Length ());
		
		fRawDataUniqueID = printer.Result ();
	
		}
	
	}
		
/******************************************************************************/

void dng_negative::FindOriginalRawFileDigest () const
	{

	if (fOriginalRawFileDigest.IsNull () && fOriginalRawFileData.Get ())
		{
		
		dng_md5_printer printer;
		
		printer.Process (fOriginalRawFileData->Buffer      (),
						 fOriginalRawFileData->LogicalSize ());
					
		fOriginalRawFileDigest = printer.Result ();
	
		}

	}
		
/*****************************************************************************/

void dng_negative::ValidateOriginalRawFileDigest ()
	{
	
	if (fOriginalRawFileDigest.IsValid () && fOriginalRawFileData.Get ())
		{
		
		dng_fingerprint oldDigest = fOriginalRawFileDigest;
		
		try
			{
			
			fOriginalRawFileDigest.Clear ();
			
			FindOriginalRawFileDigest ();
			
			}
			
		catch (...)
			{
			
			fOriginalRawFileDigest = oldDigest;
			
			throw;
			
			}
		
		if (oldDigest != fOriginalRawFileDigest)
			{
			
			#if qDNGValidate
			
			ReportError ("OriginalRawFileDigest does not match OriginalRawFileData");
			
			#else
			
			SetIsDamaged (true);
			
			#endif
			
			// Don't "repair" the original image data digest.  Once it is
			// bad, it stays bad.  The user cannot tell by looking at the image
			// whether the damage is acceptable and can be ignored in the
			// future.
			
			fOriginalRawFileDigest = oldDigest;
			
			}
			
		}
		
	}
							   
/******************************************************************************/

dng_rect dng_negative::DefaultCropArea (real64 scaleH,
				    	  			    real64 scaleV) const
	{
	
	const dng_image *image = Stage3Image ();
	
	DNG_ASSERT (image, "DefaultCropArea on NULL image");
	
	dng_point scaledSize;
	
	scaledSize.h = Round_int32 (image->Width  () * scaleH);
	scaledSize.v = Round_int32 (image->Height () * scaleV);
			
	// First compute the area using simple rounding.
		
	dng_rect result;
	
	result.l = Round_int32 (fDefaultCropOriginH.As_real64 () * fRawToFullScaleH * scaleH);
	result.t = Round_int32 (fDefaultCropOriginV.As_real64 () * fRawToFullScaleV * scaleV);
	
	result.r = result.l + Round_int32 (fDefaultCropSizeH.As_real64 () * fRawToFullScaleH * scaleH);
	result.b = result.t + Round_int32 (fDefaultCropSizeV.As_real64 () * fRawToFullScaleV * scaleV);
	
	// Sometimes the simple rounding causes the resulting default crop
	// area to slide off the scaled image area.  So we force this not
	// to happen.
	
	if (result.r > scaledSize.h)
		{
		result.l -= result.r - scaledSize.h;
		result.r  = scaledSize.h;
		}
		
	if (result.b > scaledSize.v)
		{
		result.t -= result.b - scaledSize.v;
		result.b  = scaledSize.v;
		}
		
	return result;
	
	}

/******************************************************************************/

void dng_negative::SetShadowScale (const dng_urational &scale)
	{
	
	if (scale.d > 0)
		{
		
		real64 s = scale.As_real64 ();
		
		if (s > 0.0 && s <= 1.0)
			{
	
			fShadowScale = scale;
			
			}
		
		}
	
	}
			
/******************************************************************************/

dng_memory_block * dng_negative::BuildExifBlock (const dng_resolution *resolution,
												 bool includeIPTC) const
	{
	
	dng_memory_stream stream (Allocator ());
	
		{
	
		// Create the main IFD
											 
		dng_tiff_directory mainIFD;
		
		// Optionally include the resolution tags.
		
		dng_resolution res;
		
		if (resolution)
			{
			res = *resolution;
			}
	
		tag_urational tagXResolution (tcXResolution, res.fXResolution);
		tag_urational tagYResolution (tcYResolution, res.fYResolution);
		
		tag_uint16 tagResolutionUnit (tcResolutionUnit, res.fResolutionUnit);
		
		if (resolution)
			{
			mainIFD.Add (&tagXResolution   );
			mainIFD.Add (&tagYResolution   );
			mainIFD.Add (&tagResolutionUnit);
			}

		// Optionally include IPTC block.
		
		tag_iptc tagIPTC (IPTCData   (),
						  IPTCLength ());
			
		if (includeIPTC && tagIPTC.Count ())
			{
			
			mainIFD.Add (&tagIPTC);
			
			}
							
		// Exif tags.
		
		exif_tag_set exifSet (mainIFD,
							  *GetExif (),
							  IsMakerNoteSafe (),
							  MakerNoteData   (),
							  MakerNoteLength (),
							  false);
							  
		// Figure out the Exif IFD offset.
		
		exifSet.Locate (8 + mainIFD.Size ());
			
		// Write TIFF Header.
		
		stream.SetWritePosition (0);
		
		stream.Put_uint16 (stream.BigEndian () ? byteOrderMM : byteOrderII);
		
		stream.Put_uint16 (42);
		
		stream.Put_uint32 (8);
		
		// Write the IFDs.
		
		mainIFD.Put (stream);
		
		exifSet.Put (stream);
		
		// Trim the file to this length.
		
		stream.Flush ();
		
		stream.SetLength (stream.Position ());
		
		}
		
	return stream.AsMemoryBlock (Allocator ());
		
	}
			
/******************************************************************************/

void dng_negative::SetIPTC (AutoPtr<dng_memory_block> &block, uint64 offset)
	{
	
	fIPTCBlock.Reset (block.Release ());
	
	fIPTCOffset = offset;
	
	}
					  
/******************************************************************************/

void dng_negative::SetIPTC (AutoPtr<dng_memory_block> &block)
	{
	
	SetIPTC (block, kDNGStreamInvalidOffset);
	
	}
					  
/******************************************************************************/

void dng_negative::ClearIPTC ()
	{
	
	fIPTCBlock.Reset ();
	
	fIPTCOffset = kDNGStreamInvalidOffset;
	
	}
					  
/*****************************************************************************/

const void * dng_negative::IPTCData () const
	{
	
	if (fIPTCBlock.Get ())
		{
		
		return fIPTCBlock->Buffer ();
		
		}
		
	return NULL;
	
	}

/*****************************************************************************/

uint32 dng_negative::IPTCLength () const
	{
	
	if (fIPTCBlock.Get ())
		{
		
		return fIPTCBlock->LogicalSize ();
		
		}
		
	return 0;
	
	}
		
/*****************************************************************************/

uint64 dng_negative::IPTCOffset () const
	{
	
	if (fIPTCBlock.Get ())
		{
		
		return fIPTCOffset;
		
		}
		
	return kDNGStreamInvalidOffset;
	
	}
		
/*****************************************************************************/

dng_fingerprint dng_negative::IPTCDigest () const
	{
	
	if (IPTCLength ())
		{
		
		dng_md5_printer printer;
		
		const uint8 *data = (const uint8 *) IPTCData ();
		
		uint32 count = IPTCLength ();
		
		// Because of some stupid ways of storing the IPTC data, the IPTC
		// data might be padded with up to three zeros.  Don't include
		// these zero pad bytes in the digest.
		
		uint32 removed = 0;
		
		while ((removed < 3) && (count > 0) && (data [count - 1] == 0))
			{
			removed++;
			count--;
			}
		
		printer.Process (data, count);
						 
		return printer.Result ();
			
		}
	
	return dng_fingerprint ();
	
	}
		
/******************************************************************************/

void dng_negative::RebuildIPTC ()
	{
	
	ClearIPTC ();
	
	fXMP->RebuildIPTC (*this);
	
	dng_fingerprint digest = IPTCDigest ();
	
	fXMP->SetIPTCDigest (digest);
	
	}
			  
/*****************************************************************************/

bool dng_negative::SetXMP (dng_host &host,
						   const void *buffer,
					 	   uint32 count,
					 	   bool xmpInSidecar,
					 	   bool xmpIsNewer)
	{
	
	bool result = false;
	
	try
		{
		
		AutoPtr<dng_xmp> tempXMP (MakeXMP ());
		
		tempXMP->Parse (host, buffer, count);
		
		fXMP.Reset (tempXMP.Release ());
		
		fXMPinSidecar = xmpInSidecar;
		
		fXMPisNewer = xmpIsNewer;
		
		result = true;
		
		}
		
	catch (dng_exception &except)
		{
		
		// Don't ignore transient errors.
		
		if (host.IsTransientError (except.ErrorCode ()))
			{
			
			throw;
			
			}
			
		// Eat other parsing errors.
		
		}
		
	catch (...)
		{
		
		// Eat unknown parsing exceptions.
		
		}
	
	return result;
	
	}
		
/******************************************************************************/

void dng_negative::SetActiveArea (const dng_rect &area)
	{
	
	NeedLinearizationInfo ();
	
	dng_linearization_info &info = *fLinearizationInfo.Get ();
								    
	info.fActiveArea = area;
	
	}

/******************************************************************************/

void dng_negative::SetMaskedAreas (uint32 count,
							 	   const dng_rect *area)
	{
	
	DNG_ASSERT (count <= kMaxMaskedAreas, "Too many masked areas");
	
	NeedLinearizationInfo ();
	
	dng_linearization_info &info = *fLinearizationInfo.Get ();
								    
	info.fMaskedAreaCount = Min_uint32 (count, kMaxMaskedAreas);
	
	for (uint32 index = 0; index < info.fMaskedAreaCount; index++)
		{
		
		info.fMaskedArea [index] = area [index];
		
		}
		
	}
		
/*****************************************************************************/

void dng_negative::SetLinearization (AutoPtr<dng_memory_block> &curve)
	{
	
	NeedLinearizationInfo ();
	
	dng_linearization_info &info = *fLinearizationInfo.Get ();
								    
	info.fLinearizationTable.Reset (curve.Release ());
	
	}
		
/*****************************************************************************/

void dng_negative::SetBlackLevel (real64 black,
								  int32 plane)
	{

	NeedLinearizationInfo ();
	
	dng_linearization_info &info = *fLinearizationInfo.Get ();
								    
	info.fBlackLevelRepeatRows = 1;
	info.fBlackLevelRepeatCols = 1;
	
	if (plane < 0)
		{
		
		for (uint32 j = 0; j < kMaxSamplesPerPixel; j++)
			{
			
			info.fBlackLevel [0] [0] [j] = black;
			
			}
		
		}
		
	else
		{
		
		info.fBlackLevel [0] [0] [plane] = black;
			
		}
	
	info.RoundBlacks ();
		
	}
		
/*****************************************************************************/

void dng_negative::SetQuadBlacks (real64 black0,
						    	  real64 black1,
						    	  real64 black2,
						    	  real64 black3)
	{
	
	NeedLinearizationInfo ();
	
	dng_linearization_info &info = *fLinearizationInfo.Get ();
								    
	info.fBlackLevelRepeatRows = 2;
	info.fBlackLevelRepeatCols = 2;
	
	for (uint32 j = 0; j < kMaxSamplesPerPixel; j++)
		{
		
		info.fBlackLevel [0] [0] [j] = black0;
		info.fBlackLevel [0] [1] [j] = black1;
		info.fBlackLevel [1] [0] [j] = black2;
		info.fBlackLevel [1] [1] [j] = black3;
		
		}
		
	info.RoundBlacks ();
		
	}

/*****************************************************************************/

void dng_negative::SetRowBlacks (const real64 *blacks,
				   		   		 uint32 count)
	{
	
	if (count)
		{
	
		NeedLinearizationInfo ();
		
		dng_linearization_info &info = *fLinearizationInfo.Get ();
		
		uint32 byteCount = count * sizeof (real64);
									    
		info.fBlackDeltaV.Reset (Allocator ().Allocate (byteCount));
		
		DoCopyBytes (blacks,
					 info.fBlackDeltaV->Buffer (),
					 byteCount);
		
		info.RoundBlacks ();
		
		}
		
	else if (fLinearizationInfo.Get ())
		{
		
		dng_linearization_info &info = *fLinearizationInfo.Get ();
		
		info.fBlackDeltaV.Reset ();
	
		}
								    
	}
							
/*****************************************************************************/

void dng_negative::SetColumnBlacks (const real64 *blacks,
					  		  	    uint32 count)
	{
	
	if (count)
		{
	
		NeedLinearizationInfo ();
		
		dng_linearization_info &info = *fLinearizationInfo.Get ();
		
		uint32 byteCount = count * sizeof (real64);
									    
		info.fBlackDeltaH.Reset (Allocator ().Allocate (byteCount));
		
		DoCopyBytes (blacks,
					 info.fBlackDeltaH->Buffer (),
					 byteCount);
		
		info.RoundBlacks ();
		
		}
		
	else if (fLinearizationInfo.Get ())
		{
		
		dng_linearization_info &info = *fLinearizationInfo.Get ();
									    
		info.fBlackDeltaH.Reset ();
	
		}
								    
	}
							
/*****************************************************************************/

uint32 dng_negative::WhiteLevel (uint32 plane) const
	{
	
	if (fLinearizationInfo.Get ())
		{
		
		const dng_linearization_info &info = *fLinearizationInfo.Get ();
									    
		return Round_uint32 (info.fWhiteLevel [plane]);
									    
		}
		
	return 0x0FFFF;
	
	}
							
/*****************************************************************************/

void dng_negative::SetWhiteLevel (uint32 white,
							      int32 plane)
	{

	NeedLinearizationInfo ();
	
	dng_linearization_info &info = *fLinearizationInfo.Get ();
								    
	if (plane < 0)
		{
		
		for (uint32 j = 0; j < kMaxSamplesPerPixel; j++)
			{
			
			info.fWhiteLevel [j] = (real64) white;
			
			}
		
		}
		
	else
		{
		
		info.fWhiteLevel [plane] = (real64) white;
			
		}
	
	}

/******************************************************************************/

void dng_negative::SetColorKeys (ColorKeyCode color0,
						   		 ColorKeyCode color1,
						   		 ColorKeyCode color2,
						   		 ColorKeyCode color3)
	{
	
	NeedMosaicInfo ();
	
	dng_mosaic_info &info = *fMosaicInfo.Get ();
							 
	info.fCFAPlaneColor [0] = color0;
	info.fCFAPlaneColor [1] = color1;
	info.fCFAPlaneColor [2] = color2;
	info.fCFAPlaneColor [3] = color3;
	
	}

/******************************************************************************/

void dng_negative::SetBayerMosaic (uint32 phase)
	{
	
	NeedMosaicInfo ();
	
	dng_mosaic_info &info = *fMosaicInfo.Get ();
	
	ColorKeyCode color0 = (ColorKeyCode) info.fCFAPlaneColor [0];
	ColorKeyCode color1 = (ColorKeyCode) info.fCFAPlaneColor [1];
	ColorKeyCode color2 = (ColorKeyCode) info.fCFAPlaneColor [2];
	
	info.fCFAPatternSize = dng_point (2, 2);
	
	switch (phase)
		{
		
		case 0:
			{
			info.fCFAPattern [0] [0] = color1;
			info.fCFAPattern [0] [1] = color0;
			info.fCFAPattern [1] [0] = color2;
			info.fCFAPattern [1] [1] = color1;
			break;
			}
			
		case 1:
			{
			info.fCFAPattern [0] [0] = color0;
			info.fCFAPattern [0] [1] = color1;
			info.fCFAPattern [1] [0] = color1;
			info.fCFAPattern [1] [1] = color2;
			break;
			}
			
		case 2:
			{
			info.fCFAPattern [0] [0] = color2;
			info.fCFAPattern [0] [1] = color1;
			info.fCFAPattern [1] [0] = color1;
			info.fCFAPattern [1] [1] = color0;
			break;
			}
			
		case 3:
			{
			info.fCFAPattern [0] [0] = color1;
			info.fCFAPattern [0] [1] = color2;
			info.fCFAPattern [1] [0] = color0;
			info.fCFAPattern [1] [1] = color1;
			break;
			}
			
		}
		
	info.fColorPlanes = 3;
	
	info.fCFALayout = 1;
	
	}

/******************************************************************************/

void dng_negative::SetFujiMosaic (uint32 phase)
	{
	
	NeedMosaicInfo ();
	
	dng_mosaic_info &info = *fMosaicInfo.Get ();
	
	ColorKeyCode color0 = (ColorKeyCode) info.fCFAPlaneColor [0];
	ColorKeyCode color1 = (ColorKeyCode) info.fCFAPlaneColor [1];
	ColorKeyCode color2 = (ColorKeyCode) info.fCFAPlaneColor [2];
	
	info.fCFAPatternSize = dng_point (2, 4);
	
	switch (phase)
		{
		
		case 0:
			{
			info.fCFAPattern [0] [0] = color0;
			info.fCFAPattern [0] [1] = color1;
			info.fCFAPattern [0] [2] = color2;
			info.fCFAPattern [0] [3] = color1;
			info.fCFAPattern [1] [0] = color2;
			info.fCFAPattern [1] [1] = color1;
			info.fCFAPattern [1] [2] = color0;
			info.fCFAPattern [1] [3] = color1;
			break;
			}
			
		case 1:
			{
			info.fCFAPattern [0] [0] = color2;
			info.fCFAPattern [0] [1] = color1;
			info.fCFAPattern [0] [2] = color0;
			info.fCFAPattern [0] [3] = color1;
			info.fCFAPattern [1] [0] = color0;
			info.fCFAPattern [1] [1] = color1;
			info.fCFAPattern [1] [2] = color2;
			info.fCFAPattern [1] [3] = color1;
			break;
			}
			
		}
		
	info.fColorPlanes = 3;
	
	info.fCFALayout = 2;
			
	}

/******************************************************************************/

void dng_negative::SetQuadMosaic (uint32 pattern)
	{
	
	// The pattern of the four colors is assumed to be repeat at least every two
	// columns and eight rows.  The pattern is encoded as a 32-bit integer,
	// with every two bits encoding a color, in scan order for two columns and
	// eight rows (lsb is first).  The usual color coding is:
	//
	// 0 = Green
	// 1 = Magenta
	// 2 = Cyan
	// 3 = Yellow
	//
	// Examples:
	//
	//	PowerShot 600 uses 0xe1e4e1e4:
	//
	//	  0 1 2 3 4 5
	//	0 G M G M G M
	//	1 C Y C Y C Y
	//	2 M G M G M G
	//	3 C Y C Y C Y
	//
	//	PowerShot A5 uses 0x1e4e1e4e:
	//
	//	  0 1 2 3 4 5
	//	0 C Y C Y C Y
	//	1 G M G M G M
	//	2 C Y C Y C Y
	//	3 M G M G M G
	//
	//	PowerShot A50 uses 0x1b4e4b1e:
	//
	//	  0 1 2 3 4 5
	//	0 C Y C Y C Y
	//	1 M G M G M G
	//	2 Y C Y C Y C
	//	3 G M G M G M
	//	4 C Y C Y C Y
	//	5 G M G M G M
	//	6 Y C Y C Y C
	//	7 M G M G M G
	//
	//	PowerShot Pro70 uses 0x1e4b4e1b:
	//
	//	  0 1 2 3 4 5
	//	0 Y C Y C Y C
	//	1 M G M G M G
	//	2 C Y C Y C Y
	//	3 G M G M G M
	//	4 Y C Y C Y C
	//	5 G M G M G M
	//	6 C Y C Y C Y
	//	7 M G M G M G
	//
	//	PowerShots Pro90 and G1 use 0xb4b4b4b4:
	//
	//	  0 1 2 3 4 5
	//	0 G M G M G M
	//	1 Y C Y C Y C

	NeedMosaicInfo ();
	
	dng_mosaic_info &info = *fMosaicInfo.Get ();
							 
	if (((pattern >> 16) & 0x0FFFF) != (pattern & 0x0FFFF))
		{
		info.fCFAPatternSize = dng_point (8, 2);
		}
		
	else if (((pattern >> 8) & 0x0FF) != (pattern & 0x0FF))
		{
		info.fCFAPatternSize = dng_point (4, 2);
		}
		
	else
		{
		info.fCFAPatternSize = dng_point (2, 2);
		}
		
	for (int32 row = 0; row < info.fCFAPatternSize.v; row++)
		{
		
		for (int32 col = 0; col < info.fCFAPatternSize.h; col++)
			{
			
			uint32 index = (pattern >> ((((row << 1) & 14) + (col & 1)) << 1)) & 3;
			
			info.fCFAPattern [row] [col] = info.fCFAPlaneColor [index];
			
			}
			
		}

	info.fColorPlanes = 4;
	
	info.fCFALayout = 1;
			
	}
	
/******************************************************************************/

void dng_negative::SetGreenSplit (uint32 split)
	{
	
	NeedMosaicInfo ();
	
	dng_mosaic_info &info = *fMosaicInfo.Get ();
	
	info.fBayerGreenSplit = split;
	
	}

/*****************************************************************************/

void dng_negative::Parse (dng_host &host,
						  dng_stream &stream,
						  dng_info &info)
	{
	
	// Shared info.
	
	dng_shared &shared = *(info.fShared.Get ());
	
	// Find IFD holding the main raw information.
	
	dng_ifd &rawIFD = *info.fIFD [info.fMainIndex].Get ();
	
	// Model name.
	
	SetModelName (shared.fUniqueCameraModel.Get ());
	
	// Localized model name.
	
	SetLocalName (shared.fLocalizedCameraModel.Get ());
	
	// Base orientation.
	
		{
	
		uint32 orientation = info.fIFD [0]->fOrientation;
		
		if (orientation >= 1 && orientation <= 8)
			{
			
			SetBaseOrientation (dng_orientation::TIFFtoDNG (orientation));
						
			}
			
		}
		
	// Default crop rectangle.
	
	SetDefaultCropSize (rawIFD.fDefaultCropSizeH,
					    rawIFD.fDefaultCropSizeV);

	SetDefaultCropOrigin (rawIFD.fDefaultCropOriginH,
						  rawIFD.fDefaultCropOriginV);
						        
	// Default scale.
		
	SetDefaultScale (rawIFD.fDefaultScaleH,
					 rawIFD.fDefaultScaleV);
	
	// Best quality scale.
	
	SetBestQualityScale (rawIFD.fBestQualityScale);
	
	// Baseline noise.

	SetBaselineNoise (shared.fBaselineNoise.As_real64 ());
	
	// NoiseReductionApplied.
	
	SetNoiseReductionApplied (shared.fNoiseReductionApplied);
	
	// Baseline exposure.
	
	SetBaselineExposure (shared.fBaselineExposure.As_real64 ());

	// Baseline sharpness.
	
	SetBaselineSharpness (shared.fBaselineSharpness.As_real64 ());

	// Chroma blur radius.
	
	SetChromaBlurRadius (rawIFD.fChromaBlurRadius);

	// Anti-alias filter strength.
	
	SetAntiAliasStrength (rawIFD.fAntiAliasStrength);
		
	// Linear response limit.
	
	SetLinearResponseLimit (shared.fLinearResponseLimit.As_real64 ());
	
	// Shadow scale.
	
	SetShadowScale (shared.fShadowScale);
	
	// Colorimetric reference.
	
	SetColorimetricReference (shared.fColorimetricReference);
	
	// Color channels.
		
	SetColorChannels (shared.fCameraProfile.fColorPlanes);
	
	// Analog balance.
	
	if (shared.fAnalogBalance.NotEmpty ())
		{
		
		SetAnalogBalance (shared.fAnalogBalance);
		
		}

	// Camera calibration matrices

	if (shared.fCameraCalibration1.NotEmpty ())
		{
		
		SetCameraCalibration1 (shared.fCameraCalibration1);
		
		}
		
	if (shared.fCameraCalibration2.NotEmpty ())
		{
		
		SetCameraCalibration2 (shared.fCameraCalibration2);
		
		}
		
	if (shared.fCameraCalibration1.NotEmpty () ||
		shared.fCameraCalibration2.NotEmpty ())
		{
		
		SetCameraCalibrationSignature (shared.fCameraCalibrationSignature.Get ());
		
		}

	// Embedded camera profiles.
	
	if (shared.fCameraProfile.fColorPlanes > 1)
		{
	
		if (qDNGValidate || host.NeedsMeta () || host.NeedsImage ())
			{
			
			// Add profile from main IFD.
			
				{
			
				AutoPtr<dng_camera_profile> profile (new dng_camera_profile ());
				
				dng_camera_profile_info &profileInfo = shared.fCameraProfile;
				
				profile->Parse (stream, profileInfo);
				
				// The main embedded profile must be valid.
				
				if (!profile->IsValid (shared.fCameraProfile.fColorPlanes))
					{
					
					ThrowBadFormat ();
					
					}
				
				profile->SetWasReadFromDNG ();
				
				AddProfile (profile);
				
				}
				
			// Extra profiles.

			for (uint32 index = 0; index < (uint32) shared.fExtraCameraProfiles.size (); index++)
				{
				
				try
					{

					AutoPtr<dng_camera_profile> profile (new dng_camera_profile ());
					
					dng_camera_profile_info &profileInfo = shared.fExtraCameraProfiles [index];
					
					profile->Parse (stream, profileInfo);
					
					if (!profile->IsValid (shared.fCameraProfile.fColorPlanes))
						{
						
						ThrowBadFormat ();
						
						}
					
					profile->SetWasReadFromDNG ();
					
					AddProfile (profile);

					}
					
				catch (dng_exception &except)
					{
					
					// Don't ignore transient errors.
					
					if (host.IsTransientError (except.ErrorCode ()))
						{
						
						throw;
						
						}
				
					// Eat other parsing errors.
			
					#if qDNGValidate
					
					ReportWarning ("Unable to parse extra profile");
					
					#endif
					
					}
			
				}
			
			}
			
		// As shot profile name.
		
		if (shared.fAsShotProfileName.NotEmpty ())
			{
			
			SetAsShotProfileName (shared.fAsShotProfileName.Get ());
			
			}
			
		}
		
	// Raw image data digest.
	
	if (shared.fRawImageDigest.IsValid ())
		{
		
		SetRawImageDigest (shared.fRawImageDigest);
		
		}
			
	// Raw data unique ID.
	
	if (shared.fRawDataUniqueID.IsValid ())
		{
		
		SetRawDataUniqueID (shared.fRawDataUniqueID);
		
		}
			
	// Original raw file name.
	
	if (shared.fOriginalRawFileName.NotEmpty ())
		{
		
		SetOriginalRawFileName (shared.fOriginalRawFileName.Get ());
		
		}
		
	// Original raw file data.
	
	if (shared.fOriginalRawFileDataCount)
		{
		
		SetHasOriginalRawFileData (true);
					
		if (host.KeepOriginalFile ())
			{
			
			uint32 count = shared.fOriginalRawFileDataCount;
			
			AutoPtr<dng_memory_block> block (host.Allocate (count));
			
			stream.SetReadPosition (shared.fOriginalRawFileDataOffset);
		
			stream.Get (block->Buffer (), count);
						
			SetOriginalRawFileData (block);
			
			SetOriginalRawFileDigest (shared.fOriginalRawFileDigest);
			
			ValidateOriginalRawFileDigest ();
			
			}
			
		}
			
	// DNG private data.
	
	if (shared.fDNGPrivateDataCount && host.KeepDNGPrivate ())
		{
		
		uint32 length = shared.fDNGPrivateDataCount;
		
		AutoPtr<dng_memory_block> block (host.Allocate (length));
		
		stream.SetReadPosition (shared.fDNGPrivateDataOffset);
			
		stream.Get (block->Buffer (), length);
							
		SetPrivateData (block);
			
		}
		
	// Hand off EXIF metadata to negative.
	
	fExif.Reset (info.fExif.Release ());
	
	// Parse linearization info.
	
	NeedLinearizationInfo ();
	
	fLinearizationInfo.Get ()->Parse (host,
								      stream,
								      info);
								      
	// Parse mosaic info.
	
	if (rawIFD.fPhotometricInterpretation == piCFA)
		{
	
		NeedMosaicInfo ();
		
		fMosaicInfo.Get ()->Parse (host,
							       stream,
							       info);
							  
		}
						  
	}

/*****************************************************************************/

void dng_negative::PostParse (dng_host &host,
						  	  dng_stream &stream,
						  	  dng_info &info)
	{
	
	// Shared info.
	
	dng_shared &shared = *(info.fShared.Get ());
	
	if (host.NeedsMeta ())
		{
		
		// MakerNote.
		
		if (shared.fMakerNoteCount)
			{
			
			// See if we know if the MakerNote is safe or not.
			
			SetMakerNoteSafety (shared.fMakerNoteSafety == 1);
			
			// If the MakerNote is safe, preserve it as a MakerNote.
			
			if (IsMakerNoteSafe ())
				{

				AutoPtr<dng_memory_block> block (host.Allocate (shared.fMakerNoteCount));
				
				stream.SetReadPosition (shared.fMakerNoteOffset);
					
				stream.Get (block->Buffer (), shared.fMakerNoteCount);
									
				SetMakerNote (block);
							
				}
			
			}
		
		// IPTC metadata.
		
		if (shared.fIPTC_NAA_Count)
			{
			
			AutoPtr<dng_memory_block> block (host.Allocate (shared.fIPTC_NAA_Count));
			
			stream.SetReadPosition (shared.fIPTC_NAA_Offset);
			
			uint64 iptcOffset = stream.PositionInOriginalFile();
			
			stream.Get (block->Buffer      (), 
						block->LogicalSize ());
			
			SetIPTC (block, iptcOffset);
							
			}
		
		// XMP metadata.
		
		if (shared.fXMPCount)
			{
			
			AutoPtr<dng_memory_block> block (host.Allocate (shared.fXMPCount));
			
			stream.SetReadPosition (shared.fXMPOffset);
			
			stream.Get (block->Buffer      (),
						block->LogicalSize ());
						
			fValidEmbeddedXMP = SetXMP (host,
										block->Buffer      (),
										block->LogicalSize ());
										
			#if qDNGValidate
			
			if (!fValidEmbeddedXMP)
				{
				ReportError ("The embedded XMP is invalid");
				}
			
			#endif
			
			}
				
		// Color info.
		
		if (!IsMonochrome ())
			{
			
			// If the ColorimetricReference is the ICC profile PCS,
			// then the data must be already be white balanced to the
			// ICC profile PCS white point.
			
			if (ColorimetricReference () == crICCProfilePCS)
				{
				
				ClearCameraNeutral ();
				
				SetCameraWhiteXY (PCStoXY ());
				
				}
				
			else
				{
				  		    
				// AsShotNeutral.
				
				if (shared.fAsShotNeutral.Count () == ColorChannels ())
					{
					
					SetCameraNeutral (shared.fAsShotNeutral);
										
					}
					
				// AsShotWhiteXY.
				
				if (shared.fAsShotWhiteXY.IsValid () && !HasCameraNeutral ())
					{
					
					SetCameraWhiteXY (shared.fAsShotWhiteXY);
					
					}
					
				}
				
			}
					
		}
	
	}
							
/*****************************************************************************/

void dng_negative::SynchronizeMetadata ()
	{
	
	if (!fOriginalExif.Get ())
		{
		
		fOriginalExif.Reset (fExif->Clone ());
		
		}
	
	fXMP->IngestIPTC (*this, fXMPisNewer);
	
	fXMP->SyncExif (*fExif.Get ());
	
	fXMP->SyncOrientation (*this, fXMPinSidecar);
	
	}
					
/*****************************************************************************/

void dng_negative::UpdateDateTime (const dng_date_time_info &dt)
	{
	
	fExif->UpdateDateTime (dt);
	
	fXMP->UpdateDateTime (dt);
	
	}
					
/*****************************************************************************/

void dng_negative::UpdateDateTimeToNow ()
	{
	
	dng_date_time_info dt;
	
	CurrentDateTimeAndZone (dt);
	
	UpdateDateTime (dt);
	
	}
					
/*****************************************************************************/

bool dng_negative::SetFourColorBayer ()
	{
	
	if (ColorChannels () != 3)
		{
		return false;
		}
		
	if (!fMosaicInfo.Get ())
		{
		return false;
		}
		
	if (!fMosaicInfo.Get ()->SetFourColorBayer ())
		{
		return false;
		}
		
	SetColorChannels (4);
	
	if (fCameraNeutral.Count () == 3)
		{
		
		dng_vector n (4);
		
		n [0] = fCameraNeutral [0];
		n [1] = fCameraNeutral [1];
		n [2] = fCameraNeutral [2];
		n [3] = fCameraNeutral [1];
		
		fCameraNeutral = n;
		
		}

	fCameraCalibration1.Clear ();
	fCameraCalibration2.Clear ();
	
	fCameraCalibrationSignature.Clear ();
	
	for (uint32 index = 0; index < (uint32) fCameraProfile.size (); index++)
		{
		
		fCameraProfile [index]->SetFourColorBayer ();
		
		}
			
	return true;
	
	}
					
/*****************************************************************************/

const dng_image & dng_negative::RawImage () const
	{
	
	if (fStage1Image.Get ())
		{
		return *fStage1Image.Get ();
		}
		
	if (fStage2Image.Get ())
		{
		return *fStage2Image.Get ();
		}
		
	DNG_ASSERT (fStage3Image.Get (),
				"dng_negative::RawImage with no raw image");
		    
	return *fStage3Image.Get ();
	
	}
			
/*****************************************************************************/

void dng_negative::ReadStage1Image (dng_host &host,
									dng_stream &stream,
									dng_info &info)
	{
	
	dng_ifd &rawIFD = *info.fIFD [info.fMainIndex].Get ();
	
	fStage1Image.Reset (host.Make_dng_image (rawIFD.Bounds (),
											 rawIFD.fSamplesPerPixel,
											 rawIFD.PixelType ()));
					
	rawIFD.ReadImage (host,
					  stream,
					  *fStage1Image.Get ());							 

	}
					
/*****************************************************************************/

void dng_negative::SetStage1Image (AutoPtr<dng_image> &image)
	{
	
	fStage1Image.Reset (image.Release ());
	
	}

/*****************************************************************************/

void dng_negative::DoBuildStage2 (dng_host &host,
								  uint32 pixelType)
	{
	
	dng_image &stage1 = *fStage1Image.Get ();
		
	dng_linearization_info &info = *fLinearizationInfo.Get ();
	
	fStage2Image.Reset (host.Make_dng_image (info.fActiveArea.Size (),
											 stage1.Planes (),
											 pixelType));
								   
	info.Linearize (host,
					stage1,
					*fStage2Image.Get ());
							 
	}
									   
/*****************************************************************************/

void dng_negative::BuildStage2Image (dng_host &host,
									 uint32 pixelType)
	{
	
	// Finalize linearization info.
	
		{
	
		NeedLinearizationInfo ();
	
		dng_linearization_info &info = *fLinearizationInfo.Get ();
		
		info.PostParse (host, *this);
		
		}
	
	// Perform the linearization.
	
	DoBuildStage2 (host,
			       pixelType);
	
	// Delete the stage1 image now if we are not keeping it around.
	
	if (!host.KeepStage1 ())
		{
		
		ClearStage1 ();
		
		}
		
	}
									  
/*****************************************************************************/

void dng_negative::ClearStage1 ()
	{
	
	if (fStage1Image.Get ())
		{
	
		fStage1Image.Reset ();
		
		ClearRawImageDigest ();
		
		}
	
	// If we are not keeping the stage 1 image, then the linearization
	// information is now useless.
	
	ClearLinearizationInfo ();
		
	}
									  
/*****************************************************************************/

void dng_negative::DoBuildStage3 (dng_host &host,
								  int32 srcPlane)
	{
	
	dng_image &stage2 = *fStage2Image.Get ();
		
	dng_mosaic_info &info = *fMosaicInfo.Get ();
	
	dng_point downScale = info.DownScale (host.MinimumSize   (),
										  host.PreferredSize (),
										  host.CropFactor    ());
	
	if (downScale != dng_point (1, 1))
		{
		SetIsPreview (true);
		}
	
	dng_point dstSize = info.DstSize (downScale);
			
	fStage3Image.Reset (host.Make_dng_image (dng_rect (dstSize),
											 info.fColorPlanes,
											 stage2.PixelType ()));

	if (srcPlane < 0 || srcPlane >= (int32) stage2.Planes ())
		{
		srcPlane = 0;
		}
				
	info.Interpolate (host,
					  *this,
					  stage2,
					  *fStage3Image.Get (),
					  downScale,
					  srcPlane);

	}
									   
/*****************************************************************************/

// Interpolate and merge a multi-channel CFA image.

void dng_negative::DoMergeStage3 (dng_host &host)
	{
	
	// The DNG SDK does not provide multi-channel CFA image merging code.
	// It just grabs the first channel and uses that.
	
	DoBuildStage3 (host, 0);
				   
	// Just grabbing the first channel would often result in the very
	// bright image using the baseline exposure value.
	
	fStage3Gain = pow (2.0, BaselineExposure ());
	
	}
									   
/*****************************************************************************/

void dng_negative::BuildStage3Image (dng_host &host,
									 int32 srcPlane)
	{
	
	// Finalize the mosaic information.
	
	dng_mosaic_info *info = fMosaicInfo.Get ();
	
	if (info)
		{
		
		info->PostParse (host, *this);
		
		}
		
	// If we don't have a mosaic pattern, then just move the stage 2
	// image on to stage 3.
	
	if (!info || !info->IsColorFilterArray ())
		{
		
		fStage3Image.Reset (fStage2Image.Release ());
		
		}
		
	else
		{
		
		// Remember the size of the stage 2 image.
		
		dng_point stage2_size = fStage2Image->Size ();
		
		// Special case multi-channel CFA interpolation.
		
		if ((fStage2Image->Planes () > 1) && (srcPlane < 0))
			{
			
			DoMergeStage3 (host);
			
			}
			
		// Else do a single channel interpolation.
		
		else
			{
				
			DoBuildStage3 (host, srcPlane);
						   
			}
		
		// Calculate the ratio of the stage 3 image size to stage 2 image size.
		
		dng_point stage3_size = fStage3Image->Size ();
		
		fRawToFullScaleH = (real64) stage3_size.h / (real64) stage2_size.h;
		fRawToFullScaleV = (real64) stage3_size.v / (real64) stage2_size.v;
		
		}
		
	// Delete the stage2 image if we are done with it.
	
	if (!host.KeepStage2 ())
		{
		
		ClearStage2 ();
				
		}
		
	}
		
/*****************************************************************************/

void dng_negative::ClearStage2 ()
	{
	
	if (fStage2Image.Get ())
		{
	
		fStage2Image.Reset ();
		
		ClearRawImageDigest ();
		
		}
	
	// If we did not keep the stage 1 image, and we are not keeping
	// the stage 2 image either, then the mosaic information is now
	// useless.
	
	if (!fStage1Image.Get ())
		{
		
		ClearMosaicInfo ();
		
		// To support saving linear DNG files, to need to account for
		// and upscaling during interpolation.

		if (fRawToFullScaleH > 1.0)
			{
			
			uint32 adjust = Round_uint32 (fRawToFullScaleH);
			
			fDefaultCropSizeH  .n *= adjust;
			fDefaultCropOriginH.n *= adjust;
			fDefaultScaleH     .d *= adjust;
			
			fRawToFullScaleH /= (real64) adjust;
			
			}
		
		if (fRawToFullScaleV > 1.0)
			{
			
			uint32 adjust = Round_uint32 (fRawToFullScaleV);
			
			fDefaultCropSizeV  .n *= adjust;
			fDefaultCropOriginV.n *= adjust;
			fDefaultScaleV     .d *= adjust;
			
			fRawToFullScaleV /= (real64) adjust;
			
			}

		}
		
	}
									  
/*****************************************************************************/

void dng_negative::ClearStage3 ()
	{
	
	fStage3Image.Reset ();
	
	}
	
/*****************************************************************************/

dng_exif * dng_negative::MakeExif ()
	{
	
	dng_exif *exif = new dng_exif ();
	
	if (!exif)
		{
		ThrowMemoryFull ();
		}
		
	return exif;
	
	}
		
/*****************************************************************************/

dng_xmp * dng_negative::MakeXMP ()
	{
	
	dng_xmp *xmp = new dng_xmp (Allocator ());
	
	if (!xmp)
		{
		ThrowMemoryFull ();
		}
		
	return xmp;
	
	}
		
/*****************************************************************************/

dng_linearization_info * dng_negative::MakeLinearizationInfo ()
	{
	
	dng_linearization_info *info = new dng_linearization_info ();
	
	if (!info)
		{
		ThrowMemoryFull ();
		}
		
	return info;
	
	}

/*****************************************************************************/

void dng_negative::NeedLinearizationInfo ()
	{
	
	if (!fLinearizationInfo.Get ())
		{
	
		fLinearizationInfo.Reset (MakeLinearizationInfo ());
		
		}
	
	}

/*****************************************************************************/

dng_mosaic_info * dng_negative::MakeMosaicInfo ()
	{
	
	dng_mosaic_info *info = new dng_mosaic_info ();
	
	if (!info)
		{
		ThrowMemoryFull ();
		}
		
	return info;
	
	}

/*****************************************************************************/

void dng_negative::NeedMosaicInfo ()
	{
	
	if (!fMosaicInfo.Get ())
		{
	
		fMosaicInfo.Reset (MakeMosaicInfo ());
		
		}
	
	}

/*****************************************************************************/
