#ifndef CAMERASELECTION_H
#define CAMERASELECTION_H

#include <qstring.h>
#include <qstringlist.h>
#include <kdialogbase.h>

class QComboBox;
class QListView;
class QListViewItem;
class QRadioButton;
class QVButtonGroup;
class QLabel;
class QLineEdit;

class CameraSelection : public KDialogBase {
    Q_OBJECT

public:
    CameraSelection( QWidget* parent = 0 );
    ~CameraSelection();
    void setCamera(const QString& model, const QString& port);
    QString currentModel();
    QString currentPortPath();

private:
    void getCameraList();
    void getSerialPortList();
    QListView*     listView_;
    QVButtonGroup* portButtonGroup_;
    QRadioButton*  usbButton_;
    QRadioButton*  serialButton_;
    QLabel*        portPathLabel_;
    QComboBox*     portPathComboBox_;
    QStringList    serialPortList_;

private slots:
    void slotSelectionChanged(QListViewItem *item);
    void slotPortChanged();
    void slotOkClicked();
    
signals:
    void signalOkClicked(const QString& model, const QString& port);
};

#endif 
