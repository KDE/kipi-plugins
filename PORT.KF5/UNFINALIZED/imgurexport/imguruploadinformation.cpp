/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-04-21
 * Description : a kipi plugin to export images to the Imgur web service
 *
 * Copyright (C) 2010-2012 by Marius Orcsik <marius at habarnam dot ro>
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

#include "imguruploadinformation.moc"

namespace KIPIImgurExportPlugin
{

void ImgurUploadInformation::ImgurUploadInformation(QWidget* const parent = 0)
    : QDialog(parent)
{
}

void ImgurUploadInformation::~ImgurUploadInformation()
{

}

void ImgurUploadInformation::setTitle(const QString& text)
{
    d.title = text;
}

QString ImgurUploadInformation::title()
{
    return d.title;
}

void ImgurUploadInformation::setCaption(const QString& text)
{
    d.caption = text;
}

QString ImgurUploadInformation::Caption ()
{
    return d.caption;
}

void ImgurUploadInformation::setFileUrl(const KUrl& filePath)
{
    d.fileUrl = filePath;
}

KUrl ImgurUploadInformation::fileUrl()
{
    return d.fileUrl;
}

} // namespace KIPIImgurExportPlugin
