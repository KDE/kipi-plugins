/***************************************************************************
                          utils.h  -  description
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

#ifndef UTILS_H
#define UTILS_H

class QWidget;
class QStringList;

int NINT(double n);
int MIN(int a, int b);
int MAX(int a, int b);

bool launchExternalApp(QStringList &args);
bool checkTempPath(QWidget *parent, QString tempPath);
bool checkRootPath(QWidget *parent, QString rootPath);
bool copyFile(QString src, QString dest);
bool moveFile(QString src, QString dest);

#endif
