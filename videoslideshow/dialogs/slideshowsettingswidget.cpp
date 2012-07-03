/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2012-07-01
 * @brief  Settings Widget
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

#include "slideshowsettingswidget.moc"

// Qt includes

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QString>
#include <QSpacerItem>
#include <QDir>

// kde includes 

#include <kpushbutton.h>
#include <klocalizeddate.h>
#include <kfiledialog.h>
#include <kurl.h>
#include <kseparator.h>
#include <klocalizedstring.h>
#include <sys/socket.h>

#include <kmessagebox.h>

namespace KIPIVideoSlideShowPlugin
{

class SlideShowSettingsWidget::SlideShowPriv
{
public:

    SlideShowPriv()
    {
        PPMSettingsWidget   = 0;
        VideoSettingsWidget = 0;
        selectBtn           = 0;
        tempDirLabel        = 0;
        timeVal             = 0;
        effects             = 0;
        transitions         = 0;
        transitionSpeed     = 0;
        asptRatioCorrct     = 0;
        frameHeight         = 0;
        frameWidth          = 0;
    }

    QWidget*     PPMSettingsWidget;
    QWidget*     VideoSettingsWidget;

    KPushButton* selectBtn;

    QString      path;

    QLabel*      tempDirLabel;
    QSpinBox *   timeVal;
    QComboBox*   effects;
    QComboBox*   transitions;
    QComboBox*   transitionSpeed;
    QComboBox*   asptRatioCorrct;
    QSpinBox*    frameWidth;
    QSpinBox*    frameHeight;
};

SlideShowSettingsWidget::SlideShowSettingsWidget(QWidget* const parent, Qt::WFlags flags)
    : KTabWidget(parent, flags), d(new SlideShowPriv)
{
    d->PPMSettingsWidget   = new QWidget();
    d->VideoSettingsWidget = new QWidget();
    addTab(d->PPMSettingsWidget,   QString("SlideShow Settings"));
    addTab(d->VideoSettingsWidget, QString("Video Settings"));

    setCloseButtonEnabled(false);
    setHoverCloseButton(false);

    setUpPPMSettings();

    connect(d->timeVal,         SIGNAL(valueChanged(int)),        this, SLOT(timeValueChanged(int)));
    connect(d->effects,         SIGNAL(currentIndexChanged(int)), this, SLOT(effectIndexChanged(int)));
    connect(d->transitions,     SIGNAL(currentIndexChanged(int)), this, SLOT(transIndexChanged(int)));
    connect(d->transitionSpeed, SIGNAL(currentIndexChanged(int)), this, SLOT(transSpeedIndexChanged(int)));
}

SlideShowSettingsWidget::~SlideShowSettingsWidget()
{
    delete d;
}

void SlideShowSettingsWidget::effectIndexChanged(int index)
{
    emit DataChanged(d->effects->itemText(index), MyImageList::SPECIALEFFECT);
}

void SlideShowSettingsWidget::transIndexChanged(int index)
{
    emit DataChanged(d->transitions->itemText(index), MyImageList::TRANSITION);
}

void SlideShowSettingsWidget::transSpeedIndexChanged(int index)
{
    emit DataChanged(d->transitionSpeed->itemText(index), MyImageList::TRANSSPEED);
}

void SlideShowSettingsWidget::timeValueChanged(int time)
{
    emit DataChanged(QString::number(time), MyImageList::TIME);
}

void SlideShowSettingsWidget::setUpPPMSettings()
{
    QGridLayout *mainLayout = new QGridLayout(d->PPMSettingsWidget);

    QLabel *timeLabel       = new QLabel("Image Display Time");
    QLabel *effectLabel     = new QLabel("Special Effect");
    QLabel *transitionLabel = new QLabel("Transition");
    QLabel *transSpeedLabel = new QLabel("Transition Speed");

    d->timeVal      = new QSpinBox();
    d->timeVal->setRange(1,60);

    d->effects      = new QComboBox();
    d->effects->setEditable(false);
    d->effects->addItem(i18n("None"), i18n("None"));
    d->effects->addItem(i18n("Ken-Burn Effect"), i18n("Ken-Burn Effect"));
    d->effects->addItem(i18n("Scroll Effect"), i18n("Scroll Effect"));

    d->transitions  = new QComboBox();
    d->transitions->setEditable(false);
    d->transitions->addItem(i18n("None"), i18n("None"));
    d->transitions->addItem(i18n("Random"), i18n("Random"));
    d->transitions->addItem(i18n("Fade"), i18n("Fade"));
    d->transitions->addItem(i18n("Slide Left to Right"), i18n("Slide Left to Right"));
    d->transitions->addItem(i18n("Slide Right to Left"), i18n("Slide Right to Left"));
    d->transitions->addItem(i18n("Slide Top to Bottom"), i18n("Slide Top to Bottom"));
    d->transitions->addItem(i18n("Slide Bottom to Top"), i18n("Slide Bottom to Top"));
    d->transitions->addItem(i18n("Push Left to Right"), i18n("Push Left to Right"));
    d->transitions->addItem(i18n("Push Right to Left"), i18n("Push Right to Left"));
    d->transitions->addItem(i18n("Push Top to Bottom"), i18n("Push Top to Bottom"));
    d->transitions->addItem(i18n("Push Bottom to Top"), i18n("Push Bottom to Top"));
    d->transitions->addItem(i18n("Swap Left to Right"), i18n("Swap Left to Right"));
    d->transitions->addItem(i18n("Swap Right to Left"), i18n("Swap Right to Left"));
    d->transitions->addItem(i18n("Swap Top to Bottom"), i18n("Swap Top to Bottom"));
    d->transitions->addItem(i18n("Swap Bottom to Top"), i18n("Swap Bottom to Top"));
    d->transitions->addItem(i18n("Roll Left to Right"), i18n("Roll Left to Right"));
    d->transitions->addItem(i18n("Roll Right to Left"), i18n("Roll Right to Left"));
    d->transitions->addItem(i18n("Roll Top to Bottom"), i18n("Roll Top to Bottom"));
    d->transitions->addItem(i18n("Roll Bottom to Top"), i18n("Roll Bottom to Top"));

    d->transitionSpeed  = new QComboBox();
    d->transitionSpeed->setEditable(false);
    d->transitionSpeed->addItem(i18n("Slow"), i18n("Slow"));
    d->transitionSpeed->addItem(i18n("Medium"), i18n("Medium"));
    d->transitionSpeed->addItem(i18n("Fast"), i18n("Fast"));

    mainLayout->addWidget(timeLabel,          0, 0, 1, 1);
    mainLayout->addWidget(d->timeVal,         0, 1, 1, 1);
    mainLayout->addWidget(effectLabel,        1, 0, 1, 1);
    mainLayout->addWidget(d->effects,         1, 1, 1, 1);
    mainLayout->addWidget(transitionLabel,    2, 0, 1, 1);
    mainLayout->addWidget(d->transitions,     2, 1, 1, 1); 
    mainLayout->addWidget(transSpeedLabel,    3, 0, 1, 1);
    mainLayout->addWidget(d->transitionSpeed, 3, 1, 1, 1);

    mainLayout->addWidget(new KSeparator(Qt::Horizontal), 4, 0, 1, 2);

    d->tempDirLabel           = new QLabel(i18n("Temporary Directory"));
    QLabel *asptRatioCorLabel = new QLabel("Aspect Ratio Correction");
    QLabel *heightLabel       = new QLabel("Frame Height");
    QLabel *widthLabel        = new QLabel("Frame Width");

    d->selectBtn       = new KPushButton("Browse");
    connect(d->selectBtn, SIGNAL(clicked(bool)), this, SLOT(slotSelectTempDirectory()));

    d->asptRatioCorrct = new QComboBox();
    d->asptRatioCorrct->setEditable(false);
    d->asptRatioCorrct->addItem(i18n("Auto"), i18n("Auto"));
    d->asptRatioCorrct->addItem(i18n("None"), i18n("None"));
    d->asptRatioCorrct->addItem(i18n("Fill In"), i18n("Fill In"));
    d->asptRatioCorrct->addItem(i18n("Fit In"), i18n("Fit In"));

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

    mainLayout->addItem(new QSpacerItem(1,1), 10, 0, 1, 2);
    mainLayout->setRowStretch(10, 10);
}

void SlideShowSettingsWidget::resetToDefault()
{
    d->timeVal->setValue(25);
    d->effects->setCurrentIndex(d->effects->findData(i18n("None")));
    d->transitions->setCurrentIndex(d->transitions->findData(i18n("Random")));
    d->transitionSpeed->setCurrentIndex(d->transitionSpeed->findData(i18n("Medium")));
    d->asptRatioCorrct->setCurrentIndex(d->asptRatioCorrct->findData(i18n("None")));
    d->frameWidth->setValue(500);
    d->frameHeight->setValue(500);
}

void SlideShowSettingsWidget::setUpVideoSettings()
{
}

void SlideShowSettingsWidget::slotSelectTempDirectory()
{
    QString path = KFileDialog::getExistingDirectory(KUrl(), this,
                                                     i18n("Select temporary directory"));

    if (!path.isEmpty())
    {
        d->path = path;
        d->tempDirLabel->setText(path);
    }
}

QString SlideShowSettingsWidget::getTempDirPath()
{
    return d->path;
}

void SlideShowSettingsWidget::setTempDirPath(QString& path)
{
    if(KUrl(path).isValid())
        d->path = path;
    else
        d->path = QDir::tempPath();
}

void SlideShowSettingsWidget::updateData(int time, const QString& transition, const QString& transSpeed, const QString& effect)
{
    d->timeVal->setValue(time);
    d->effects->setCurrentIndex(d->effects->findData(i18n(effect.toUtf8())));
    d->transitions->setCurrentIndex(d->transitions->findData(i18n(transition.toUtf8())));
    d->transitionSpeed->setCurrentIndex(d->transitionSpeed->findData(i18n(transSpeed.toUtf8())));
}

} // namespace KIPIVideoSlideShowPlugin
