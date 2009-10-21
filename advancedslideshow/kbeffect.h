/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-11-14
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2007-2008 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 *
 * Parts of this code are based on smoothslidesaver by Carsten Weinhold
 * <carsten dot weinhold at gmx dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef KBEFFECT_H
#define KBEFFECT_H

namespace KIPIAdvancedSlideshowPlugin
{

class Image;

class SlideShowKB;

class KBEffect
{

public:

    typedef enum
    {
        Fade,
        Blend
    } Type;

public:

    KBEffect(SlideShowKB *parent, bool m_needFadeIn = true);
    virtual ~KBEffect();

    virtual void advanceTime(float step) = 0;
    virtual Type type() = 0;
    virtual bool done() = 0;
    virtual bool fadeIn() const
    {
        return m_needFadeIn;
    };

    static Type chooseKBEffect(Type oldType);

protected:

    void   setupNewImage(int img);
    void   swapImages();
    Image *image(int img);

protected:

    static int m_numKBEffectRepeated;
    bool       m_needFadeIn;
    Image     *m_img[2];

private:

    SlideShowKB *slideWidget;
};

// -------------------------------------------------------------------------

class FadeKBEffect: public KBEffect
{

public:

    FadeKBEffect(SlideShowKB *parent, bool m_needFadeIn = true);
    virtual ~FadeKBEffect();

    virtual void advanceTime(float step);
    virtual Type type()
    {
        return Fade;
    };

    virtual bool done();
};

// -------------------------------------------------------------------------

class BlendKBEffect: public KBEffect
{

public:

    BlendKBEffect(SlideShowKB *parent, bool m_needFadeIn = true);
    virtual ~BlendKBEffect();

    virtual void advanceTime(float step);
    virtual Type type()
    {
        return Blend;
    };

    virtual bool done();
};

}  // namespace KIPIAdvancedSlideshowPlugin

#endif // KBEFFECT_H
