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
#ifndef IMAGEGENERATIONFUNCTOR_H
#define IMAGEGENERATIONFUNCTOR_H

#include "uniquenamehelper.h"


namespace KIPIHTMLExport {

class GalleryInfo;
class Generator;
class ImageElement;

/**
 * This functor generates images (full and thumbnail) for an url and returns an
 * ImageElement initialized to fill the xml writer.
 * It is used as an argument to QtConcurrent::mapped().
 */
class ImageGenerationFunctor {
public:
	typedef ImageElement result_type;

	ImageGenerationFunctor(Generator* generator, GalleryInfo* info, const QString& destDir);

	void operator()(ImageElement& element);

private:
	Generator* mGenerator;
	GalleryInfo* mInfo;
	QString mDestDir;

	UniqueNameHelper mUniqueNameHelper;

	bool writeDataToFile(const QByteArray& data, const QString& destPath);
	void emitWarning(const QString& msg);
};


} // namespace

#endif /* IMAGEGENERATIONFUNCTOR_H */
