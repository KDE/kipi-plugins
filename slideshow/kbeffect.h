/* ============================================================
 * File  : kbeffect.h
 * Author: Valerio Fuoglio <valerio.fuoglio@gmail.com>
 * Date  : 2007-11-14
 * Description : 
 * 
 * Copyright 2007 by Valerio Fuoglio <valerio.fuoglio@gmail.com>
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

/***************************************************************************
 *   Parts of this code are based on smoothslidesaver by Carsten Weinhold  *
 *   <carsten.weinhold@gmx.de>                                             *
 **************************************************************************/

#ifndef KBEFFECT_H
#define KBEFFECT_H
namespace KIPISlideShowPlugin
{
class Image;
class SlideShowKB;

class KBEffect {
public:
    typedef enum { Fade, Blend } Type;
    
    KBEffect(SlideShowKB *parent, bool m_needFadeIn = true);
    virtual ~KBEffect();

    virtual void advanceTime(float step) = 0;
    virtual Type type() = 0;
    virtual bool done() = 0;
    virtual bool fadeIn() const { return m_needFadeIn; };

    static Type chooseKBEffect(Type oldType);

protected:
    void   setupNewImage(int img);
    void   swapImages();
    Image *image(int img);
            
private:
    SlideShowKB *slideWidget;

protected:
    static int m_numKBEffectRepeated;
    bool       m_needFadeIn;
    Image     *m_img[2];
};

// -------------------------------------------------------------------------

class FadeKBEffect: public KBEffect {
public:
    FadeKBEffect(SlideShowKB *parent, bool m_needFadeIn = true);
    virtual ~FadeKBEffect();

    virtual void advanceTime(float step);
    virtual Type type() { return Fade; };
    virtual bool done();
};

// -------------------------------------------------------------------------

class BlendKBEffect: public KBEffect {
public:
    BlendKBEffect(SlideShowKB *parent, bool m_needFadeIn = true);
    virtual ~BlendKBEffect();

    virtual void advanceTime(float step);
    virtual Type type() { return Blend; };
    virtual bool done();
};

}  // NameSpace KIPISlideShowPlugin
#endif

