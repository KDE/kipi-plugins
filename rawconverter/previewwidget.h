/* ============================================================
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Gilles Caulier <caulier dot gilles at gmail dot com>
 * Date   : 2003-10-22
 * Description : preview raw file widget used in single convert
 *
 * Copyright 2003-2005 by Renchi Raju
 * Copyright 2006-2007 by Gilles Caulier
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

#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

// Qt includes.

#include <qcolor.h>
#include <qframe.h>
#include <qstring.h>

class QPaintEvent;
class QResizeEvent;

namespace KIPIRawConverterPlugin
{
class PreviewWidgetPriv;

class PreviewWidget : public QFrame
{
    Q_OBJECT
    
public:

    PreviewWidget(QWidget *parent);
    ~PreviewWidget();

    void load(const QString& file);
    void setText(const QString& text, const QColor& color=Qt::white);

protected:

    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);

private slots:

    void slotResize();
    
private:

    PreviewWidgetPriv* d;
};

} // NameSpace KIPIRawConverterPlugin

#endif /* PREVIEWWIDGET_H */
