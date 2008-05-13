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
// Self
#include "galleryinfo.h"

// Qt

// KDE
#include <kconfigbase.h>

// Local

namespace KIPIHTMLExport {

static const char* THEME_GROUP_PREFIX="Theme ";

QString GalleryInfo::getThemeParameterValue(
	const QString& theme,
	const QString& parameter,
	const QString& defaultValue) const
{
	QString groupName = THEME_GROUP_PREFIX + theme;
	KConfigGroup group = config()->group(groupName);
	return group.readEntry(parameter, defaultValue);
}


void GalleryInfo::setThemeParameterValue(
	const QString& theme,
	const QString& parameter,
	const QString& value)
{
	// FIXME: This is hackish, but config() is const :'(
	KConfig* localConfig = const_cast<KConfig*>(config());

	QString groupName = THEME_GROUP_PREFIX + theme;
	KConfigGroup group = localConfig->group(groupName);
	group.writeEntry(parameter, value);
}



} // namespace
