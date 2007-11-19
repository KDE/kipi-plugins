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

#ifndef OUTPUTDIALOG_H
#define OUTPUTDIALOG_H

// Include files for Qt

#include <qstring.h>

// Include files for KDE

#include <kdialogbase.h>

// Local includes

#include "kpaboutdata.h"

class QTextView;
class QPushButton;

namespace KIPIBatchProcessImagesPlugin
{

class OutputDialog : public KDialogBase
{
Q_OBJECT

public:

  OutputDialog( QWidget* parent=0, QString caption=QString::null,
                QString Messages=QString::null, QString Header=QString::null );
  ~OutputDialog();

private slots:

  void slotHelp(void);
  void slotCopyToCliboard( void );

private:
  
  QPushButton *m_helpButton;
  QTextView *debugView;
  
  KIPIPlugins::KPAboutData  *m_about;

};

}  // NameSpace KIPIBatchProcessImagesPlugin

#endif  // OUTPUTDIALOG_H

