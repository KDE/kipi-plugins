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

#ifndef ENCODERDECODER_H
#define ENCODERDECODER_H

// Qt includes

#include <QStringList>
#include <QObject>

// QtGstreamer includes

#include <QGst/Pipeline>
#include <QGst/Utils/ApplicationSource>

namespace KIPIVideoSlideShowPlugin
{

enum AUDIO_TYPE
{
    AUDIO_MP2,
    AUDIO_MP3
};

enum VIDEO_TYPE
{
    VIDEO_VCD,
    VIDEO_SVCD,
    VIDEO_XVCD,
    VIDEO_DVD,
    VIDEO_AVI,
    VIDEO_OGG
};

enum VIDEO_FORMAT
{
    VIDEO_FORMAT_PAL,
    VIDEO_FORMAT_NTSC,
    VIDEO_FORMAT_SECAM
};

enum ASPECT_RATIO
{
    ASPECT_RATIO_DEFAULT,
    ASPECT_RATIO_4_3,
    ASPECT_RATIO_16_9
};

class EncoderDecoder : public QObject
{
    Q_OBJECT

public:

    EncoderDecoder();
    ~EncoderDecoder();

    void encodeVideo(const QString& destination, const QString& audiFile, VIDEO_FORMAT format, VIDEO_TYPE type,
                     const QString& imagePath, ASPECT_RATIO ratio);
    void cancel();

Q_SIGNALS:

    void encoderError(const QString& message);
    void finished();

private:

    void onBusMessage(const QGst::MessagePtr& message);

    //QGst::BinPtr createAudioSrcBin(QString file, AUDIO_TYPE type, int sampleRate, int bitRate);
    //QGst::BinPtr createVideoSrcBin(VIDEO_TYPE type, VIDEO_FORMAT format);

private:

    QGst::PipelinePtr              m_pipeline;
    QStringList                    m_audioPipelines;
    QStringList                    m_videoPipelines;

    QGst::Utils::ApplicationSource m_src;
};

} // namespace KIPIVideoSlideShowPlugin

#endif // ENCODERDECODER_H
