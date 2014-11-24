/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-04-04
 * Description : A KIPI plugin to generate HTML image galleries
 *
 * Copyright (C) 2006-2010 by Aurelien Gateau <aurelien dot gateau at free.fr>
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

// vim: set tabstop=4 shiftwidth=4 noexpandtab:

#ifndef UNIQUENAMEHELPER_H
#define UNIQUENAMEHELPER_H

#include <QStringList>

namespace KIPIHTMLExport {

/**
 * This helper class is used to make sure we use unique filenames
 */
class UniqueNameHelper {
public:
	QString makeNameUnique(QString name) {
		QString nameBase = name;
		int count=2;
		while (mList.indexOf(name)!=-1) {
			name = nameBase + QString::number(count);
			++count;
		};

		mList.append(name);
		return name;
	}

private:
	QStringList mList;
};


} // namespace

#endif /* UNIQUENAMEHELPER_H */
