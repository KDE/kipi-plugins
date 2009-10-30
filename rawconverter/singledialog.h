/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-22
 * Description : Raw converter single dialog
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SINGLEDIALOG_H
#define SINGLEDIALOG_H

// Qt includes

#include <QString>
#include <QPixmap>

// KDE includes

#include <kdialog.h>
#include <kurl.h>

class QCloseEvent;

namespace KIPI
{
    class Interface;
}

namespace KIPIRawConverterPlugin
{

class ActionData;
class SingleDialogPriv;

class SingleDialog : public KDialog
{
    Q_OBJECT

public:

    SingleDialog(const QString& file, KIPI::Interface* iface);
    ~SingleDialog();

    void setFile(const QString& file);

protected:

    void closeEvent(QCloseEvent *e);

private:

    void readSettings();
    void saveSettings();

    void busy(bool busy);

    void setIdentity(const KUrl& url, const QString& identity);
    void setThumbnail(const KUrl& url, const QPixmap& thumb);

    void previewing(const KUrl& url);
    void previewed(const KUrl& url, const QString& tmpFile);
    void previewFailed(const KUrl& url);

    void processing(const KUrl& url);
    void processed(const KUrl& url, const QString& tmpFile);
    void processingFailed(const KUrl& url);

private Q_SLOTS:

    void slotDefault();
    void slotClose();
    void slotHelp();
    void slotUser1();
    void slotUser2();
    void slotUser3();

    void slotIdentify();

    void slotPreviewBlinkTimerDone();
    void slotConvertBlinkTimerDone();

    void slotAction(const KIPIRawConverterPlugin::ActionData&);

    void slotThumbnail(const KUrl&, const QPixmap&);

    void slotSixteenBitsImageToggled(bool);

private:

    SingleDialogPriv* const d;
};

} // namespace KIPIRawConverterPlugin

#endif // SINGLEDIALOG_H
