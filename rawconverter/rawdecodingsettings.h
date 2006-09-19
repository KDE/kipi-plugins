/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net> 
 * Date   : 2006-12-09
 * Description : Raw decoding settings
 *
 * Copyright 2006 by Gilles Caulier
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef RAWDECODINGSETTINGS_H
#define RAWDECODINGSETTINGS_H

// Qt Includes.

namespace KIPIRawConverterPlugin
{

class RawDecodingSettings
{

public:

    enum OutputFormat 
    {
        JPEG = 0,
        TIFF,
        PPM,
        PNG
    };

    enum DecodingQuality 
    {
        BILINEAR = 0,
        VNG      = 2,
        AHD      = 3
    };

    enum OutputColorSpace 
    {
        RAWCOLOR = 0,
        SRGB,
        ADOBERGB,
        WIDEGAMMUT,
        PROPHOTO
    };

    RawDecodingSettings()
    {
        outputFileFormat        = PNG;

        sixteenBitsImage        = false;
        enableNoiseReduction    = false;
        NRSigmaDomain           = 2.0;
        NRSigmaRange            = 4.0;

        brightness              = 1.0;
        RAWQuality              = BILINEAR;
        outputColorSpace        = SRGB;

        RGBInterpolate4Colors   = false;
        SuperCCDsecondarySensor = false;
        unclipColors            = 0;
        cameraColorBalance      = true;
        automaticColorBalance   = true;
        halfSizeColorImage      = false;
    };
    
    ~RawDecodingSettings(){};

public:

    /** The file format used to convert RAW data. See OutputFormat values 
        for details. 
    */
    OutputFormat outputFileFormat;

    /** If true, decode RAW file in 16 bits per color per pixel else 8 bits.
    */
    bool  sixteenBitsImage;    

    /** Half-size color image decoding (twice as fast as "enableRAWQuality"). 
        Use this option to reduce time loading to render histogram for example, 
        no to render an image to screen. 
    */
    bool  halfSizeColorImage;

    /**  Use the color balance specified by the camera. If this can't be found, 
         reverts to the default. 
    */
    bool  cameraColorBalance;
    
    /** Automatic color balance. The default is to use a fixed color balance 
        based on a white card photographed in sunlight. 
    */
    bool  automaticColorBalance;
    
    /** RAW file decoding using RGB interpolation as four colors. 
    */
    bool  RGBInterpolate4Colors;

    /** For Fuji Super CCD SR cameras, use the secondary sensors. In effect 
        underexposing the image by four stops to reveal detail in the highlights. 
    */
    bool  SuperCCDsecondarySensor;
    
    /** Unclip Highlight color level: 
        0   = Clip all highlights to solid white.
        1   = Leave highlights unclipped in various shades of pink.
        2-9 = Reconstruct highlights. Low numbers favor whites; high numbers 
              favor colors.
    */
    int   unclipColors;

    /** RAW quality decoding factor value. See DecodingQuality values 
        for details. 
    */
    DecodingQuality RAWQuality;

    /** RAW file decoding using bilateral filter to reduce noise. This is '-B 
        sigma_domain sigma_range' dcraw option to smooth noise while preserving 
        edges. sigma_domain is in units of pixels, while sigma_range is in units 
        of CIELab colorspace. 
    */
    bool  enableNoiseReduction;

    /** Noise reduction sigma domain value. 
    */
    float NRSigmaDomain;
    
    /** Noise reduction sigma range value. 
    */
    float NRSigmaRange;    

    /** Brightness of output image. 
    */
    float brightness;   

    /** The output color space used to decoded RAW data. See OutputColorSpace 
        values for details. 
    */
    OutputColorSpace outputColorSpace;
};

}  // namespace KIPIRawConverterPlugin

#endif /* RAWDECODINGSETTINGS_H */
