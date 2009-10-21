/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-09
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
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

#include "captiondialog.h"
#include "captiondialog.moc"

// Qt includes

#include <QFont>

// Local includes

#include "commoncontainer.h"

namespace KIPIAdvancedSlideshowPlugin
{

CaptionDialog::CaptionDialog( QWidget* parent, SharedContainer* sharedData)
             : QWidget(parent)
{
    setupUi(this);
    m_sharedData = sharedData;
    m_commentsFontChooser->setSampleText(i18n("AdvancedSlideshow is part of "
                                              "KIPI-Plugins (http://www.kipi-plugins.org)"));
}

CaptionDialog::~CaptionDialog()
{
}

void CaptionDialog::readSettings()
{
    connect(m_commentsFontColor, SIGNAL(changed(const QColor&)), 
            this, SLOT(slotCommentsFontColorChanged()));

    connect(m_commentsBgColor, SIGNAL(changed(const QColor&)), 
            this, SLOT(slotCommentsBgColorChanged()));

    connect(m_transparentBgCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(slotTransparentBgToggled()));

    m_commentsLinesLengthSpinBox->setValue(m_sharedData->commentsLinesLength);
    m_commentsFontColor->setColor(QColor(m_sharedData->commentsFontColor));
    m_commentsBgColor->setColor(QColor(m_sharedData->commentsBgColor));
    m_commentsBgColor->setEnabled(!m_sharedData->transparentBg);
    m_transparentBgCheckBox->setChecked(m_sharedData->transparentBg);
    m_commentsFontChooser->setFont(*(m_sharedData->captionFont));

    slotTransparentBgToggled();
}

void CaptionDialog::saveSettings()
{
    delete m_sharedData->captionFont;
    m_sharedData->captionFont         = new QFont(m_commentsFontChooser->font());
    QColor fontColor                  = QColor(m_commentsFontColor->color());
    m_sharedData->commentsFontColor   = fontColor.rgb();
    QColor bgColor                    = QColor(m_commentsBgColor->color());
    m_sharedData->commentsBgColor     = bgColor.rgb();
    m_sharedData->transparentBg       = m_transparentBgCheckBox->isChecked();
    m_sharedData->commentsLinesLength = m_commentsLinesLengthSpinBox->value();
}

// --- Slots

void CaptionDialog::slotCommentsBgColorChanged()
{
    m_commentsFontChooser->setBackgroundColor(m_commentsBgColor->color());
}

void CaptionDialog::slotCommentsFontColorChanged()
{
    m_commentsFontChooser->setColor(m_commentsFontColor->color());
}

void CaptionDialog::slotTransparentBgToggled()
{
    m_commentsBgColor->setEnabled(!m_transparentBgCheckBox->isChecked());
}

}  // namespace KIPIAdvancedSlideshowPlugin
