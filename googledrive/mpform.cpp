/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Google-Drive web service
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
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

#include "mpform.h"

//c++ includes

#include <cstring>
#include <cstdio>

//Qt includes

#include <QFile>

//KDE includes

#include <kdebug.h>
#include <kmimetype.h>
#include <krandom.h>

namespace KIPIGoogleDrivePlugin{

MPForm::MPForm(){
    m_boundary = KRandom::randomString(42+13).toAscii();
    reset();
}

MPForm::~MPForm(){

}

void MPForm::reset(){
    m_buffer.resize(0);
}

void MPForm::finish(){
    kDebug() << "in finish";
    QString str;
    str += "--";
    str += m_boundary;
    str += "--";
    m_buffer.append(str.toAscii());
    kDebug() << "finish:" << m_buffer;
}

void MPForm::addPair(const QString &name,const QString &description,const QString &path,const QString &id){
    KMimeType::Ptr ptr = KMimeType::findByUrl(path);
    QString mime = ptr->name();
    kDebug() << "in add pair:" << name << " " << description << " " << path << " " << id << " " << mime;
    QString str;

    str += "--";
    str += m_boundary;
    str += "\r\n";
    str += "Content-Type:application/json; charset=UTF-8\r\n\r\n";
    str += "{\"title\":\"";
    str += name.toAscii();
    str += "\",\r\n";
    str += "\"description\":\"";
    str += description.toAscii();                            //add description
    str += "\",\r\n";
    str += "\"mimeType\":\"";
    str += mime.toAscii();                            //add mimetype
    str += "\",\r\n";
    str += "\"parents\":";
    str += "[{";
    str += "\"id\":\"";
    str += id.toAscii();                            //add id
    str += "\"}]\r\n";
    str += "}\r\n";
    m_buffer.append(str.toAscii());
}

bool MPForm::addFile(const QString &path){
    QString str;
    kDebug() << "in addfile" << path;

    KMimeType::Ptr ptr = KMimeType::findByUrl(path);
    QString mime = ptr->name();
    str += "--";
    str += m_boundary;
    str += "\r\n";
    str += "Content-Type: ";
    str += mime.toAscii();
    str += "\r\n\r\n";

    QFile imageFile(path);

    if(!imageFile.open(QIODevice::ReadOnly)){
        return false;
    }

    QByteArray imageData = imageFile.readAll();
    m_file_size = QString("%1").arg(imageFile.size());

    imageFile.close();

    m_buffer.append(str.toAscii());
    m_buffer.append(imageData);
    m_buffer.append("\r\n");

    return true;
}

QByteArray MPForm::formData() const{
    return m_buffer;
}

QString MPForm::boundary() const{
    return m_boundary;
}

QString MPForm::contentType() const{
    return QString("Content-Type: multipart/related;boundary="+m_boundary);
}

QString MPForm::getFileSize() const{
    return m_file_size;
}

}//namespace KIPIGoogleDrivePlugin
