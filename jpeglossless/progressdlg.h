/* ============================================================
 * File  : progressdlg.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-12-11
 * Description : 
 * 
 * Copyright 2003 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#ifndef PROGRESSDLG_H
#define PROGRESSDLG_H

#include <qdialog.h>

class QProgressBar;
class QLabel;
class QPushButton;

namespace JPEGLossLess
{

class ProgressDlg : public QDialog
{
    Q_OBJECT
    
public:
    
    ProgressDlg();
    ~ProgressDlg();

    void setText(const QString& text);
    void setProgress(int current, int total);
    void reset();

signals:

    void signalCanceled();
    
private:

    QPushButton  *btn_;
    QProgressBar *bar_;
    QLabel       *label_;
    
};

}

#endif /* PROGRESSDLG_H */
