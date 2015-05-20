/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2012-07-01
 * @brief  Settings Widget
 *
 * @author Copyright (C) 2012      by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
 *         Copyright (C) 2013-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "slideshowsettingswidget.moc"

// Qt includes

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QString>
#include <QSpacerItem>
#include <QDir>
#include <QCheckBox>

// KDE includes 

#include <kpushbutton.h>
#include <klocalizeddate.h>
#include <kfiledialog.h>
#include <kurl.h>
#include <kseparator.h>
#include <klocalizedstring.h>
#include <kmessagebox.h>

using namespace KIPIPlugins;

namespace KIPIVideoSlideShowPlugin
{

class SlideShowSettingsWidget::Private
{
public:

    Private()
    {
        PPMSettingsWidget   = 0;
        VideoSettingsWidget = 0;
        selectBtn           = 0;
        audioBtn            = 0;
        saveBtn             = 0;
        tempDirLabel        = 0;
        audioFile           = 0;
        timeVal             = 0;
        effects             = 0;
        transitions         = 0;
        transitionSpeed     = 0;
        asptRatioCorrct     = 0;
        videoFormat         = 0;
        videoType           = 0;
        frameHeight         = 0;
        frameWidth          = 0;
        saveFileLabel       = 0;
        checkbox            = 0;
    }

    QWidget*     PPMSettingsWidget;
    QWidget*     VideoSettingsWidget;

    KPushButton* selectBtn;
    KPushButton* audioBtn;
    KPushButton* saveBtn;

    QString      path;
    QString      audioPath;
    QString      savePath;

    QLabel*      tempDirLabel;
    QLabel*      audioFile;
    QLabel*      saveFileLabel;
    QSpinBox *   timeVal;
    QComboBox*   effects;
    QComboBox*   transitions;
    QComboBox*   transitionSpeed;
    QComboBox*   asptRatioCorrct;
    QComboBox*   aspectRatio;
    QComboBox*   videoType;
    QComboBox*   videoFormat;
    QSpinBox*    frameWidth;
    QSpinBox*    frameHeight;
    QCheckBox*   checkbox;
};

SlideShowSettingsWidget::SlideShowSettingsWidget(QWidget* const parent, Qt::WFlags flags)
    : KTabWidget(parent, flags),
      d(new Private)
{
    d->PPMSettingsWidget   = new QWidget();
    d->VideoSettingsWidget = new QWidget();
    addTab(d->PPMSettingsWidget,   i18n("SlideShow Settings"));
    addTab(d->VideoSettingsWidget, i18n("Video Settings"));

    setCloseButtonEnabled(false);
    setHoverCloseButton(false);

    setUpPPMSettings();
    setUpVideoSettings();

    connect(d->timeVal, SIGNAL(valueChanged(int)),
            this, SLOT(timeValueChanged(int)));

    connect(d->effects, SIGNAL(currentIndexChanged(int)),
            this, SLOT(effectIndexChanged(int)));

    connect(d->transitions, SIGNAL(currentIndexChanged(int)),
            this, SLOT(transIndexChanged(int)));

    connect(d->transitionSpeed, SIGNAL(currentIndexChanged(int)),
            this, SLOT(transSpeedIndexChanged(int)));

    connect(d->videoType, SIGNAL(currentIndexChanged(int)),
            this, SLOT(videoTypeChanged(int)));

    connect(d->videoFormat, SIGNAL(currentIndexChanged(int)),
            this, SLOT(videoFormatChanged(int)));
}

SlideShowSettingsWidget::~SlideShowSettingsWidget()
{
    delete d;
}

void SlideShowSettingsWidget::effectIndexChanged(int index)
{
    Q_EMIT effectDataChanged(d->effects->itemText(index), (EFFECT)d->effects->itemData(index).toInt());
}

void SlideShowSettingsWidget::transIndexChanged(int index)
{
    Q_EMIT transDataChanged(d->transitions->itemText(index), (TRANSITION_TYPE)d->transitions->itemData(index).toInt());
}

void SlideShowSettingsWidget::transSpeedIndexChanged(int index)
{
    Q_EMIT transSpeedDataChanged(d->transitionSpeed->itemText(index), (TRANSITION_SPEED)d->transitionSpeed->itemData(index).toInt());
}

void SlideShowSettingsWidget::timeValueChanged(int time)
{
    Q_EMIT timeDataChanged(time);
}

void SlideShowSettingsWidget::videoTypeChanged(int index)
{
    VIDEO_TYPE type     = (VIDEO_TYPE)d->videoType->itemData(index).toInt();
    VIDEO_FORMAT format = (VIDEO_FORMAT)d->videoFormat->itemData(d->videoFormat->currentIndex()).toInt();

    // set frame width and frame height for dvd, vcd,etc to standard tv size
    switch(type)
    {
        case VIDEO_VCD:
            if(format == VIDEO_FORMAT_NTSC)
            {
                d->frameWidth->setValue(352);
                d->frameHeight->setValue(240);
            }
            else
            {
                d->frameWidth->setValue(352);
                d->frameHeight->setValue(288);
            }
            break;

        case VIDEO_DVD:
        case VIDEO_XVCD:
            if(format == VIDEO_FORMAT_NTSC)
            {
                d->frameWidth->setValue(720);
                d->frameHeight->setValue(480);
            }
            else
            {
                d->frameWidth->setValue(720);
                d->frameHeight->setValue(576);
            }
            break;

        case VIDEO_SVCD:
            if(format == VIDEO_FORMAT_NTSC)
            {
                d->frameWidth->setValue(480);
                d->frameHeight->setValue(480);
            }
            else
            {
                d->frameWidth->setValue(576);
                d->frameHeight->setValue(480);
            }
            break;

        default:
            break;
    };
}

void SlideShowSettingsWidget::videoFormatChanged(int index)
{
    VIDEO_TYPE type     = (VIDEO_TYPE)d->videoType->itemData(d->videoType->currentIndex()).toInt();
    VIDEO_FORMAT format = (VIDEO_FORMAT)d->videoFormat->itemData(index).toInt();

    // set frame width and frame height for dvd, vcd,etc to standard tv size
    if (format == VIDEO_FORMAT_NTSC)
    {
        switch(type)
        {
            case VIDEO_DVD:
            case VIDEO_XVCD:
            {
                d->frameWidth->setValue(720);
                d->frameHeight->setValue(480);
            }
            break;

            case VIDEO_SVCD:
            {
                d->frameWidth->setValue(480);
                d->frameHeight->setValue(480);
            }
            break;

            case VIDEO_VCD:
            {
                d->frameWidth->setValue(352);
                d->frameHeight->setValue(240);
            }
            break;

            default:
                break;
        }
    }
    else
    {
        switch(type)
        {
            case VIDEO_DVD:
            case VIDEO_XVCD:
            {
                d->frameWidth->setValue(720);
                d->frameHeight->setValue(576);
            }
            break;

            case VIDEO_SVCD:
            {
                d->frameWidth->setValue(576);
                d->frameHeight->setValue(480);
            }
            break;

            case VIDEO_VCD:
            {
                d->frameWidth->setValue(352);
                d->frameHeight->setValue(288);
            }
            break;

            default:
                break;
        }
    }
}

void SlideShowSettingsWidget::setUpPPMSettings()
{
    QGridLayout* const mainLayout = new QGridLayout(d->PPMSettingsWidget);
    QLabel* const timeLabel       = new QLabel(i18n("Image Display Time"));
    QLabel* const effectLabel     = new QLabel(i18n("Special Effect"));
    QLabel* const transitionLabel = new QLabel(i18n("Transition"));
    QLabel* const transSpeedLabel = new QLabel(i18n("Transition Speed"));

    d->timeVal      = new QSpinBox();
    d->timeVal->setRange(1,60);

    d->effects      = new QComboBox();
    d->effects->setEditable(false);
    d->effects->addItem(i18n("None"),            (int)EFFECT_NONE);
    d->effects->addItem(i18n("Ken-Burn Effect"), (int)EFFECT_KENBURN);

    d->transitions  = new QComboBox();
    d->transitions->setEditable(false);
    d->transitions->addItem(i18n("None"),                (int)TRANSITION_TYPE_NONE);
    d->transitions->addItem(i18n("Random"),              (int)TRANSITION_TYPE_RANDOM);
    d->transitions->addItem(i18n("Fade"),                (int)TRANSITION_TYPE_FADE);
    d->transitions->addItem(i18n("Slide Left to Right"), (int)TRANSITION_TYPE_SLIDE_L2R);
    d->transitions->addItem(i18n("Slide Right to Left"), (int)TRANSITION_TYPE_SLIDE_R2L);
    d->transitions->addItem(i18n("Slide Top to Bottom"), (int)TRANSITION_TYPE_SLIDE_T2B);
    d->transitions->addItem(i18n("Slide Bottom to Top"), (int)TRANSITION_TYPE_SLIDE_B2T);
    d->transitions->addItem(i18n("Push Left to Right"),  (int)TRANSITION_TYPE_PUSH_L2R);
    d->transitions->addItem(i18n("Push Right to Left"),  (int)TRANSITION_TYPE_PUSH_R2L);
    d->transitions->addItem(i18n("Push Top to Bottom"),  (int)TRANSITION_TYPE_PUSH_T2B);
    d->transitions->addItem(i18n("Push Bottom to Top"),  (int)TRANSITION_TYPE_PUSH_B2T);
    d->transitions->addItem(i18n("Swap Left to Right"),  (int)TRANSITION_TYPE_SWAP_L2R);
    d->transitions->addItem(i18n("Swap Right to Left"),  (int)TRANSITION_TYPE_SWAP_R2L);
    d->transitions->addItem(i18n("Swap Top to Bottom"),  (int)TRANSITION_TYPE_SWAP_T2B);
    d->transitions->addItem(i18n("Swap Bottom to Top"),  (int)TRANSITION_TYPE_SWAP_B2T);
    d->transitions->addItem(i18n("Roll Left to Right"),  (int)TRANSITION_TYPE_ROLL_L2R);
    d->transitions->addItem(i18n("Roll Right to Left"),  (int)TRANSITION_TYPE_ROLL_R2L);
    d->transitions->addItem(i18n("Roll Top to Bottom"),  (int)TRANSITION_TYPE_ROLL_T2B);
    d->transitions->addItem(i18n("Roll Bottom to Top"),  (int)TRANSITION_TYPE_ROLL_B2T);

    d->transitionSpeed  = new QComboBox();
    d->transitionSpeed->setEditable(false);
    d->transitionSpeed->addItem(i18n("Slow"),   (int)TRANSITION_SLOW);
    d->transitionSpeed->addItem(i18n("Medium"), (int)TRANSITION_MEDIUM);
    d->transitionSpeed->addItem(i18n("Fast"),   (int)TRANSITION_FAST);

    mainLayout->addWidget(timeLabel,          0, 0, 1, 1);
    mainLayout->addWidget(d->timeVal,         0, 1, 1, 1);
    mainLayout->addWidget(effectLabel,        1, 0, 1, 1);
    mainLayout->addWidget(d->effects,         1, 1, 1, 1);
    mainLayout->addWidget(transitionLabel,    2, 0, 1, 1);
    mainLayout->addWidget(d->transitions,     2, 1, 1, 1); 
    mainLayout->addWidget(transSpeedLabel,    3, 0, 1, 1);
    mainLayout->addWidget(d->transitionSpeed, 3, 1, 1, 1);

    mainLayout->addWidget(new KSeparator(Qt::Horizontal), 4, 0, 1, 2);

    d->tempDirLabel                 = new QLabel(i18n("Temporary Directory"));
    QLabel* const asptRatioCorLabel = new QLabel(i18n("Aspect Ratio Correction"));
    QLabel* const heightLabel       = new QLabel(i18n("Frame Height"));
    QLabel* const widthLabel        = new QLabel(i18n("Frame Width"));
    QLabel* const aspectLabel       = new QLabel(i18n("Aspect Ratio"));
    d->selectBtn                    = new KPushButton(i18n("Browse"));

    connect(d->selectBtn, SIGNAL(clicked(bool)),
            this, SLOT(slotSelectTempDirectory()));

    d->asptRatioCorrct = new QComboBox();
    d->asptRatioCorrct->setEditable(false);
    d->asptRatioCorrct->addItem(i18n("Auto"),    (int)ASPECTCORRECTION_TYPE_AUTO);
    d->asptRatioCorrct->addItem(i18n("None"),    (int)ASPECTCORRECTION_TYPE_NONE);
    d->asptRatioCorrct->addItem(i18n("Fill In"), (int)ASPECTCORRECTION_TYPE_FITIN);
    d->asptRatioCorrct->addItem(i18n("Fit In"),  (int)ASPECTCORRECTION_TYPE_FILLIN);

    d->aspectRatio     = new QComboBox();
    d->aspectRatio->addItem(i18n("default"),     (int)ASPECT_RATIO_DEFAULT);
    d->aspectRatio->addItem(i18n("4:3"),         (int)ASPECT_RATIO_4_3);
    d->aspectRatio->addItem(i18n("16:9"),        (int)ASPECT_RATIO_16_9);

    d->frameHeight     = new QSpinBox();
    d->frameHeight->setRange(100, 1000);
    d->frameWidth      = new QSpinBox();
    d->frameWidth->setRange(100, 1000);

    mainLayout->addWidget(d->tempDirLabel,    5, 0, 1, 2);
    mainLayout->addWidget(d->selectBtn,       6, 1, 1, 1);
    mainLayout->addWidget(asptRatioCorLabel,  7, 0, 1, 1);
    mainLayout->addWidget(d->asptRatioCorrct, 7, 1, 1, 1);
    mainLayout->addWidget(heightLabel,        8, 0, 1, 1);
    mainLayout->addWidget(d->frameHeight,     8, 1, 1, 1);
    mainLayout->addWidget(widthLabel,         9, 0, 1, 1);
    mainLayout->addWidget(d->frameWidth,      9, 1, 1, 1);
    mainLayout->addWidget(aspectLabel,        10, 0, 1, 1);
    mainLayout->addWidget(d->aspectRatio,     10, 1, 1, 1);
    mainLayout->addItem(new QSpacerItem(1,1), 11, 0, 1, 2);
    mainLayout->setRowStretch(11, 10);
}

void SlideShowSettingsWidget::resetToDefault()
{
    d->timeVal->setValue(25);
    d->effects->setCurrentIndex(d->effects->findData((int)EFFECT_NONE));
    d->transitions->setCurrentIndex(d->transitions->findData((int)TRANSITION_TYPE_RANDOM));
    d->transitionSpeed->setCurrentIndex(d->transitionSpeed->findData((int)TRANSITION_MEDIUM));
    d->asptRatioCorrct->setCurrentIndex(d->asptRatioCorrct->findData((int)ASPECTCORRECTION_TYPE_NONE));
    d->frameWidth->setValue(500);
    d->frameHeight->setValue(500);
    d->videoType->setCurrentIndex(d->videoType->findData((int)VIDEO_SVCD));
    d->videoFormat->setCurrentIndex(d->videoFormat->findData((int)VIDEO_FORMAT_PAL));
}

void SlideShowSettingsWidget::setUpVideoSettings()
{
    QGridLayout* const mainLayout = new QGridLayout(d->VideoSettingsWidget);
    QLabel* const typeLabel       = new QLabel(i18n("Video Type"));
    QLabel* const formatLabel     = new QLabel(i18n("Video Format"));

    d->videoType    = new QComboBox();
    d->videoType->setEditable(false);
    d->videoType->addItem(i18n("AVI"),  (int)VIDEO_AVI);
    d->videoType->addItem(i18n("VCD"),  (int)VIDEO_VCD);
    d->videoType->addItem(i18n("SVCD"), (int)VIDEO_SVCD);
    d->videoType->addItem(i18n("XVCD"), (int)VIDEO_XVCD);
    d->videoType->addItem(i18n("DVD"),  (int)VIDEO_DVD);
    d->videoType->addItem(i18n("OGG"),  (int)VIDEO_OGG);

    d->videoFormat  = new QComboBox();
    d->videoFormat->setEditable(false);
    d->videoFormat->addItem(i18n("PAL"),   (int)VIDEO_FORMAT_PAL);
    d->videoFormat->addItem(i18n("NTSC"),  (int)VIDEO_FORMAT_NTSC);
    d->videoFormat->addItem(i18n("SECAM"), (int)VIDEO_FORMAT_SECAM);

    mainLayout->addWidget(typeLabel,                      0, 0, 1, 1);
    mainLayout->addWidget(d->videoType,                   0, 1, 1, 1);
    mainLayout->addWidget(formatLabel,                    1, 0, 1, 1);
    mainLayout->addWidget(d->videoFormat,                 1, 1, 1, 1);
    mainLayout->addWidget(new KSeparator(Qt::Horizontal), 2, 0, 1, 2);

    d->audioFile    = new QLabel(i18n("Audio Disabled"));
    d->audioBtn     = new KPushButton(i18n("Browse"));
    d->checkbox     = new QCheckBox(i18n("Add Audio"));
    d->audioBtn->setEnabled(false);

    d->saveFileLabel = new QLabel(i18n("Save video"));
    d->saveBtn       = new KPushButton(i18n("Browse"));

    connect(d->saveBtn, SIGNAL(clicked(bool)),
            this, SLOT(slotSelectSaveFileName()));

    connect(d->audioBtn, SIGNAL(clicked(bool)),
            this, SLOT(slotSelectAudio()));

    connect(d->checkbox, SIGNAL(stateChanged(int)),
            this, SLOT(slotAudioChecked()));

    mainLayout->addWidget(d->audioFile,     3, 0, 1, 2);
    mainLayout->addWidget(d->audioBtn,      4, 1, 1, 1);
    mainLayout->addWidget(d->checkbox,      5, 0, 1, 2);
    mainLayout->addWidget(d->saveFileLabel, 6, 0, 1, 2);
    mainLayout->addWidget(d->saveBtn,       7, 1, 1, 1);

    mainLayout->addItem(new QSpacerItem(1,1), 10, 0, 1, 2);
    mainLayout->setRowStretch(10, 10);
}

void SlideShowSettingsWidget::slotSelectTempDirectory()
{
    QString path = KFileDialog::getExistingDirectory(KUrl(), this, i18n("Select temporary directory"));

    if (!path.isEmpty())
    {
        setTempDirPath(path);
    }
}

void SlideShowSettingsWidget::slotSelectAudio()
{
    QString path = KFileDialog::getOpenUrl().path();
    d->audioPath = path;
    d->audioFile->setText(path);
}

void SlideShowSettingsWidget::slotAudioChecked()
{
    if (!d->checkbox->isChecked())
    {
        d->audioPath = "";
        d->audioFile->setText(i18n("Audio Disabled"));
        d->audioBtn->setEnabled(false);
    }
    else
    {
       d->audioFile->setText(i18n("Select Audio"));
       d->audioBtn->setEnabled(true);
    }
}

void SlideShowSettingsWidget::slotSelectSaveFileName()
{
    QString path = KFileDialog::getSaveFileName();
    d->saveFileLabel->setText(path);
    d->savePath  = path;
}

QString SlideShowSettingsWidget::getTempDirPath() const
{
    return d->path;
}

void SlideShowSettingsWidget::setTempDirPath(const QString& path)
{
    if (KUrl(path).isValid())
        d->path = path;
    else
        d->path = QDir::tempPath();

    d->tempDirLabel->setText(path);
}

void SlideShowSettingsWidget::updateData(int time, TRANSITION_TYPE transition, TRANSITION_SPEED transSpeed, EFFECT effect)
{
    d->timeVal->setValue(time);
    d->effects->setCurrentIndex(d->effects->findData((int)effect));
    d->transitions->setCurrentIndex(d->transitions->findData((int)transition));
    d->transitionSpeed->setCurrentIndex(d->transitionSpeed->findData((int)transSpeed));
}

ASPECTCORRECTION_TYPE SlideShowSettingsWidget::getAspectCorrection() const
{
    return (ASPECTCORRECTION_TYPE)d->asptRatioCorrct->itemData(d->asptRatioCorrct->currentIndex()).toInt();
}

ASPECT_RATIO SlideShowSettingsWidget::getAspectRatio() const
{
    return (ASPECT_RATIO)d->aspectRatio->itemData(d->aspectRatio->currentIndex()).toInt();
}

int SlideShowSettingsWidget::getFrameHeight() const
{
    return d->frameHeight->value();
}

int SlideShowSettingsWidget::getFrameWidth() const
{
    return d->frameWidth->value();
}

QString SlideShowSettingsWidget::getAudioFile() const
{
    return d->audioPath;
}

VIDEO_FORMAT SlideShowSettingsWidget::getVideoFormat() const
{
    return (VIDEO_FORMAT)d->videoFormat->itemData(d->videoFormat->currentIndex()).toInt();
}

VIDEO_TYPE SlideShowSettingsWidget::getVideoType() const
{
    return (VIDEO_TYPE)d->videoType->itemData(d->videoType->currentIndex()).toInt();
}

QString SlideShowSettingsWidget::getSaveFile() const
{
    return d->savePath;
}

} // namespace KIPIVideoSlideShowPlugin
