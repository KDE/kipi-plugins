/***************************************************************************
                          utils.cpp  -  description
                             -------------------
    begin                : Fri Jan 31 2003
    copyright            : (C) 2003 by Todd Shoemaker
    email                : jtshoe11@yahoo.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// C Ansi includes

extern "C"
{
#include <unistd.h>
#include <stdio.h>
}

// Include files for Qt

#include <qstringlist.h>
#include <qwidget.h>
#include <qdir.h>
#include <qprocess.h>

// Include files for KDE

#include <kurl.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kio/netaccess.h>

namespace KIPIPrintWizardPlugin
{

int NINT(double n)
{
  return (int)(n + 0.5);
}

int MIN(int a, int b)
{
  if (a < b)
    return a;
  else
    return b;
}

int MAX(int a, int b)
{
  if (a > b)
    return a;
  else
    return b;
}

// given a list of args, launch this app as a separate thread.
// args[0] is the application to run.
bool launchExternalApp(QString &program, QStringList &args)
{
  QProcess process;

  return process.startDetached(program, args);
}

bool checkTempPath(QWidget *parent, QString tempPath)
{
  // does the temp path exist?
  QDir tempDir(tempPath);
  if (!tempDir.exists())
  {
        if (!tempDir.mkdir(tempDir.path()))
        {
        KMessageBox::sorry(parent,
          i18n("Unable to create a temporary folder; "
               "please make sure you have proper permissions to this folder and try again."));
        return false;
        }
  }
  return true;
}

}  // NameSpace KIPIPrintWizardPlugin

