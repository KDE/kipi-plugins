/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-02-04
 * Description : a tool to export or import image to imgur.com
 *
 * Copyright (C) 2010 by Marius Orcisk <marius at habarnam dot ro>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */
#include "webservice.h"

namespace KIPIImgurExportPlugin {

    const QString WebService::apiKey =_IMGUR_API_KEY;

    WebService::WebService (QObject *parent) {

    }

    const QString WebService::getStatusError (WebService::ServerStatusCode code) {
        switch (code) {
        case NO_IMAGE:
            return tr ("No image selected");
            break;
        case UPLOAD_FAILED:
            return tr ("Image failed to upload");
            break;
        case TOO_LARGE:
            return tr ("Image larger than 10MB");
            break;
        case INVALID_TYPE:
            return tr ("Invalid image type or URL");
            break;
        case INVALID_KEY:
            return tr ("Invalid API Key");
            break;
        case PROCESS_FAILED:
            return tr ("Upload failed during process");
            break;
        case COPY_FAILED:
            return tr ("Upload failed during the copy process");
            break;
        case THUMBNAIL_FAILED:
            return tr ("Upload failed during thumbnail process");
            break;
        case UPLOAD_LIMIT:
            return tr ("Upload limit reached");
            break;
        case GIF_TOO_LARGE:
            return tr ("Animated GIF is larger than 2MB");
            break;
        case PNG_TOO_LARGE:
            return tr ("Animated PNG is larger than 2MB");
            break;
        case INVALID_URL:
            return tr ("Invalid URL");
            break;
        case URL_UNAVAILABLE:
            return tr ("Could not download the image from that URL");
            break;
        case INVALID_API_REQUEST:
            return tr ("Invalid API request");
            break;
        case INVALID_RESPONSE_FORMAT:
            return tr ("Invalid response format");
            break;
        default:
            return tr ("Unimplemented");
        }
    }
}
