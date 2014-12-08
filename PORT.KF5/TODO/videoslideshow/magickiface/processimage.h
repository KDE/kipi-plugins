/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2012-06-01
 * @brief  Transitions, AspectRatioCorrection and otherImageEffects
 *
 * @author Copyright (C) 2012      by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
 *         Copyright (C) 2012-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef PROCESSIMAGE_H
#define PROCESSIMAGE_H

// Qt includes

#include <QObject>

namespace KIPIPlugins
{

enum ASPECTCORRECTION_TYPE
{
    ASPECTCORRECTION_TYPE_AUTO,
    ASPECTCORRECTION_TYPE_NONE,
    ASPECTCORRECTION_TYPE_FITIN,
    ASPECTCORRECTION_TYPE_FILLIN
} ;

enum TRANSITION_TYPE
{
    TRANSITION_TYPE_RANDOM,
    TRANSITION_TYPE_NONE,
    TRANSITION_TYPE_FADE,
    TRANSITION_TYPE_SLIDE_L2R,
    TRANSITION_TYPE_SLIDE_R2L,
    TRANSITION_TYPE_SLIDE_T2B,
    TRANSITION_TYPE_SLIDE_B2T,
    TRANSITION_TYPE_PUSH_L2R,
    TRANSITION_TYPE_PUSH_R2L,
    TRANSITION_TYPE_PUSH_T2B,
    TRANSITION_TYPE_PUSH_B2T,
    TRANSITION_TYPE_SWAP_L2R,
    TRANSITION_TYPE_SWAP_R2L,
    TRANSITION_TYPE_SWAP_T2B,
    TRANSITION_TYPE_SWAP_B2T,
    TRANSITION_TYPE_ROLL_L2R,
    TRANSITION_TYPE_ROLL_R2L,
    TRANSITION_TYPE_ROLL_T2B,
    TRANSITION_TYPE_ROLL_B2T
};

enum TRANSITION_SPEED
{
    TRANSITION_SLOW,
    TRANSITION_MEDIUM,
    TRANSITION_FAST
};

enum EFFECT
{
    EFFECT_NONE,
    EFFECT_KENBURN
};

#define TRANS_MAX 18

typedef struct GeoImage
{
    GeoImage(int a = 0, int b = 0, int c = 0, int d = 0)
    {
        x = a;
        y = b;
        w = c;
        h = d;
    }
    int x, y, w, h;
} GeoImage;

class MagickImage;
class MagickApi;

// ---------------------------------------------------------------------------------------

class ProcessImage : public QObject
{
    Q_OBJECT

public:

    explicit ProcessImage(MagickApi* const api);

    /// corrects the aspect ratio of images - not complete
    MagickImage* aspectRatioCorrection(MagickImage& image, double aspectratio, ASPECTCORRECTION_TYPE aspectcorrection);

    /** These functions claculates the required properties at a paricular instance, we can copy them
        frame by frame to a stream and write to a file in the end
        makes an instance of MagickImage which will be shown during given step(0,1,... steps - 1)
     */
    MagickImage* transition(const MagickImage& from, const MagickImage& to, int type, int step, int steps);

    /// calculates the required geometry of image to be shown during a instance for zoom effect
    GeoImage* getGeometry(const GeoImage& from, const GeoImage& to, int image_width, int image_height, int step, int steps) const;

Q_SIGNALS:

    void signalProcessError(const QString& errMess);

private:

    /// increase value with increasing step
    int incValue(int v, int step, int steps) const;

    /// decrease value with increasing step
    int decValue(int v, int step, int steps) const;

private:

    MagickApi* m_api;
};

} // namespace KIPIPlugins

#endif // PROCESSIMAGE_H
