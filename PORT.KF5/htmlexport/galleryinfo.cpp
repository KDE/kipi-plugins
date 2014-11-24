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
