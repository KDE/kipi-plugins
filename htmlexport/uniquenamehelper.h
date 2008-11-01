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
