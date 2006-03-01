// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/*
A KIPI plugin to generate HTML image galleries
Copyright 2006 Aurelien Gateau

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#ifndef GALLERYINFO_H
#define GALLERYINFO_H

// Qt
#include <qvaluelist.h>

// KDE
#include <kurl.h>

// KIPI
#include <libkipi/imagecollection.h>

namespace KIPIHTMLGallery {

struct GalleryInfo {
	KURL mDestURL;
	QValueList<KIPI::ImageCollection> mCollectionList;
	bool mOpenInBrowser;
	QString mTheme;
	
	bool mFullResize;
	int mFullSize;
	QString mFullFormat;

	int mThumbnailSize;
	QString mThumbnailFormat;
};

} // namespace

#endif /* GALLERYINFO_H */
