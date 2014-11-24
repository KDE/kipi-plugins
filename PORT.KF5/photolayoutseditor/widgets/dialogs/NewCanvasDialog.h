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

#ifndef NEWCANVASDIALOG_H
#define NEWCANVASDIALOG_H

#include <kdialog.h>

#include <QListWidgetItem>

namespace KIPIPhotoLayoutsEditor
{
    class CanvasSize;
    class TemplatesModel;

    class NewCanvasDialog : public KDialog
    {
            Q_OBJECT

        public:

            explicit NewCanvasDialog(QWidget* parent = 0);
            ~NewCanvasDialog();

            bool hasTemplateSelected() const;
            QString templateSelected() const;
            CanvasSize canvasSize() const;

        private slots:

            void paperSizeSelected(QListWidgetItem* current, QListWidgetItem* previous);
            void orientationChanged();
            void setHorizontal(bool);
            void setVertical(bool);

        private:

            void setupUI();
            void loadTemplatesList(const QString & path, TemplatesModel* model);

            class Private;
            Private* d;
            friend class Private;

    };
}

#endif // NEWCANVASDIALOG_H
