/* ============================================================
 * File  : processcontroller.cpp
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

#include <qprocess.h>
#include <qtimer.h>

extern "C"
{
#include <unistd.h>
#include <time.h>
}

#include "processcontroller.h"

namespace RawConverter
{

ProcessController::ProcessController(QObject *parent)
    : QObject(parent)
{
    dcProcess_ = new QProcess(this);
    connect(dcProcess_, SIGNAL(processExited()),
            SLOT(slotProcessFinished()));

    currTime_ = QString::number(::time(0));
    
    state_ = NONE;
}

ProcessController::~ProcessController()
{
    dcProcess_->tryTerminate();
    dcProcess_->kill();
    if (!tmpFile_.isNull())
        ::unlink(tmpFile_.latin1());
}

void ProcessController::identify(const QStringList& fileList)
{
    fileList_ = fileList;
    identifyOne();
}

void ProcessController::process(const QString& file)
{
    dcProcess_->kill();
    fileList_.clear();
    
    fileCurrent_ = file;

    emit signalBusy(true);
    emit signalProcessing(fileCurrent_);    

    QFileInfo fi(fileCurrent_);
    tmpFile_ = fi.dirPath(true) + QString("/")
               + ".digikam-rawconverter-tmp-"
               + currTime_;

    dcProcess_->clearArguments();

    dcProcess_->addArgument("digikamdcrawclient");
    dcProcess_->addArgument("-o");
    dcProcess_->addArgument(tmpFile_);

    if (settings.cameraWB)
        dcProcess_->addArgument("-w");
    if (settings.fourColorRGB)
        dcProcess_->addArgument("-f");
    dcProcess_->addArgument("-g");
    dcProcess_->addArgument(QString::number(settings.gamma));
    dcProcess_->addArgument("-b");
    dcProcess_->addArgument(QString::number(settings.brightness));
    dcProcess_->addArgument("-r");
    dcProcess_->addArgument(QString::number(settings.redMultiplier));
    dcProcess_->addArgument("-l");
    dcProcess_->addArgument(QString::number(settings.blueMultiplier));
    dcProcess_->addArgument("-F");
    dcProcess_->addArgument(settings.outputFormat);
    dcProcess_->addArgument(fileCurrent_);
    dcProcess_->start();

    state_ = PROCESS;
}

void ProcessController::preview(const QString& file)
{
    dcProcess_->kill();
    fileList_.clear();

    fileCurrent_ = file;
    
    emit signalBusy(true);
    emit signalPreviewing(fileCurrent_);

    QFileInfo fi(fileCurrent_);
    tmpFile_ = fi.dirPath(true) + QString("/")
               + ".digikam-rawconverter-tmp-"
               + currTime_;

    dcProcess_->clearArguments();

    dcProcess_->addArgument("digikamdcrawclient");
    dcProcess_->addArgument("-q");
    dcProcess_->addArgument("-o");
    dcProcess_->addArgument(tmpFile_);

    if (settings.cameraWB)
        dcProcess_->addArgument("-w");
    if (settings.fourColorRGB)
        dcProcess_->addArgument("-f");
    dcProcess_->addArgument("-g");
    dcProcess_->addArgument(QString::number(settings.gamma));
    dcProcess_->addArgument("-b");
    dcProcess_->addArgument(QString::number(settings.brightness));
    dcProcess_->addArgument("-r");
    dcProcess_->addArgument(QString::number(settings.redMultiplier));
    dcProcess_->addArgument("-l");
    dcProcess_->addArgument(QString::number(settings.blueMultiplier));
    dcProcess_->addArgument(fileCurrent_);
    dcProcess_->start();

    state_ = PREVIEW;
}

void ProcessController::abort()
{
    dcProcess_->tryTerminate();
    QTimer::singleShot( 100, dcProcess_, SLOT( kill() ) );
}

void ProcessController::identifyOne()
{
    if (fileList_.empty()) return;
    
    fileCurrent_ = fileList_.first();
    fileList_.pop_front();
    
    dcProcess_->clearArguments();

    dcProcess_->addArgument("digikamdcrawclient");
    dcProcess_->addArgument("-i");

    QFileInfo fi(fileCurrent_);
    dcProcess_->addArgument("-D");
    dcProcess_->addArgument(fi.dirPath(true));
    dcProcess_->addArgument(fi.fileName());
    dcProcess_->start();

    state_ = IDENTIFY;
}


void ProcessController::slotProcessFinished()
{
    emit signalBusy(false);

    switch (state_) {
    case(IDENTIFY): {
        QString identity = QString(dcProcess_->readStdout());
        if (!dcProcess_->normalExit() || dcProcess_->exitStatus() != 0) {
            emit signalIdentifyFailed(fileCurrent_, identity);
        }
        else {
            identity.remove(QFileInfo(fileCurrent_).fileName());
            identity.remove(" is a ");
            identity.remove(" image.");
            emit signalIdentified(fileCurrent_, identity);
        }
        identifyOne();
        break;
    }

    case (PROCESS): {

        if (!dcProcess_->normalExit() || dcProcess_->exitStatus() != 0) {
            emit signalProcessingFailed(fileCurrent_);
        }
        else {
            emit signalProcessed(fileCurrent_, tmpFile_);
        }
        break;
    }
        
    case (PREVIEW): {

        if (!dcProcess_->normalExit() || dcProcess_->exitStatus() != 0) {
            emit signalPreviewFailed(fileCurrent_);
        }
        else {
            emit signalPreviewed(fileCurrent_, tmpFile_);
        }
        break;
    }

    default:
        break;
    }
}

}

