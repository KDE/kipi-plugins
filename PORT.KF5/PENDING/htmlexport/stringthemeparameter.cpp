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

#include "stringthemeparameter.h"

// KDE includes

#include <kconfiggroup.h>
#include <klineedit.h>

namespace KIPIHTMLExport {

QWidget* StringThemeParameter::createWidget(QWidget* parent, const QString& value) const {
	KLineEdit* edit = new KLineEdit(parent);
	edit->setText(value);

	return edit;
}

QString StringThemeParameter::valueFromWidget(QWidget* widget) const {
	Q_ASSERT(widget);
	KLineEdit* edit = static_cast<KLineEdit*>(widget);
	return edit->text();
}

} // namespace
