/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-09
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008      by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2012-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PRESENTATION_CAPTION_PAGE_H
#define PRESENTATION_CAPTION_PAGE_H

// Local includes

#include "ui_presentation_captionpage.h"

namespace KIPIAdvancedSlideshowPlugin
{

class PresentationContainer;

class PrensetationCaptionPage : public QWidget, Ui::PrensetationCaptionPage
{
    Q_OBJECT

public:

    PrensetationCaptionPage(QWidget* const parent, PresentationContainer* const sharedData);
    ~PrensetationCaptionPage();

    void readSettings();
    void saveSettings();

private Q_SLOTS:

    void slotCommentsFontColorChanged();
    void slotCommentsBgColorChanged();
    void slotOpenFontDialog();

private:

    PresentationContainer* m_sharedData;
};

} // namespace KIPIAdvancedSlideshowPlugin

#endif // PRESENTATION_CAPTION_PAGE_H
