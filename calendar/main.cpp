#include <qapplication.h>

#include "caltemplate.h"
#include "calwidget.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    DKCalendar::CalTemplate *w =
        new DKCalendar::CalTemplate(0);
//      DKCalendar::CalWidget *w =
//          new DKCalendar::CalWidget(0,216*3,279*3);

    a.setMainWidget(w);    
    w->show();    
    
    return a.exec();

    
}
