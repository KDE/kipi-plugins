#include <klocale.h>

#include <qvariant.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qbuttongroup.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qslider.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qfontdatabase.h>
#include <qpaintdevicemetrics.h>

#include <qpainter.h>
#include <qprinter.h>

#include "caltemplate.h"
#include "calwidget.h"
#include "calsettings.h"
#include "calpainter.h"

namespace DKCalendar
{

CalTemplate::CalTemplate(QWidget* parent, const char* name)
    : QWidget(parent, name)
{
    QGridLayout *mainLayout = new QGridLayout(this, 1, 1, 5, 5);

    // ---------------------------------------------------------------

    previewSize_ = 300;
    
    QGroupBox *boxPreview_ = new QGroupBox( i18n("Preview"), this );
    boxPreview_->setColumnLayout(0, Qt::Vertical);
    boxPreview_->layout()->setMargin( 5 );

    QVBoxLayout *previewLayout = new QVBoxLayout(boxPreview_->layout());
    calWidget_ = new CalWidget(boxPreview_);
    previewLayout->addWidget(calWidget_, 0, Qt::AlignCenter);
    
    mainLayout->addWidget( boxPreview_, 0, 0 );

    // ---------------------------------------------------------------

    QGroupBox *gbox = new QGroupBox( this );
    gbox->setColumnLayout(0, Qt::Vertical );
    gbox->layout()->setSpacing( 11 );
    gbox->layout()->setMargin( 6 );
    QVBoxLayout* gboxLayout = new QVBoxLayout( gbox->layout() );

    // ---------------------------------------------------------------

    QHBoxLayout *hlayout = new QHBoxLayout( 0, 0, 5 ); 
    hlayout->addWidget( new QLabel(i18n("Paper Size"), gbox) );

    comboPaperSize_ = new QComboBox(false, gbox);
    hlayout->addWidget(comboPaperSize_);
    gboxLayout->addLayout( hlayout );

    QStringList paperSizes;
    paperSizes << "A4";
    paperSizes << "US Letter";
    comboPaperSize_->insertStringList(paperSizes);

    connect(comboPaperSize_, SIGNAL(activated(int)),
            SLOT(slotParamsChanged()));
    
    // ---------------------------------------------------------------
    
    btnGroupImagePos_ = new QVButtonGroup(i18n("Image Position"), gbox);
    btnGroupImagePos_->setRadioButtonExclusive(true);

    QRadioButton* radioBtn = new QRadioButton(i18n("Top"), btnGroupImagePos_);
    radioBtn->setChecked(true);

    radioBtn = new QRadioButton(i18n("Left"), btnGroupImagePos_);
    radioBtn = new QRadioButton(i18n("Right"), btnGroupImagePos_);

    gboxLayout->addWidget( btnGroupImagePos_ );

    connect(btnGroupImagePos_, SIGNAL(clicked(int)),
            SLOT(slotParamsChanged()));
    
    // ---------------------------------------------------------------

    checkBoxDrawLines_ = new QCheckBox(i18n("Draw lines in Calendar"), gbox);
    gboxLayout->addWidget( checkBoxDrawLines_ );

    connect(checkBoxDrawLines_, SIGNAL(toggled(bool)),
            SLOT(slotParamsChanged()));

    // ---------------------------------------------------------------

    hlayout = new QHBoxLayout( 0, 0, 5 );

    hlayout->addWidget(new QLabel(i18n("Image to Text Ratio :"), gbox));

    sliderRatio_ = new QSlider(50,300,5,100,Qt::Horizontal,gbox);
    hlayout->addWidget( sliderRatio_ );

    gboxLayout->addLayout( hlayout );

    connect(sliderRatio_, SIGNAL(valueChanged(int)),
            SLOT(slotParamsChanged()));
    
    // ---------------------------------------------------------------

    hlayout = new QHBoxLayout( 0, 0, 5 );

    hlayout->addWidget(new QLabel(i18n("Font:"), gbox));

    comboFont_ = new QComboBox(false, gbox);
    hlayout->addWidget( comboFont_ );

    QFontDatabase fontDB;
    comboFont_->insertStringList(fontDB.families());
    

    gboxLayout->addLayout( hlayout );

    connect(comboFont_, SIGNAL(activated(int)),
            SLOT(slotParamsChanged()));
    

    // ---------------------------------------------------------------

    gboxLayout->addItem(new QSpacerItem(5,10,QSizePolicy::Minimum,
                                        QSizePolicy::Expanding));
    
    mainLayout->addWidget( gbox, 0, 1 );


    // ---------------------------------------------------------------

    QFrame* hline = new QFrame( this );
    hline->setFrameShape( QFrame::HLine );
    hline->setFrameShadow( QFrame::Sunken );
    
    mainLayout->addMultiCellWidget( hline, 1, 1, 0, 1 );

    // ---------------------------------------------------------------

    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()),
            SLOT(slotUpdatePreview()));
    timer_->start(0,true);
}

CalTemplate::~CalTemplate()
{
    delete timer_;
}


void CalTemplate::slotParamsChanged()
{
    timer_->start(10, true);
}

void CalTemplate::slotUpdatePreview()
{
    if (timer_->isActive()) return;

    CalParams& params = CalSettings::instance()->calParams;
    
    QString paperSize = comboPaperSize_->currentText();
    if (paperSize == "A4") {
        params.paperWidth  = 210;
        params.paperHeight = 297;
        params.pageSize    = KPrinter::A4;
    }
    else if (paperSize == "US Letter") {
        params.paperWidth  = 216;
        params.paperHeight = 279;
        params.pageSize    = KPrinter::Letter;
    }
    
    QString imgPos =
        btnGroupImagePos_->selected()->text();
    
    if (imgPos == i18n("Top")) {
        params.imgPos = CalParams::Top;

        float zoom = QMIN((float)previewSize_/params.paperWidth,
                          (float)previewSize_/params.paperHeight);
        params.width  = (int)(params.paperWidth  * zoom);
        params.height = (int)(params.paperHeight * zoom);

    }
    else if (imgPos == i18n("Left")) {
        params.imgPos = CalParams::Left;

        float zoom = QMIN((float)previewSize_/params.paperWidth,
                          (float)previewSize_/params.paperHeight);
        params.width  = (int)(params.paperHeight  * zoom);
        params.height = (int)(params.paperWidth   * zoom);

    }
    else {
        params.imgPos = CalParams::Right;

        float zoom = QMIN((float)previewSize_/params.paperWidth,
                          (float)previewSize_/params.paperHeight);
        params.width  = (int)(params.paperHeight  * zoom);
        params.height = (int)(params.paperWidth   * zoom);
    }
    
    params.drawLines = checkBoxDrawLines_->isChecked();
    params.ratio     = sliderRatio_->value();
    params.baseFont  = QFont(comboFont_->currentText());
    calWidget_->recreate();    
}


}
