// Qt includes.

#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qradiobutton.h>
#include <qvbuttongroup.h>
#include <qlayout.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qlineedit.h>
#include <qpushbutton.h>

// KDE includes.

#include <klocale.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>

// Local includes.

#include "cameraselection.h"
#include "gpiface.h"

CameraSelection::CameraSelection(QWidget* parent) 
               : KDialogBase(parent, 0, true, i18n("Camera Selection"),
                             Help|Ok|Cancel, Ok, true) 
{
    // About data and help button.
    
    KAboutData* about = new KAboutData("kipiplugins",
                                       I18N_NOOP("KameraKlient"), 
                                       "0.1.0-cvs",
                                       I18N_NOOP("An Digital camera interface Kipi plugin"),
                                       KAboutData::License_GPL,
                                       "(c) 2003-2004, Renchi Raju\n"
                                       "(c) 2004, Tudor Calin", 
                                       0,
                                       "http://extragear.kde.org/apps/kipi.php");
    
    about->addAuthor("Renchi Raju", I18N_NOOP("Original author from Digikam project"),
                     "renchi@pooh.tam.uiuc.edu");

    about->addAuthor("Tudor Calin", I18N_NOOP("Porting the Digikam GPhoto2 interface to Kipi. Maintainer"),
                     "tudor@1xtech.com");

    helpButton_ = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("KameraKlient handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    helpButton_->setPopup( helpMenu->menu() );
    
    QWidget *page = new QWidget(this);
    setMainWidget(page);
    
    QVBoxLayout *topLayout = new QVBoxLayout(page, 5, 5); 

    QGroupBox* mainBox = new QGroupBox(page);
    mainBox->setTitle(i18n("Camera Configuration"));
    mainBox->setColumnLayout(0, Qt::Vertical );
    mainBox->layout()->setSpacing( 5 );
    mainBox->layout()->setMargin( 5 );
    QGridLayout* mainBoxLayout = new QGridLayout(mainBox->layout());
    mainBoxLayout->setAlignment(Qt::AlignTop);

    listView_ = new QListView( mainBox );
    listView_->addColumn( i18n("Cameras") );
    listView_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    mainBoxLayout->addMultiCellWidget(listView_, 0, 4, 0, 0);

    portButtonGroup_ = new QVButtonGroup(mainBox);
    portButtonGroup_->setTitle(i18n("Camera Port Type"));
    portButtonGroup_->setRadioButtonExclusive(true);
    portButtonGroup_->layout()->setSpacing(5);
    portButtonGroup_->layout()->setMargin(5);

    usbButton_ = new QRadioButton(portButtonGroup_);
    usbButton_->setText( i18n("USB"));

    serialButton_ = new QRadioButton( portButtonGroup_ );
    serialButton_->setText( i18n("Serial"));

    mainBoxLayout->addWidget(portButtonGroup_, 1, 1);

    QGroupBox* portPathBox = new QGroupBox(mainBox);
    portPathBox->setTitle( i18n("Camera Port Path"));
    portPathBox->setColumnLayout(0, Qt::Vertical );
    portPathBox->layout()->setSpacing( 5 );
    portPathBox->layout()->setMargin( 5 );
    QVBoxLayout* portPathBoxLayout = new QVBoxLayout( portPathBox->layout() );
    portPathBoxLayout->setAlignment( Qt::AlignTop );

    QLabel* portPathLabel_ = new QLabel( portPathBox);
    portPathLabel_->setText( i18n("only for serial port\n" "cameras"));
    portPathBoxLayout->addWidget( portPathLabel_ );

    portPathComboBox_ = new QComboBox( false, portPathBox );
    portPathComboBox_->setDuplicatesEnabled( FALSE );
    portPathBoxLayout->addWidget(portPathComboBox_);

    mainBoxLayout->addWidget(portPathBox, 2, 1);

    QSpacerItem* spacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mainBoxLayout->addItem(spacer, 4, 1);

    topLayout->addWidget( mainBox );
    
    // Connections --------------------------------------------------
    
    connect(listView_, SIGNAL(selectionChanged(QListViewItem *)), 
            this, SLOT(slotSelectionChanged(QListViewItem *)));
            
    connect(portButtonGroup_, SIGNAL(clicked(int)), this, SLOT(slotPortChanged()));
    
    connect(this, SIGNAL(okClicked()), this, SLOT(slotOkClicked()));
    
    // Initialize  --------------------------------------------------
    
    getCameraList();
    getSerialPortList();
}

CameraSelection::~CameraSelection() {
}

void CameraSelection::slotHelp()
{
    KApplication::kApplication()->invokeHelp("kameraklient",
                                             "kipi-plugins");
} 

void CameraSelection::setCamera(const QString& model, const QString& port) {
    QString camModel(model);
    QListViewItem* item = listView_->findItem(camModel, 0);
    if (!item) {
	return;
    }
    listView_->setSelected(item, true);
    listView_->ensureItemVisible(item);
    
    if (port.contains("usb")) {
        usbButton_->setChecked(true);
    } else if (port.contains("serial")) {
        serialButton_->setChecked(true);
        for (int i=0; i<portPathComboBox_->count(); i++) {
            if (port == portPathComboBox_->text(i)) {
                portPathComboBox_->setCurrentItem(i);
                break;
            }
        }
    }
}

void CameraSelection::getCameraList() {
    int count = 0;
    QStringList clist;
    GPIface::getSupportedCameras(count, clist);
    QString cname;
    for (int i=0; i<count; i++) {
        cname = clist[i];
	new QListViewItem(listView_, cname);
    }
}

void CameraSelection::getSerialPortList() {
    QStringList plist;
    GPIface::getSupportedPorts(plist);
    serialPortList_.clear();
    for (unsigned int i=0; i<plist.count(); i++) {
        if ((plist[i]).startsWith("serial:")) {
            serialPortList_.append(plist[i]);
	}
    }
}

void CameraSelection::slotSelectionChanged(QListViewItem *item) {
    if (!item) {
	return;
    }
    QString model(item->text(0));
    QStringList plist;
    GPIface::getCameraSupportedPorts(model, plist);
    if (plist.contains("serial")) {
        serialButton_->setEnabled(true);
        serialButton_->setChecked(true);
    } else {
        serialButton_->setEnabled(true);
        serialButton_->setChecked(false);
        serialButton_->setEnabled(false);
    }
    if (plist.contains("usb")) {
        usbButton_->setEnabled(true);
        usbButton_->setChecked(true);
    } else {
        usbButton_->setEnabled(true);
        usbButton_->setChecked(false);
        usbButton_->setEnabled(false);
    }
    slotPortChanged();
}

void CameraSelection::slotPortChanged() {
    if (usbButton_->isChecked()) {
        portPathComboBox_->setEnabled(true);
        portPathComboBox_->clear();
        portPathComboBox_->insertItem( QString("usb:"), 0 );
        portPathComboBox_->setEnabled(false);
        return;
    }
    if (serialButton_->isChecked()) {
        portPathComboBox_->setEnabled(true);
        portPathComboBox_->clear();
        portPathComboBox_->insertStringList(serialPortList_);
    }
}

QString CameraSelection::currentModel() {
    QListViewItem* item = listView_->currentItem();
    if(!item) {
        return QString::null;
    }
    QString model(item->text(0));
    return model;
        
}

QString CameraSelection::currentPortPath() {
    return portPathComboBox_->currentText();
}

void CameraSelection::slotOkClicked() {
    emit signalOkClicked(currentModel(), currentPortPath());
}


#include "cameraselection.moc"
