#ifndef TIMEADJUSTDIALOG_H
#define TIMEADJUSTDIALOG_H

#include <kdialogbase.h>
#include <kurl.h>
#include <libkipi/interface.h>
class QSpinBox;
class QRadioButton;

class TimeAdjustDialog :public KDialogBase {
    Q_OBJECT

public:
    TimeAdjustDialog( KIPI::Interface* interface, QWidget* parent, const char* name = 0 );
    void setImages( const KURL::List& images );

protected slots:
    void updateExample();
    void slotOK();

protected:
    void addInfoPage();
    void addConfigPage();
    void addAboutPage();
    QDateTime updateTime( QDateTime time ) const;

private:
    KIPI::Interface* m_interface;
    KURL::List m_images;
    QRadioButton* m_add;
    QLabel* m_infoLabel;
    QSpinBox* m_secs;
    QSpinBox* m_minutes;
    QSpinBox* m_hours;
    QSpinBox* m_days;
    QSpinBox* m_months;
    QSpinBox* m_years;
    QLabel* m_exampleAdj;
    QDateTime m_exampleDate;

};


#endif /* TIMEADJUSTDIALOG_H */

