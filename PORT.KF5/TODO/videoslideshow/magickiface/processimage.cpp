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
 *         Copyright (C) 2012-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "processimage.moc"

// C++ includes

#include <cmath>

// Local includes

#include "magickiface.h"

#define INC(v) incValue(v, step, steps)
#define DEC(v) decValue(v, step, steps)

namespace KIPIPlugins
{

ProcessImage::ProcessImage(MagickApi* const api)
{
    m_api = api;
}

MagickImage* ProcessImage::aspectRatioCorrection(MagickImage& img, double aspectratio, ASPECTCORRECTION_TYPE aspectcorrection)
{
    MagickImage* newimg    = 0;
    double img_aspectratio = (double) img.getWidth() / (double) img.getHeight();

    // tolerate some error in double value
    if (abs(img_aspectratio - aspectratio) <= 0.001)
        return &img;

    if (aspectcorrection == ASPECTCORRECTION_TYPE_AUTO)
    {
        // find out whether we should use fitin, fillin or none
        if (img_aspectratio > 1.0)
            aspectcorrection = ASPECTCORRECTION_TYPE_FILLIN;
        else
            aspectcorrection = ASPECTCORRECTION_TYPE_FITIN;
    }

    switch (aspectcorrection)
    {
        case ASPECTCORRECTION_TYPE_FITIN:
            //fit in with black bars
            if (img_aspectratio < aspectratio)
            {
                // resulting image will have black bars on right and left side
                if (!(newimg = m_api->createImage("black", img.getHeight() * aspectratio,img.getHeight())))
                {
                    Q_EMIT signalProcessError("couldn't create image\n");
                    return &img;
                }

                m_api->overlayImage(*newimg, (newimg->getWidth() - img.getWidth()) / 2,0, img);
            }
            else
            {
                // resulting image will have black bars on top and bottom
                if (!(newimg = m_api->createImage("black", img.getHeight() * aspectratio, img.getHeight())))
                {
                    Q_EMIT signalProcessError("couldn't create image\n");
                    return &img;
                }

                m_api->overlayImage(*newimg, 0, (newimg->getHeight() - img.getHeight()) / 2, img);
            }
            break;

        case ASPECTCORRECTION_TYPE_FILLIN:
            // part of image is cut off
            if (img_aspectratio < aspectratio)
            {
                // cut on top and bottom side
                if (!(newimg = m_api->createImage("black", img.getHeight() * aspectratio, img.getHeight())))
                {
                    Q_EMIT signalProcessError("couldn't create image\n");
                    return &img;
                }

                m_api->bitblitImage(*newimg, 0, 0, img, 0, (img.getHeight() - newimg->getHeight()) / 2, newimg->getWidth(), newimg->getHeight());
            }
            else
            {
                // cut on right and left side
                if (!(newimg = m_api->createImage("black", img.getHeight() * aspectratio, img.getHeight())))
                {
                    Q_EMIT signalProcessError("couldn't create image\n");
                    return &img;
                }

                m_api->bitblitImage(*newimg, 0, 0, img,(img.getWidth() - newimg->getWidth()) / 2,0, newimg->getWidth(), newimg->getHeight());
            }
            break;

        case ASPECTCORRECTION_TYPE_NONE:
        default:
            // no correction needed
            break;
    }

    if (newimg)
    {
        // swap the images if we got a new one
        m_api->freeImage(img);
        img = *newimg;
    }

    return &img;
}

int ProcessImage::incValue(int v, int step, int steps) const
{
    Q_ASSERT(steps != 0);

    return (v) * (step + 1) / steps;
}

int ProcessImage::decValue(int v, int step, int steps) const
{
    Q_ASSERT(steps != 0);

    return (v) * (steps - step - 1) / steps;
}

MagickImage* ProcessImage::transition(const MagickImage& from, const MagickImage& to, int type, int step, int steps)
{
    int w, h;

    if (step <= 0 || step >= steps)
    {
        Q_EMIT signalProcessError(QString("step: %1 is out of range (%2)").arg(step).arg(steps));
        return 0;
    }

    // create a new target image and copy the from image onto
    MagickImage* const dst = m_api->createImage("black", w = from.getWidth(), h = from.getHeight());

    if (!dst)
    {
        return 0;
    }

    switch (type)
    {
        // sliding

        case TRANSITION_TYPE_SLIDE_L2R:
            m_api->overlayImage(*dst, 0,       0, from);
            m_api->overlayImage(*dst, DEC(-w), 0, to);
            break;

        case TRANSITION_TYPE_SLIDE_R2L:
            m_api->overlayImage(*dst, 0,      0, from);
            m_api->overlayImage(*dst, DEC(w), 0, to);
            break;

        case TRANSITION_TYPE_SLIDE_T2B:
            m_api->overlayImage(*dst, 0, 0,       from);
            m_api->overlayImage(*dst, 0, DEC(-h), to);
            break;

        case TRANSITION_TYPE_SLIDE_B2T:
            m_api->overlayImage(*dst, 0, 0,      from);
            m_api->overlayImage(*dst, 0, DEC(h), to);
            break;

        // pushing

        case TRANSITION_TYPE_PUSH_L2R:
            m_api->overlayImage(*dst, INC(w),  0, from);
            m_api->overlayImage(*dst, DEC(-w), 0, to);
            break;

        case TRANSITION_TYPE_PUSH_R2L:
            m_api->overlayImage(*dst, INC(-w), 0, from);
            m_api->overlayImage(*dst, DEC(w),  0, to);
            break;

        case TRANSITION_TYPE_PUSH_T2B:
            m_api->overlayImage(*dst, 0, INC(h),  from);
            m_api->overlayImage(*dst, 0, DEC(-h), to);
            break;

        case TRANSITION_TYPE_PUSH_B2T:
            m_api->overlayImage(*dst, 0, INC(-h), from);
            m_api->overlayImage(*dst, 0, DEC(h),  to);
            break;

        // swapping

        case TRANSITION_TYPE_SWAP_L2R:
            if (step < steps / 2)
            {
                m_api->overlayImage(*dst, INC(w),  0, to);
                m_api->overlayImage(*dst, INC(-w), 0, from);
            }
            else
            {
                m_api->overlayImage(*dst, DEC(-w), 0, from);
                m_api->overlayImage(*dst, DEC(w),  0, to);
            }
            break;

        case TRANSITION_TYPE_SWAP_R2L:
            if (step < steps / 2)
            {
                m_api->overlayImage(*dst, INC(-w), 0, to);
                m_api->overlayImage(*dst, INC(w),  0, from);
            }
            else
            {
                m_api->overlayImage(*dst, DEC(w),  0, from);
                m_api->overlayImage(*dst, DEC(-w), 0, to);
            }
            break;

        case TRANSITION_TYPE_SWAP_T2B:
            if (step < steps / 2)
            {
                m_api->overlayImage(*dst, 0, INC(h),  to);
                m_api->overlayImage(*dst, 0, INC(-h), from);
            }
            else
            {
                m_api->overlayImage(*dst, 0, DEC(-h), from);
                m_api->overlayImage(*dst, 0, DEC(h),  to);
            }
            break;

        case TRANSITION_TYPE_SWAP_B2T:
            if (step < steps / 2)
            {
                m_api->overlayImage(*dst, 0, INC(-h), to);
                m_api->overlayImage(*dst, 0, INC(h),  from);
            }
            else
            {
                m_api->overlayImage(*dst, 0, DEC(h),  from);
                m_api->overlayImage(*dst, 0, DEC(-h), to);
            }
            break;

        // rolling

        case TRANSITION_TYPE_ROLL_L2R:
            if (INC(w))
                m_api->scaleblitImage(*dst, 0,      0, INC(w), h, to,   0, 0, w, h);
            if (DEC(w))
                m_api->scaleblitImage(*dst, INC(w), 0, DEC(w), h, from, 0, 0, w, h);
            break;

        case TRANSITION_TYPE_ROLL_R2L:
            if (DEC(w))
                m_api->scaleblitImage(*dst, 0,      0, DEC(w), h, from, 0, 0, w, h);
            if (INC(w))
                m_api->scaleblitImage(*dst, DEC(w), 0, INC(w), h, to,   0, 0, w, h);
            break;

        case TRANSITION_TYPE_ROLL_T2B:
            if (INC(h))
                m_api->scaleblitImage(*dst, 0, 0,      w, INC(h), to,   0, 0, w, h);
            if (DEC(h))
                m_api->scaleblitImage(*dst, 0, INC(h), w, DEC(h), from, 0, 0, w, h);
            break;

        case TRANSITION_TYPE_ROLL_B2T:
            if (DEC(h))
                m_api->scaleblitImage(*dst, 0, 0,      w, DEC(h), from, 0, 0, w, h);
            if (INC(h))
                m_api->scaleblitImage(*dst, 0, DEC(h), w, INC(h), to,   0, 0, w, h);
            break;

        // fade

        case TRANSITION_TYPE_FADE:
        default:
            m_api->blendImage(*dst, from, to, 1.0 / steps * step);
            break;
    }

    return dst;
}

GeoImage* ProcessImage::getGeometry(const GeoImage& from, const GeoImage& to, int image_width, int image_height, 
                                    int step, int steps) const
{
    GeoImage* const geometry = new GeoImage();
    // compute the dimesions in current step
    steps--;

    if (step <= 0)
    {
        geometry->x = lround(from.x);
        geometry->y = lround(from.y);
        geometry->w = lround(from.w);
        geometry->h = lround(from.h);
    }
    else if (step >= steps)
    {
        geometry->x = lround(to.x);
        geometry->y = lround(to.y);
        geometry->w = lround(to.w);
        geometry->h = lround(to.h);
    }
    else
    {
        geometry->x = lround(from.x + (to.x - from.x) * (double) step / (double) steps);
        geometry->y = lround(from.y + (to.y - from.y) * (double) step / (double) steps);
        geometry->w = lround(from.w + (to.w - from.w) * (double) step / (double) steps);
        geometry->h = lround(from.h + (to.h - from.h) * (double) step / (double) steps);
    }

    geometry->x = qMin(qMax(geometry->x, 0), image_width  - 1);
    geometry->y = qMin(qMax(geometry->y, 0), image_height - 1);
    geometry->w = qMin(qMax(geometry->w, 0), image_width  - 1 - geometry->x);
    geometry->h = qMin(qMax(geometry->h, 0), image_height - 1 - geometry->y);

    return geometry;
}

} // namespace KIPIPlugins
