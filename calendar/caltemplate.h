
#ifndef CALTEMPLATE_H
#define CALTEMPLATE_H

#include <qwidget.h>

class QFrame;
class QComboBox;
class QVButtonGroup;
class QRadioButton;
class QCheckBox;
class QSlider;
class QSpinBox;
class QLabel;
class QTimer;

namespace DKCalendar
{

class CalWidget;

class CalTemplate : public QWidget
{
    Q_OBJECT

public:
    
    CalTemplate(QWidget* parent, const char* name=0);
    ~CalTemplate();

private:
    
    QComboBox*     comboPaperSize_;
    QVButtonGroup* btnGroupImagePos_;
    QCheckBox*     checkBoxDrawLines_;
    QSlider*       sliderSpacing_;
    QSlider*       sliderRatio_;
    QComboBox*     comboFont_;
    
    CalWidget*    calWidget_;
    QTimer*       timer_;
    int           previewSize_;

private slots:

    void slotParamsChanged();
    void slotUpdatePreview();
    
};

}

#endif // CALTEMPLATE_H
