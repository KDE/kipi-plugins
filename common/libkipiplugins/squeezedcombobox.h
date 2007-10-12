/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-01-01
 * Description : Kipi-Plugins shared library.
 *               A combo box with a width not depending of text 
 *               content size
 * 
 * Copyright (C) 2005 by Tom Albers <tomalbers@kde.nl>
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com> 
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

/** @file squeezedcombobox.h */

#ifndef SQUEEZEDCOMBOBOX_H
#define SQUEEZEDCOMBOBOX_H

// Qt includes.

#include <QComboBox>
#include <QWidget>

// Libkipi includes.

#include <libkipi/libkipi_export.h>

namespace KIPIPlugins
{

class SqueezedComboBoxPriv;

/** @class SqueezedComboBox
 *
 * This widget is a QComboBox, but then a little bit
 * different. It only shows the right part of the items
 * depending on de size of the widget. When it is not
 * possible to show the complete item, it will be shortened
 * and "..." will be prepended.
 */
class LIBKIPI_EXPORT SqueezedComboBox : public QComboBox
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent parent widget
     * @param name name to give to the widget
     */
    SqueezedComboBox(QWidget *parent = 0, const char *name = 0 );
    /**
     * destructor
     */
    virtual ~SqueezedComboBox();

    /**
     * 
     * Returns true if the combobox contains the original (not-squeezed)
     * version of text.
     * @param text the original (not-squeezed) text to check for
     */
    bool contains(const QString& text) const;

    /**
     * This inserts a item to the list. See QComboBox::insertItem()
     * for details. Please do not use QComboBox::insertItem() to this
     * widget, as that will fail.
     * @param newItem the original (long version) of the item which needs
     *                to be added to the combobox
     * @param index the position in the widget.
     * @param userData custom meta-data assigned to new item.
     */
    void insertSqueezedItem(const QString& newItem, int index, 
                            const QVariant& userData=QVariant());

    /**
     * Append an item.
     * @param newItem the original (long version) of the item which needs
     *                to be added to the combobox
     * @param userData custom meta-data assigned to new item.
     */
    void addSqueezedItem(const QString& newItem, 
                         const QVariant& userData=QVariant());

    /**
     * Set the current item to the one matching the given text.
     *
     * @param itemText the original (long version) of the item text
     */
    void setCurrent(const QString& itemText);

    /**
     * This method returns the full text (not squeezed) of the currently
     * highlighted item.
     * @return full text of the highlighted item
     */
    QString itemHighlighted();

    /**
     * This method returns the full text (not squeezed) for the index.
     * @param index the position in the widget.
     * @return full text of the item
     */
    QString item(int index);

    /**
     * Sets the sizeHint() of this widget.
     */
    virtual QSize sizeHint() const;

private slots:

    void slotTimeOut();
    void slotUpdateToolTip(int index);

private:

    void resizeEvent(QResizeEvent *);
    QString squeezeText(const QString& original);

    // Prevent these from being used.
    QString currentText() const;
    void setCurrentText(const QString& itemText);
    void insertItem(const QString &text);
    void insertItem(qint32 index, const QString &text);
    void addItem(const QString &text);
    QString itemText(int index) const;

private:

    SqueezedComboBoxPriv *d;
};

}  // namespace KIPIPlugins

#endif // SQUEEZEDCOMBOBOX_H
