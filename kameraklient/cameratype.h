/* ============================================================
 * File  : cameratype.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-01-29
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

#ifndef CAMERATYPE_H
#define CAMERATYPE_H

#include <qstring.h>

class CameraType {
    public:
	CameraType();
	CameraType(const QString& model, const QString& port);
	~CameraType();
	CameraType(const CameraType& ctype);
	CameraType& operator=(const CameraType& type);
	void setModel(const QString& model);
	void setPort(const QString& port);
	void setValid(bool valid);
	QString model() const;
	QString port()  const;
	bool valid();

    private:
	QString model_;
	QString port_;
	bool    valid_;
};

#endif 

