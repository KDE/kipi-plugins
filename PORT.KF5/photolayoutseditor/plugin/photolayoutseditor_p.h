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

#ifndef PHOTOLAYOUTSEDITOR_P_H
#define PHOTOLAYOUTSEDITOR_P_H

// KDE
#include <kaction.h>
#include <kactionmenu.h>
#include <krecentfilesaction.h>
#include <kfiledialog.h>
#include <ktoggleaction.h>
#include <kservicetypetrader.h>

// Qt
#include <QHBoxLayout>
#include <QTreeView>
#include <QDockWidget>

// Local
#include "LayersTree.h"
#include "LayersTreeTitleWidget.h"
#include "ToolsDockWidget.h"
#include "EffectsEditorTool.h"
#include "BorderEditTool.h"
#include "AbstractPhotoEffectFactory.h"
#include "PLEStatusBar.h"
#include "global.h"
#include "photolayoutseditor.h"

namespace KIPIPhotoLayoutsEditor
{
    class PLEStatusBar;
    class PhotoLayoutsEditor::PhotoLayoutsEditorPriv
    {
        public:
            PhotoLayoutsEditorPriv() :
                centralWidget(0),
                openNewFileAction(0),
                openFileAction(0),
                openRecentFilesMenu(0),
                saveAction(0),
                saveAsAction(0),
                saveAsTemplateAction(0),
                exportFileAction(0),
                printPreviewAction(0),
                printAction(0),
                closeAction(0),
                quitAction(0),
                undoAction(0),
                redoAction(0),
                settingsAction(0),
                addImageAction(0),
                showGridToggleAction(0),
                gridConfigAction(0),
                changeCanvasSizeAction(0),
                fileDialog(0),
                tree(0),
                treeWidget(0),
                treeTitle(0),
                toolsWidget(0),
                toolEffects(0),
                toolBorders(0),
                statusBar(0)
            {}

            ~PhotoLayoutsEditorPriv()
            {
                Q_DELETE(centralWidget)

                // File menu
                Q_DELETE(openNewFileAction)
                Q_DELETE(openFileAction)
                Q_DELETE(openRecentFilesMenu)
                Q_DELETE(saveAction)
                Q_DELETE(saveAsAction)
                Q_DELETE(saveAsTemplateAction)
                Q_DELETE(exportFileAction)
                Q_DELETE(printPreviewAction)
                Q_DELETE(printAction)
                Q_DELETE(closeAction)
                Q_DELETE(quitAction)

                // Edit menu
                Q_DELETE(undoAction)
                Q_DELETE(redoAction)
                Q_DELETE(settingsAction)

                // View menu
                Q_DELETE(addImageAction)
                Q_DELETE(showGridToggleAction)
                Q_DELETE(gridConfigAction)
                Q_DELETE(changeCanvasSizeAction)

                // Other
                Q_DELETE(fileDialog)
                Q_DELETE(tree)
                //Q_DELETE(treeWidget)  // DELETED BY main window
                //Q_DELETE(treeTitle)   // DELETED BY treeWidget

                //Q_DELETE(toolsWidget)     // DELETED BY main window
                //Q_DELETE(toolEffects)     // DELETED BY main window
                //Q_DELETE(toolBorders)     // DELETED BY main window
            }

            // Central widget of the window
            QWidget *   centralWidget;

            // File menu
            KAction *   openNewFileAction;
            KAction *   openFileAction;
            KRecentFilesAction *   openRecentFilesMenu;
            KAction *   saveAction;
            KAction *   saveAsAction;
            KAction *   saveAsTemplateAction;
            KAction *   exportFileAction;
            KAction *   printPreviewAction;
            KAction *   printAction;
            KAction *   closeAction;
            KAction *   quitAction;

            // Edit menu
            KAction *   undoAction;
            KAction *   redoAction;
            KAction *   settingsAction;

            // Canvas menu
            KAction *   addImageAction;
            KToggleAction * showGridToggleAction;
            KAction *   gridConfigAction;
            KAction *   changeCanvasSizeAction;

            // File dialog
            KFileDialog * fileDialog;

            // Tree of layers
            LayersTree *  tree;
            QDockWidget * treeWidget;
            LayersTreeTitleWidget * treeTitle;

            // Tools
            ToolsDockWidget * toolsWidget;
            EffectsEditorTool * toolEffects;
            BorderEditTool * toolBorders;

            // Plugins
            QMap<QString, KService::Ptr> effectsServiceMap;
            QMap<QString, KService::Ptr> bordersServiceMap;
            QMap<QString, AbstractPhotoEffectFactory*> effectsMap;
            QMap<QString, BorderDrawerFactoryInterface*> bordersMap;

            PLEStatusBar * statusBar;
    };
}

#endif // PHOTOLAYOUTSEDITOR_P_H
