// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/*
A KIPI plugin to generate HTML image galleries
Copyright 2008 Aurelien Gateau <aurelien dot gateau at free.fr>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/
#include "imagegenerationfunctor.h"

// Qt
#include <QFile>
#include <QImage>
#include <QImageReader>

// KDE
#include <klocale.h>

// KIPI
#include <libkipi/interface.h>

// Local
#include "galleryinfo.h"
#include "generator.h"
#include "imageelement.h"

namespace KIPIHTMLExport {


/**
 * Genearate a square thumbnail from @fullImage of @size x @size pixels
 */
static QImage generateSquareThumbnail(const QImage& fullImage, int size) {
	QImage image = fullImage.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

	if (image.width() != size || image.height() != size) {
		int sx=0, sy=0;
		if (image.width() > size) {
			sx=(image.width() - size) / 2;
		} else {
			sy=(image.height() - size) / 2;
		}
		image = image.copy(sx, sy, size, size);
	}

	return image;
}


ImageGenerationFunctor::ImageGenerationFunctor(Generator* generator, GalleryInfo* info, const QString& destDir)
: mGenerator(generator)
, mInfo(info)
, mDestDir(destDir)
{}


void ImageGenerationFunctor::operator()(ImageElement& element) {
	// Load image
	QString path = element.mPath;
	QFile imageFile(path);
	if (!imageFile.open(QIODevice::ReadOnly)) {
		emitWarning(i18n("Could not read image '%1'", path));
		return;
	}

	QString imageFormat = QImageReader::imageFormat(&imageFile);
	if (imageFormat.isEmpty()) {
		emitWarning(i18n("Format of image '%1' is unknown", path));
		return;
	}
	imageFile.close();
	imageFile.open(QIODevice::ReadOnly);

	QByteArray imageData = imageFile.readAll();
	QImage originalImage;
	if (!originalImage.loadFromData(imageData) ) {
		emitWarning(i18n("Error loading image '%1'", path));
		return;
	}

	// Process images
	QImage fullImage = originalImage;
	if (!mInfo->useOriginalImageAsFullImage()) {
		if (mInfo->fullResize()) {
			int size = mInfo->fullSize();
			fullImage = fullImage.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		}
		if (element.mAngle != 0) {
			QMatrix matrix;
			matrix.rotate(element.mAngle);
			fullImage = fullImage.transformed(matrix);
		}
	}

	QImage thumbnail = generateSquareThumbnail(fullImage, mInfo->thumbnailSize());

	// Save images
	QString baseFileName = Generator::webifyFileName(element.mTitle);
	baseFileName = mUniqueNameHelper.makeNameUnique(baseFileName);

	// Save full
	QString fullFileName;
	if (mInfo->useOriginalImageAsFullImage()) {
		fullFileName = baseFileName + "." + imageFormat.toLower();
		if (!writeDataToFile(imageData, mDestDir + "/" + fullFileName)) {
			return;
		}

	} else {
		fullFileName = baseFileName + "." + mInfo->fullFormatString().toLower();
		QString destPath = mDestDir + "/" + fullFileName;
		if (!fullImage.save(destPath, mInfo->fullFormatString().toAscii(), mInfo->fullQuality())) {
			emitWarning(i18n("Could not save image '%1' to '%2'", path, destPath));
			return;
		}
	}
	element.mFullFileName = fullFileName;
	element.mFullSize = fullImage.size();

	// Save original
	if (mInfo->copyOriginalImage()) {
		QString originalFileName = "original_" + fullFileName;
		if (!writeDataToFile(imageData, mDestDir + "/" + originalFileName)) {
			return;
		}
		element.mOriginalFileName = originalFileName;
		element.mOriginalSize = originalImage.size();
	}

	// Save thumbnail
	QString thumbnailFileName = "thumb_" + baseFileName + "." + mInfo->thumbnailFormatString().toLower();
	QString destPath = mDestDir + "/" + thumbnailFileName;
	if (!thumbnail.save(destPath, mInfo->thumbnailFormatString().toAscii(), mInfo->thumbnailQuality())) {
		mGenerator->logWarningRequested(i18n("Could not save thumbnail for image '%1' to '%2'", path, destPath));
		return;
	}
	element.mThumbnailFileName = thumbnailFileName;
	element.mThumbnailSize = thumbnail.size();
	element.mValid = true;
}


bool ImageGenerationFunctor::writeDataToFile(const QByteArray& data, const QString& destPath) {
	QFile destFile(destPath);
	if (!destFile.open(QIODevice::WriteOnly)) {
		emitWarning(i18n("Could not open file '%1' for writing", destPath));
		return false;
	}
	if (destFile.write(data) != data.size()) {
		emitWarning(i18n("Could not save image to file '%1'", destPath));
		return false;
	}
	return true;
}


void ImageGenerationFunctor::emitWarning(const QString& message) {
	emit mGenerator->logWarningRequested(message);
}

} // namespace
