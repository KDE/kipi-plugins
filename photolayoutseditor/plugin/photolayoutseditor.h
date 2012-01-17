/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-01
 * Description : a plugin to create photo layouts by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Łukasz Spas <lukasz dot spas at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PHOTOLAYOUTSEDITOR_H
#define PHOTOLAYOUTSEDITOR_H

// Qt
#include <QUndoStack>
#include <QDebug>

// KDE
#include <kxmlguiwindow.h>
#include <kurl.h>

// LibKIPI
#include <libkipi/interface.h>

// Local
#include "CanvasSize.h"

namespace KIPIPhotoLayoutsEditor
{
    class Canvas;
    class CanvasSizeChangeCommand;
    class ProgressEvent;
    class UndoCommandEventFilter;

    class PhotoLayoutsEditor : public KXmlGuiWindow
    {
            Q_OBJECT

        public:

            ~PhotoLayoutsEditor();
            static PhotoLayoutsEditor * instance(QWidget * parent = 0);
            void addUndoCommand(QUndoCommand * command);
            void beginUndoCommandGroup(const QString & name);
            void endUndoCommandGroup();
            void setInterface(KIPI::Interface * interface);
            bool hasInterface() const;
            void setItemsList(const KUrl::List & images);
            KIPI::Interface * interface() const;

        public Q_SLOTS:

            void open();
            void openDialog();
            void open(const KUrl & fileUrl);
            void save();
            void saveAs();
            void saveAsTemplate();
            void saveFile(const KUrl & fileUrl = KUrl(), bool setFileAsDefault = true);
            void exportFile();
            void printPreview();
            void print();
            bool closeDocument();
            void loadNewImage();
            void setGridVisible(bool isVisible);
            void createCanvas(const CanvasSize & size);
            void createCanvas(const KUrl & fileUrl);
            void settings();
            void setupGrid();
            void changeCanvasSize();
            void setTemplateEditMode(bool isEnabled);

        protected:

            void progressEvent(ProgressEvent * event);

        protected Q_SLOTS:

            bool queryClose();
            void refreshActions();
            void addRecentFile(const KUrl & url);
            void clearRecentList();

        private:

            explicit PhotoLayoutsEditor(QWidget * parent = 0);
            static PhotoLayoutsEditor * m_instance;

            void setupActions();
            void createWidgets();
            void loadEffects();
            void loadBorders();
            void prepareSignalsConnections();

        private:

            Canvas*          m_canvas;
            KIPI::Interface* m_interface;

            class PhotoLayoutsEditorPriv;
            PhotoLayoutsEditorPriv* const d;

            friend class UndoCommandEventFilter;
    };
}

#endif // PHOTOLAYOUTSEDITOR_H
