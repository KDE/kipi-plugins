/* ============================================================
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2003-10-24
 * Description : kipi dcraw process controller
 *
 * Copyright 2003-2005 by Renchi Raju
 * Copyright 2006 by Gilles Caulier
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

#ifndef PROCESSCONTROLLER_H
#define PROCESSCONTROLLER_H

// Qt includes.

#include <qobject.h>
#include <qstringlist.h>

class QProcess;

namespace KIPIRawConverterPlugin
{

struct Settings
{
    bool    cameraWB;
    bool    fourColorRGB;

    float   brightness;
    float   redMultiplier;
    float   blueMultiplier;

    QString outputFormat;
};
    
class ProcessController : public QObject
{
    Q_OBJECT

public:

    enum State 
    {
        NONE = 0,
        IDENTIFY,
        PREVIEW,
        PROCESS
    };
    
    Settings settings;

public:

    ProcessController(QObject *parent);
    ~ProcessController();

    void identify(const QStringList& fileList);
    void process(const QString& file);
    void preview(const QString& file);
    void abort();
    
signals:

    void signalIdentified(const QString& file, const QString& identity);
    void signalIdentifyFailed(const QString& file, const QString& identity);

    void signalProcessing(const QString& file);
    void signalProcessed(const QString& file, const QString& tmpFile);
    void signalProcessingFailed(const QString& file);

    void signalPreviewing(const QString& file);
    void signalPreviewed(const QString& file, const QString& tmpFile);
    void signalPreviewFailed(const QString& file);
    
    void signalBusy(bool val);

private:

    void identifyOne();
    
private slots:

    void slotProcessFinished();
    void slotProcessStdErr();

private:

    QProcess    *dcProcess_;

    QStringList  fileList_;

    QString      fileCurrent_;
    QString      tmpFile_;
    QString      currTime_;

    State        state_;
};

} // NameSpace KIPIRawConverterPlugin

#endif /* PROCESSCONTROLLER_H */
