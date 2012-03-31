/* ============================================================
*
* This file is a part of kipi-plugins project
* http://www.digikam.org
*
* Date        : 2012-02-02
* Description : a plugin to export photos or videos to ImageShack web service
*
* Copyright (C) 2012 Dodon Victor <dodonvictor at gmail dot com>
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

#ifndef IMAGESHACKWINDOW_H
#define IMAGESHACKWINDOW_H

// KDE includes

#include <kdialog.h>

// Qt includes

#include <QList>

// Debug

#include <QTextStream>
#include <QFile>

namespace KIPI
{
    class Interface;
}

namespace KIPIPlugins
{
    class KPAboutData;
    class KPImagesList;
}

namespace KIPIImageshackExportPlugin
{

class Imageshack;
class ImageshackWidget;
class ImageshackTalker;

class ImageshackWindow : public KDialog
{
    Q_OBJECT

public:

    ImageshackWindow(KIPI::Interface* const interface, QWidget* const parent, Imageshack* const pImageshack);
    ~ImageshackWindow();

    KIPIPlugins::KPImagesList* getImagesList() const;

Q_SIGNALS:

    void signalBusy(bool val);

private Q_SLOTS:

    void slotHelp();
    void slotImageListChanged();
    void slotStartTransfer();
    void slotBusy(bool val);

    void slotButtonClicked(int button);
    void slotNeedRegistrationCode();
    void slotLoginInProgress(int step, int maxStep, const QString& format);
    void slotLoginDone(int errCode, const QString& errMsg);

    void slotAddPhotoDone(int errCode, const QString& errMsg);

private:

    void readSettings();
    void saveSettings();

    void authenticate();
    void askRegistrationCode();
    void authenticationDone(int errCode, const QString& errMsg);

    void uploadNextItem();

    void closeEvent(QCloseEvent* e);

private Q_SLOTS:

    void slotChangeRegistrantionCode();

private:

    bool                      m_import;
    unsigned int              m_imagesCount;
    unsigned int              m_imagesTotal;

    KUrl::List                m_transferQueue;

    Imageshack*               m_imageshack;
    ImageshackWidget*         m_widget;
    ImageshackTalker*         m_talker;

    KIPI::Interface*          m_interface;
    KIPIPlugins::KPAboutData* m_about;
};

} // namespace KIPIImageshackExportPlugin

#endif // IMAGESHACKWINDOW_H
