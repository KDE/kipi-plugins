/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef BORDEROPTIONSDIALOG_H
#define BORDEROPTIONSDIALOG_H

// Include files for KDE

#include <kdialogbase.h>

class KIntNumInput;
class KColorButton;

namespace KIPIBatchProcessImagesPlugin
{

class BorderOptionsDialog : public KDialogBase
{
Q_OBJECT

 public:
   BorderOptionsDialog(QWidget *parent=0, int BorderType = 0);
   ~BorderOptionsDialog();

   KIntNumInput    *m_solidBorderWidth;
   KColorButton    *m_button_solidBorderColor;

   KIntNumInput    *m_lineNiepceBorderWidth;
   KColorButton    *m_button_lineNiepceBorderColor;
   KIntNumInput    *m_NiepceBorderWidth;
   KColorButton    *m_button_NiepceBorderColor;

   KIntNumInput    *m_raiseBorderWidth;

   KIntNumInput    *m_frameBorderWidth;
   KIntNumInput    *m_frameBevelBorderWidth;
   KColorButton    *m_button_frameBorderColor;

 private slots:
   void slotFrameBorderWidthChanged (int value);
};

}  // NameSpace KIPIBatchProcessImagesPlugin

#endif  // BORDEROPTIONSDIALOG_H
