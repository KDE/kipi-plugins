/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-10-05
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2007 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2004-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
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

#ifndef TOOLBAR_H
#define TOOLBAR_H

// Qt includes.

#include <QWidget>
#include <QKeyEvent>

class QToolButton;

namespace KIPISlideShowPlugin
{

class ToolBar : public QWidget
{
    Q_OBJECT
    
public:

    ToolBar(QWidget* parent);
    ~ToolBar();

    bool canHide() const;
    bool isPaused() const;
    void setPaused(bool val);

    void setEnabledPlay(bool val);
    void setEnabledNext(bool val);
    void setEnabledPrev(bool val);
    
signals:

    void signalNext();
    void signalPrev();
    void signalClose();
    void signalPlay();
    void signalPause();

protected:

    void keyPressEvent(QKeyEvent *event);
    
private slots:

    void slotPlayBtnToggled();
    void slotNexPrevClicked();

private:

    QToolButton* m_playBtn;
    QToolButton* m_stopBtn;
    QToolButton* m_nextBtn;
    QToolButton* m_prevBtn;
    bool         m_canHide;

    friend class SlideShow;
    friend class SlideShowGL;
};

} // namespace KIPISlideShowPlugin

#endif /* TOOLBAR_H */
