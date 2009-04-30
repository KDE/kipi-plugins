/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-02-25
 * Description : a kipi plugin to e-mailing images
 *
 * Copyright (C) 2004-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SENDIMAGES_H
#define SENDIMAGES_H

// Qt includes

#include <QObject>

// Local includes

#include "emailsettingscontainer.h"

namespace KIPI
{
    class Interface;
}

namespace KIPISendimagesPlugin
{
class SendImagesPriv;

class SendImages : public QObject
{

Q_OBJECT

public:

    SendImages(const EmailSettingsContainer& settings, QObject *parent, KIPI::Interface *iface);
    ~SendImages();

    void sendImages();

private Q_SLOTS:

    void slotStartingResize(const KUrl&);
    void slotFinishedResize(const KUrl&, const KUrl&, int);
    void slotFailedResize(const KUrl&, const QString&, int);
    void slotCompleteResize();
    void slotCancel();
    void slotCleanUp();

private:

    void secondStage();
    void buildPropertiesFile();
    bool showFailedResizedImages();
    KUrl::List divideEmails();
    bool invokeMailAgent();
    void invokeMailAgentError(const QString& prog, const QStringList& args);
    void invokeMailAgentDone(const QString& prog, const QStringList& args);

private:

    SendImagesPriv* const d;
};

}  // namespace KIPISendimagesPlugin

#endif  // SENDIMAGES_H
