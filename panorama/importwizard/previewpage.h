/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 *
 * Copyright (C) 2011 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#ifndef PREVIEW_PAGE_H
#define PREVIEW_PAGE_H

// Local includes

#include "wizardpage.h"
#include "actions.h"

namespace KIPIPanoramaPlugin
{

class Manager;

class PreviewPage : public KIPIPlugins::WizardPage
{
    Q_OBJECT

public:

    PreviewPage(Manager* mngr, KAssistantDialog* dlg);
    ~PreviewPage();

    void cancel();
    void computePreview();
    void startStitching();
    void resetPage();

Q_SIGNALS:

    void signalPreviewGenerating();
    void signalPreviewGenerated(const KUrl&);
    void signalStitchingFinished();

private Q_SLOTS:

    void slotAction(const KIPIPanoramaPlugin::ActionData&);

private:

    struct PreviewPagePriv;
    PreviewPagePriv* const d;
};

}   // namespace KIPIPanoramaPlugin

#endif /* PREVIEW_PAGE_H */
