// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/*
A KIPI plugin to generate HTML image galleries
Copyright 2006 Aurelien Gateau <aurelien dot gateau at free.fr>

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
#ifndef GENERATOR_H
#define GENERATOR_H

#include <qobject.h>

#include "uniquenamehelper.h"

class KUrl;

namespace KIPIPlugins {
class BatchProgressDialog;
}

namespace KIPI {
class Interface;
}

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

	ImageGenerationFunctor(Generator* generator, KIPI::Interface* iface, GalleryInfo* info, const QString& destDir);

	ImageElement operator()(const KUrl& imageUrl);

private:
	Generator* mGenerator;
	KIPI::Interface* mInterface;
	GalleryInfo* mInfo;
	QString mDestDir;

	UniqueNameHelper mUniqueNameHelper;

	bool writeDataToFile(const QByteArray& data, const QString& destPath);
	void emitWarning(const QString& msg);
};


/**
 * This class is responsible for generating the HTML and scaling the images
 * according to the settings specified by the user.
 */
class Generator : public QObject {
Q_OBJECT
public:
	Generator(KIPI::Interface*, GalleryInfo*, KIPIPlugins::BatchProgressDialog*);
	~Generator();
	bool run();
	bool warnings() const;

Q_SIGNALS:
	/**
	 * This signal is emitted from ImageGenerationFunctor. It uses a
	 * QueuedConnection to switch between the ImageGenerationFunctor thread and
	 * the gui thread.
	 */
	void logWarningRequested(const QString& text);

private:
	struct Private;
	friend struct Private;
	Private* d;

	friend class ImageGenerationFunctor;

private Q_SLOTS:
	void logWarning(const QString& text);
};


} // namespace

#endif /* GENERATOR_H */
