/* ============================================================
 * File  : kbeffect.cpp
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

#include <assert.h>
#include <qdatetime.h>
#include "kbeffect.h"
#include "slideshowkb.h"

namespace KIPISlideShowPlugin
{

// -------------------------------------------------------------------------

int KBEffect::m_numKBEffectRepeated = 0;

// -------------------------------------------------------------------------

KBEffect::KBEffect(SlideShowKB *parent, bool needFadeIn) {

    this->slideWidget = parent;
    this->m_needFadeIn  = needFadeIn;
}


KBEffect::~KBEffect() {

}


void KBEffect::setupNewImage(int img) {

    slideWidget->setupNewImage(img);
}


void KBEffect::swapImages() {

    slideWidget->swapImages();
}


Image *KBEffect::image(int img) {

    assert(img >= 0 && img < 2);
    return slideWidget->m_image[img];
}


KBEffect::Type KBEffect::chooseKBEffect(KBEffect::Type oldType) {

    KBEffect::Type type;

    do {
        type = (rand() < RAND_MAX / 2) ? KBEffect::Fade : KBEffect::Blend;        
    } while (type == oldType && m_numKBEffectRepeated >= 1);
    
    if (type == oldType)
        m_numKBEffectRepeated++;
    else
        m_numKBEffectRepeated = 0;
        
   return type;
}



// -------------------------------------------------------------------------

FadeKBEffect::FadeKBEffect(SlideShowKB *parent, bool needFadeIn):
    KBEffect(parent, needFadeIn)
{
    m_img[0] = image(0);
}


FadeKBEffect::~FadeKBEffect() {

}


bool FadeKBEffect::done() {

    if (m_img[0]->m_pos >= 1.0) {
        setupNewImage(0);
        return true;
    }
    
    return false;
}


void FadeKBEffect::advanceTime(float step) {

    m_img[0]->m_pos += step;
    if (m_img[0]->m_pos >= 1.0)
        m_img[0]->m_pos = 1.0;
        
    if (m_needFadeIn && m_img[0]->m_pos < 0.1)
        m_img[0]->m_opacity = m_img[0]->m_pos * 10;
    else if (m_img[0]->m_pos > 0.9)
        m_img[0]->m_opacity = (1.0 - m_img[0]->m_pos) * 10;
    else
        m_img[0]->m_opacity = 1.0;    
}

// -------------------------------------------------------------------------

BlendKBEffect::BlendKBEffect(SlideShowKB *parent, bool needFadeIn):
    KBEffect(parent, needFadeIn)
{
    m_img[0] = image(0);
    m_img[1] = 0;
}


BlendKBEffect::~BlendKBEffect() {

}


bool BlendKBEffect::done() {

    if (m_img[0]->m_pos >= 1.0) {
        m_img[0]->m_paint = false;
        swapImages();
        return true;
    }
    
    return false;
}


void BlendKBEffect::advanceTime(float step) {

    m_img[0]->m_pos += step;
    if (m_img[0]->m_pos >= 1.0)
        m_img[0]->m_pos = 1.0;
        
    if (m_img[1])
        m_img[1]->m_pos += step;

    if (m_needFadeIn && m_img[0]->m_pos < 0.1)
        m_img[0]->m_opacity = m_img[0]->m_pos * 10;
    
    else if (m_img[0]->m_pos > 0.9) {
        
        m_img[0]->m_opacity = (1.0 - m_img[0]->m_pos) * 10;
        
        if (m_img[1] == 0) {
            setupNewImage(1);
            m_img[1] = image(1);
            m_img[1]->m_opacity = 1.0;
        }
    
    } else
        m_img[0]->m_opacity = 1.0;
}


}  // NameSpace KIPISlideShowPlugin
