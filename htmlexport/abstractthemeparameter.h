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
#ifndef ABSTRACTTHEMEPARAMETER_H
#define ABSTRACTTHEMEPARAMETER_H

class QByteArray;
class QString;
class QWidget;
class KConfigGroup;

namespace KIPIHTMLExport {

/**
 * Represents a theme parameter. For each type of parameter, one should inherit
 * from this class and add the necessary code in the Theme class to load the
 * new type.
 */
class AbstractThemeParameter {
public:
	AbstractThemeParameter();
	virtual ~AbstractThemeParameter();

	/**
	 * Reads theme parameters from configGroup. Initializes the internalName,
	 * name and defaultValue fields.
	 */
	virtual void init(const QByteArray& internalName, const KConfigGroup* configGroup);

	QByteArray internalName() const;

	QString name() const;

	QString defaultValue() const;

	/**
	 * This method should return a QWidget representing the parameter,
	 * initialized with value.
	 */
	virtual QWidget* createWidget(QWidget* parent, const QString& value) const = 0;

	/**
	 * The opposite of createWidget: given a widget previously created with
	 * createWidget, this method returns the current widget value.
	 */
	virtual QString valueFromWidget(QWidget*) const = 0;

private:
	class Private;
	Private* d;
};

} // namespace

#endif /* ABSTRACTTHEMEPARAMETER_H */
