/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-12
 * Description : a kipi plugin to export images to the Imgur web service
 *
 * Copyright (C) 2012-2012 by Marius Orcsik <marius at habarnam dot ro>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// local
#include "imgurwindow.h"

using namespace KIPIPlugins;

namespace KIPIImgurExportPlugin
{

ImgurWindow::ImgurWindow(KIPI::Interface* interface, QWidget* parent)
    : KDialog(parent)
{

    setWindowIcon(KIcon("imgur"));
    setWindowTitle(i18n("Export to the imgur.com web service"));

    setButtons(Help | /* User1 |*/ Close);
    setDefaultButton(Close);
    setModal(false);
    //
    m_imagesList = new ImagesList(interface, this);

//        m_imagesList->addActions();
//        ProgressWidget *p = new ProgressWidget(interface);
//        p->show();

    m_imagesList->loadImagesFromCurrentSelection();
    m_imagesList->setAllowDuplicate(false);
    m_imagesList->setAllowRAW(false);


    m_webServiceTalker = new ImgurTalker(interface, this);

    connect(m_webServiceTalker, SIGNAL(signalUploadStart(KUrl)),
            m_imagesList, SLOT(processing(KUrl)));

    connect(m_webServiceTalker, SIGNAL(signalUploadDone(KUrl, bool)),
            m_imagesList, SLOT(processed(KUrl, bool)));

    connect(m_webServiceTalker, SIGNAL(signalUploadProgress(int)),
            m_imagesList, SLOT(slotProgressTimerDone(int)));

    connect(m_imagesList, SIGNAL(signalAddItems(KUrl::List)),
            m_webServiceTalker, SLOT(slotAddItems(KUrl::List)));

    m_webServiceTalker->startUpload();
};

ImgurWindow::~ImgurWindow()
{
    //
};

KIPIPlugins::ImagesList* ImgurWindow::imagesList()
{
    return m_imagesList;
}

void ImgurWindow::reactivate()
{
    m_imagesList->loadImagesFromCurrentSelection();
    show();

}
} // namespace KIPIImgurExportPlugin
