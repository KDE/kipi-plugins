/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2012-08-01
 * @brief  encode images to video
 *
 * @author Copyright (C) 2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "encoderdecoder.moc"
#include "../../gstreamerapi.h"

// Qt includes

#include <QDir>

// QtGstreamer includes

#include <QGlib/Error>
#include <QGlib/Connect>
#include <QGst/Init>
#include <QGst/Bus>
#include <QGst/ElementFactory>
#include <QGst/Pad>
#include <QGst/Parse>
#include <QGst/Message>

// KDE includes

#include <kurl.h>

namespace KIPIVideoSlideShowPlugin
{

EncoderDecoder::EncoderDecoder()
{
    QGst::init();

#if GSTREAMER_API_1
    m_audioPipelines.append("filesrc location=\"%1\" ! decodebin ! audioconvert ! audioresample !"
                          "audio/x-raw, rate=%2 ! avenc_mp2 bitrate=%3 ! queue");

    m_audioPipelines.append("filesrc location=\"%1\" ! decodebin ! audioconvert ! audioresample !"
                          "audio/x-raw, rate=%2 ! lamemp3enc bitrate=%3 ! id3v2mux ! queue");

    m_videoPipelines.append("multifilesrc location=\"%1\" caps=image/ppm,framerate=%2 ! avdec_ppm ! videoconvert !"
                          " y4menc ! y4mdec ! videoscale ! mpeg2enc format=%3 norm=%4 bitrate=%5 aspect=%6 ! "
                          " filesink location=\"%7\"");

    m_videoPipelines.append("multifilesrc location=\"%1\" caps=image/ppm,framerate=%2 ! avdec_ppm ! videoconvert !"
                          " avenc_mpeg4 ! avimux ! filesink location=\"%3\"");

    m_videoPipelines.append("multifilesrc location=\"%1\" caps=image/ppm,framerate=%2 ! avdec_ppm ! videoconvert !"
                          " theoraenc ! oggmux ! filesink location=\"%3\"");

    m_videoPipelines.append("multifilesrc location=\"%1\" caps=image/ppm,framerate=%2 ! avdec_ppm ! videoconvert ! "
                          " avenc_mpeg4 ! queue ! mux. filesrc location =\"%3\" ! decodebin ! audioconvert !"
                          " audio/x-raw, rate=44100 ! lamemp3enc ! queue ! mux. avimux name=mux ! filesink location=\"%4\"");
#else
    m_audioPipelines.append("filesrc location=\"%1\" ! decodebin ! audioconvert ! audioresample !"
                          "audio/x-raw-int, rate=%2 ! ffenc_mp2 bitrate=%3 ! queue");

    m_audioPipelines.append("filesrc location=\"%1\" ! decodebin ! audioconvert ! audioresample !"
                          "audio/x-raw-int, rate=%2 ! lamemp3enc bitrate=%3 ! id3v2mux ! queue");

    m_videoPipelines.append("multifilesrc location=\"%1\" caps=image/ppm,framerate=%2 ! ffdec_ppm ! ffmpegcolorspace !"
                          " y4menc ! y4mdec ! videoscale ! mpeg2enc format=%3 norm=%4 bitrate=%5 aspect=%6 ! "
                          " filesink location=\"%7\"");

    m_videoPipelines.append("multifilesrc location=\"%1\" caps=image/ppm,framerate=%2 ! ffdec_ppm ! ffmpegcolorspace !"
                          " xvidenc ! avimux ! filesink location=\"%3\"");

    m_videoPipelines.append("multifilesrc location=\"%1\" caps=image/ppm,framerate=%2 ! ffdec_ppm ! ffmpegcolorspace !"
                          " theoraenc ! oggmux ! filesink location=\"%3\"");

    m_videoPipelines.append("multifilesrc location=\"%1\" caps=image/ppm,framerate=%2 ! ffdec_ppm ! ffmpegcolorspace ! "
                          " xvidenc ! queue ! mux. filesrc location =\"%3\" ! decodebin ! audioconvert !"
                          " audio/x-raw-int, rate=44100 ! lamemp3enc ! queue ! mux. avimux name=mux ! filesink location=\"%4\"");
#endif

}

EncoderDecoder::~EncoderDecoder()
{
    m_pipeline->setState(QGst::StateNull);
}

void EncoderDecoder::onBusMessage(const QGst::MessagePtr& message)
{
    switch(message->type())
    {
        case QGst::MessageAsyncDone:
        case QGst::MessageEos:
            Q_EMIT finished();
            break;

        case QGst::MessageError:
            Q_EMIT encoderError(message.staticCast<QGst::ErrorMessage>()->debugMessage());
            break;

        default:
            break;
    }
}

void EncoderDecoder::cancel()
{
    if(m_pipeline)
    {
        m_pipeline->setState(QGst::StateNull);
        m_pipeline.clear();
    }
}

void EncoderDecoder::encodeVideo(const QString& destination, const QString& audiFile, VIDEO_FORMAT format, VIDEO_TYPE type,
                                 const QString& imagePath, ASPECT_RATIO ratio)
{
    Q_UNUSED(audiFile);
    QString pipeDescr;
    QString location(imagePath + QDir::separator() + "tempvss%d.ppm");

    QString framerate;
    QString typeLetter;
    QString aspectRatio;

    switch(format)
    {
        case VIDEO_FORMAT_PAL:
            framerate  = "25/1";
            typeLetter = "p";
            break;
        case VIDEO_FORMAT_NTSC:
            framerate  = "30000/1001";
            typeLetter = "n";
            break;
        case VIDEO_FORMAT_SECAM:
            framerate  = "25/1";
            typeLetter = "s";
    }

    switch(ratio)
    {
        case ASPECT_RATIO_DEFAULT:
            aspectRatio = "0";
            break;
        case ASPECT_RATIO_4_3:
            aspectRatio = "2";
            break;
        case ASPECT_RATIO_16_9:
            aspectRatio = "3";
            break;
    };

    bool audio = false;

    if(KUrl(audiFile).isValid() && !audiFile.isEmpty())
        audio = true;

    switch(type)
    {
        case VIDEO_AVI:
            if(!audio)
                pipeDescr = m_videoPipelines.at(1).arg(location, "25/1", destination);
            else
                pipeDescr = m_videoPipelines.at(3).arg(location, "25/1", audiFile, destination);
            break;
        case VIDEO_OGG:
            pipeDescr = m_videoPipelines.at(2).arg(location, "25/1", destination);
            break;
        case VIDEO_SVCD:
            pipeDescr = m_videoPipelines.at(0).arg(location, framerate, QString::number(4), typeLetter,
                                                QString::number(2500), aspectRatio, destination);
            break;
        case VIDEO_XVCD:
            pipeDescr = m_videoPipelines.at(0).arg(location, framerate, QString::number(5), typeLetter,
                                                QString::number(2500), aspectRatio, destination);
            break;
        case VIDEO_VCD:
            pipeDescr = m_videoPipelines.at(0).arg(location, framerate, QString::number(1), typeLetter,
                                                QString::number(1150), aspectRatio, destination);
            break;
        case VIDEO_DVD:
            pipeDescr = m_videoPipelines.at(0).arg(location, framerate, QString::number(8), typeLetter,
                                                QString::number(8000), aspectRatio, destination);
            break;
    }

    if(m_pipeline)
    {
        m_pipeline->setState(QGst::StateNull);
        m_pipeline.clear();
    }

    try
    {
        QGst::ElementPtr ptr = QGst::Parse::launch(pipeDescr);

        if(ptr)
            m_pipeline = ptr.dynamicCast<QGst::Pipeline>();
    }
    catch(QGlib::Error e)
    {
        Q_EMIT encoderError(e.message());
    }

    if(!m_pipeline)
    {
        Q_EMIT encoderError("could not create m_pipeline");
        return;
    }

    m_pipeline->bus()->enableSyncMessageEmission();
    QGlib::connect(m_pipeline->bus(), "sync-message", this, &EncoderDecoder::onBusMessage);

    m_pipeline->setState(QGst::StatePlaying);

    /*

    bool audio = true;

    if(audiFile.isEmpty() || !KUrl(audiFile).isValid())
        audio = false;

    QGst::BinPtr audioSrcBin;

    if(audio)
        audioSrcBin = createAudioSrcBin(audiFile, AUDIO_MP2, 32000, 224000);

    QGst::BinPtr videoSrcBin = createVideoSrcBin(type, format);
    QGst::ElementPtr mux;

    switch(type)
    {
        case VIDEO_AVI:
            mux = QGst::ElementFactory::make("avimux");
            break;
        case VIDEO_SVCD:
        case VIDEO_VCD:
        case VIDEO_XVCD:
        case VIDEO_DVD:
            mux = QGst::ElementFactory::make("mplex");
            break;
    }

    QGst::ElementPtr sink = QGst::ElementFactory::make("filesink");

    if ((!audioSrcBin && audio) || !videoSrcBin || !mux || !sink)
    {
        Q_EMIT encoderError("One or more elements could not be created. "
                              "Verify that you have all the necessary element plugins installed.");
        return;
    }

    sink->setProperty("location", destination);

    switch(type)
    {
        case VIDEO_SVCD:
            mux->setProperty("mux-bitrate", 2500);
            break;
            break;
        case VIDEO_VCD:
            mux->setProperty("mux-bitrate", 1150);
            break;
        case VIDEO_XVCD:
            mux->setProperty("mux-bitrate", 2500);
            break;
        case VIDEO_DVD:
            mux->setProperty("mux-bitrate", 8000);
            break;
        default:
            break;
    }

    m_pipeline = QGst::Pipeline::create();

    if(audio)
        m_pipeline->add(audioSrcBin, videoSrcBin, mux, sink);
    else
        m_pipeline->add(videoSrcBin, mux, sink);

    //link elements
    if(audio)
    {
        QGst::PadPtr audioPad = mux->getRequestPad("audio_%d");
        audioSrcBin->getStaticPad("m_src")->link(audioPad);
    }

    QGst::PadPtr videoPad = mux->getRequestPad("video_%d");
    videoSrcBin->getStaticPad("m_src")->link(videoPad);

    mux->link(sink);

    //connect the bus
    QGlib::connect(m_pipeline->bus(), "message", this, &EncoderDecoder::onBusMessage);
    m_pipeline->bus()->addSignalWatch();

    //go!
    m_pipeline->setState(QGst::StatePlaying);

    */
}

/*
 *
QGst::BinPtr EncoderDecoder::createAudioSrcBin(QString file, AUDIO_TYPE type, int sampleRate, int bitRate)
{
    QGst::BinPtr audioBin;

    int index = 0;

    switch(type)
    {
        case AUDIO_MP2:
            index = 0;
            break;
        case AUDIO_MP3:
            index = 1;
            break;
    };

    QString pipeDescr = m_audioPipelines.at(index).arg(file, QString::number(sampleRate), QString::number(bitRate));
    audioBin          = QGst::Bin::fromDescription(pipeDescr);

    if(!audioBin)
    {
        Q_EMIT encoderError("Failed to create audio source bin:");
        return QGst::BinPtr();
    }

    return audioBin;
}

QGst::BinPtr EncoderDecoder::createVideoSrcBin(VIDEO_TYPE type, VIDEO_FORMAT format)
{
    QGst::BinPtr videoBin;

    QString pipeDescr;
    QString framerate;
    QString typeLetter;

    switch(format)
    {
        case VIDEO_FORMAT_PAL:
            framerate  = "25/1";
            typeLetter = "p";
            break;
        case VIDEO_FORMAT_NTSC:
            framerate = "30000/1001";
            typeLetter = "n";
            break;
    }

    switch(type)
    {
        case VIDEO_AVI:
            pipeDescr = m_videoPipelines.at(1).arg("tempvss%d", "25/1");
            break;
        case VIDEO_SVCD:
            pipeDescr = m_videoPipelines.at(0).arg("tempvss%d", framerate, QString::number(4), typeLetter);
            break;
        case VIDEO_VCD:
            pipeDescr = m_videoPipelines.at(0).arg("tempvss%d", framerate, QString::number(1), typeLetter);
            break;
        case VIDEO_XVCD:
            pipeDescr = m_videoPipelines.at(0).arg("tempvss%d", framerate, QString::number(5), typeLetter);
            break;
        case VIDEO_DVD:
            pipeDescr = m_videoPipelines.at(0).arg("tempvss%d").arg(framerate, QString::number(8), typeLetter);
            break;
    }

    videoBin = QGst::Bin::fromDescription(pipeDescr);

    if(!videoBin)
    {
        Q_EMIT encoderError("Failed to create video source bin:");
        return QGst::BinPtr();
    }

    return videoBin;
}

*/

} // namespace KIPIVideoSlideShowPlugin
