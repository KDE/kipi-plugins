/* ============================================================
 * File  : processcontroller.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-10-24
 * Description : 
 * 
 * Copyright 2003 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
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

#include <qobject.h>
#include <qstringlist.h>

class QProcess;

namespace RawConverter
{

struct Settings
{
    bool    cameraWB;
    bool    fourColorRGB;
    float   gamma;
    float   brightness;
    float   redMultiplier;
    float   blueMultiplier;
    QString outputFormat;
};
    
class ProcessController : public QObject
{
    Q_OBJECT

public:

    enum State {
        NONE = 0,
        IDENTIFY,
        PREVIEW,
        PROCESS
    };

    
    ProcessController(QObject *parent);
    ~ProcessController();

    void identify(const QStringList& fileList);
    void process(const QString& file);
    void preview(const QString& file);
    void abort();

    Settings settings;
    
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
    
    State       state_;
    QProcess*   dcProcess_;
    QStringList fileList_;
    QString     fileCurrent_;
    QString     tmpFile_;
    QString     currTime_;

private slots:

    void slotProcessFinished();
};

}

#endif /* PROCESSCONTROLLER_H */
