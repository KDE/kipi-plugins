/* ============================================================
 * Copyright 2003 by Renchi Raju
 * Copyright 2004 by Tudor Calin

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
#ifndef SAVEFILEDIALOG_H
#define SAVEFILEDIALOG_H

#include <qdialog.h>

class QString;
class QLineEdit;
class QPushButton;

namespace KIPIKameraKlientPlugin
{

class SavefileDialog : public QDialog {
Q_OBJECT
	
public:
	enum Operation { Rename, Skip, SkipAll, Overwrite, OverwriteAll, None };
	SavefileDialog(const QString& file, QWidget *parent=0, const char* name=0, bool modal=true);
	~SavefileDialog();
	Operation saveFileOperation();
	QString   renameFile();
		
private slots:
	void slot_rename() {
		op=Rename; accept();
	}
	void slot_skip() {
		op=Skip; accept();
	}
	void slot_skipAll() {
		op=SkipAll; accept();
	}
	void slot_overwrite() {
		op=Overwrite; accept();
	}
	void slot_overwriteAll() {
		op=OverwriteAll; accept();
	}
	void slot_renameEnabled();
		
private:
	QLineEdit *renameEdit;
	QPushButton *renameBtn;
	Operation op;
};

}  // NameSpace KIPIKameraKlientPlugin

#endif

