// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/*
A KIPI plugin to generate HTML image galleries
Copyright 2006 by Aurelien Gateau <aurelien dot gateau at free.fr>

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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.

*/
#ifndef GALLERYINFO_H
#define GALLERYINFO_H

// Qt
#include <QList>

// KDE
#include <kurl.h>

// KIPI
#include <libkipi/imagecollection.h>

// Local
#include <theme.h>
#include <htmlexportconfig.h>

namespace KIPIHTMLExport {

/**
 * This class stores all the export settings. It is initialized by the
 * Wizard and read by the Generator.
 */
class GalleryInfo : public Config {
public:
	QString fullFormatString() const {
		return getEnumString("fullFormat");
	}
	
	QString thumbnailFormatString() const {
		return getEnumString("thumbnailFormat");
	}

	QList<KIPI::ImageCollection> mCollectionList;

	QString getThemeParameterValue(const QString& theme, const QString& parameter, const QString& defaultValue) const;

	void setThemeParameterValue(const QString& theme, const QString& parameter, const QString& value);

private:

	/**
	 * KConfigXT enums are mapped to ints.
	 * This method returns the string associated to the enum value.
	 */
	QString getEnumString(const QString& itemName) const {
		// findItem is not marked const :-(
		GalleryInfo* that=const_cast<GalleryInfo*>(this);
		KConfigSkeletonItem* tmp=that->findItem(itemName);

		KConfigSkeleton::ItemEnum* item=dynamic_cast<KConfigSkeleton::ItemEnum*>(tmp);
		Q_ASSERT(item);
		if (!item) return QString::null;

		int value=item->value();
		QList<KConfigSkeleton::ItemEnum::Choice> lst=item->choices();
		QList<KConfigSkeleton::ItemEnum::Choice>::ConstIterator
			it=lst.constBegin(), end=lst.constEnd();

		for (int pos=0; it!=end; ++it, pos++) {
			if (pos==value) {
				return (*it).name;
			}
		}
		return QString::null;
	}
};

} // namespace

#endif /* GALLERYINFO_H */
