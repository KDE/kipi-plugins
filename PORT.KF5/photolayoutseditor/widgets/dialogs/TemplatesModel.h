/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-01
 * Description : a plugin to create photo layouts by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011-2012 by Łukasz Spas <lukasz dot spas at gmail dot com>
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

#ifndef TEMPLATESMODEL_H
#define TEMPLATESMODEL_H

#include <QAbstractItemModel>
#include <QMap>
#include <QImage>
#include <QString>
#include <QtSvg/QSvgRenderer>
#include <QPainter>
#include <QFile>
#include <QDomDocument>
#include <QDebug>

namespace KIPIPhotoLayoutsEditor
{
    class TemplateItem : public QObject
    {
            QString fpath, fname;
            QImage image;

        public:

            TemplateItem(const QString & path, const QString & name) :
                fpath(path),
                fname(name)
            {
                fname.remove(".ple");
                if (fpath.isEmpty())
                    return;

		// Try to read preview image
                bool render = false;
		QFile f(path);
		QDomDocument document;
                QString imageAttribute;
		document.setContent(&f, true);

                QDomElement svg = document.firstChildElement("svg");
                if  (svg.isNull())
                    return;

                QDomElement g = svg.firstChildElement("g");
                if  (svg.isNull())
                    return;

                QDomElement defs = g.firstChildElement("defs");
                while(!defs.isNull() && (defs.attribute("id") != "Preview"))
                    defs = defs.nextSiblingElement("defs");
                QDomElement img = defs.firstChildElement("image");

                if (!img.isNull() && !(imageAttribute = img.text()).isEmpty())
                {
                    image = QImage::fromData( QByteArray::fromBase64(imageAttribute.toAscii()) );
                    if (image.isNull())
                        render = true;
                }
		else
                     render = true;
                if (render)
		{
		    // Try to render preview image
                    QSvgRenderer renderer(fpath);
                    if (renderer.isValid())
                    {
                    	image = QImage( renderer.viewBoxF().size().toSize(), QImage::Format_ARGB32 );
                    	image.fill(Qt::white);
                    	QPainter p(&image);
                    	renderer.render(&p);
                    	p.end();
                    }
                }
                image = image.scaled(QSize(100, 100), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }

            QString name() const
            {
                return fname;
            }

            QString path() const
            {
                return fpath;
            }

            QImage icon() const
            {
                return image;
            }
    };

    class TemplatesModel : public QAbstractItemModel
    {
            Q_OBJECT

            QList<TemplateItem*> templates;

        public:

            explicit TemplatesModel(QObject * parent = 0);

            virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
            virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
            virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
            virtual bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
            virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());
            virtual QVariant data(const QModelIndex & index, int role) const;
            virtual QModelIndex parent(const QModelIndex & child) const;

            void addTemplate(const QString & path, const QString & name);

        Q_SIGNALS:

        public Q_SLOTS:

    };
}

#endif // TEMPLATESMODEL_H
