/* ============================================================
 * File  : gpfileiteminfo.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-01-22
 * Description :
 *
 * Copyright 2003 by Renchi Raju

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

// Local
#include "gpfileiteminfo.h"

namespace KIPIKameraKlientPlugin
{

GPFileItemInfo::GPFileItemInfo() {
    name = "";
    folder = "";
    // ----------------------------------------------------------
    fileInfoAvailable = false;
    mime = "";
    time = "";
    size = -1;
    width = -1;
    height = -1;
    readPermissions = -1;
    writePermissions = -1;
    downloaded = -1;
    // ----------------------------------------------------------
    previewInfoAvailable = false;
    previewMime = "";
    previewSize = -1;
    previewWidth = -1;
    previewHeight = -1;
    previewDownloaded = -1;
    // ----------------------------------------------------------
    audioInfoAvailable = false;
    audioMime = "";
    audioSize = -1;
    audioDownloaded = -1;
    // ----------------------------------------------------------
    viewItem = 0;
}

GPFileItemInfo::~GPFileItemInfo() {
}

GPFileItemInfo::GPFileItemInfo(const GPFileItemInfo& info) {
    name = info.name;
    folder = info.folder;
    // ----------------------------------------------------------
    fileInfoAvailable = info.fileInfoAvailable;
    mime = info.mime;
    time = info.time;
    size = info.size;
    width = info.width;
    height = info.height;
    readPermissions = info.readPermissions;
    writePermissions = info.writePermissions;
    downloaded = info.downloaded;
    // ----------------------------------------------------------
    previewInfoAvailable = info.previewInfoAvailable;
    previewMime = info.previewMime;
    previewSize = info.previewSize;
    previewWidth = info.previewWidth;
    previewHeight = info.previewHeight;
    previewDownloaded = info.previewDownloaded;
    // ----------------------------------------------------------
    audioInfoAvailable = info.audioInfoAvailable;
    audioMime = info.audioMime;
    audioSize = info.audioSize;
    audioDownloaded = info.audioDownloaded;
    // ----------------------------------------------------------
    viewItem = 0;
}

GPFileItemInfo& GPFileItemInfo::operator=(const GPFileItemInfo& info) {
    if (this != &info) {
        name = info.name;
        folder = info.folder;
        // ----------------------------------------------------------
        fileInfoAvailable = info.fileInfoAvailable;
        mime = info.mime;
        time = info.time;
        size = info.size;
        width = info.width;
        height = info.height;
        readPermissions = info.readPermissions;
        writePermissions = info.writePermissions;
        downloaded = info.downloaded;
        // ----------------------------------------------------------
        previewInfoAvailable = info.previewInfoAvailable;
        previewMime = info.previewMime;
        previewSize = info.previewSize;
        previewWidth = info.previewWidth;
        previewHeight = info.previewHeight;
        previewDownloaded = info.previewDownloaded;
        // ----------------------------------------------------------
        audioInfoAvailable = info.audioInfoAvailable;
        audioMime = info.audioMime;
        audioSize = info.audioSize;
        audioDownloaded = info.audioDownloaded;
        // ----------------------------------------------------------
        viewItem = 0;
    }
    return *this;
}

}  // NameSpace KIPIKameraKlientPlugin
