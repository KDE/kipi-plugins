/* ============================================================
 * File  : cameratype.cpp
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

// Local
#include "cameratype.h"

CameraType::CameraType() {
    valid_  = false;    
}

CameraType::CameraType(const QString& model, const QString& port) {
    model_  = model;
    port_   = port;
    valid_  = true;    
}

CameraType::~CameraType() {
}

CameraType::CameraType(const CameraType& ctype) {
    model_ = ctype.model_;
    port_  = ctype.port_;
    valid_ = ctype.valid_;
}

CameraType& CameraType::operator=(const CameraType& ctype) {
    if (this != &ctype) {
        model_ = ctype.model_;
        port_  = ctype.port_;
        valid_ = ctype.valid_;
    }
    return *this;
}

void CameraType::setModel(const QString& model) {
    model_ = model;
}

void CameraType::setPort(const QString& port) {
    port_  = port;
}

QString CameraType::model() const {
    return model_;
}

QString CameraType::port() const {
    return port_;
}

bool CameraType::valid() {
    return valid_;
}

void CameraType::setValid(bool valid) {
    valid_ = valid;    
}

