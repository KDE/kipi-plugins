#include "photolayoutseditor.h"

#include <QDesktopWidget>
#include <QResource>
#include <qmath.h>

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

using namespace KIPIPhotoLayoutsEditor;

#include <QtSvg/QSvgGenerator>
#include <QPainter>

int main(int argc, char *argv[])
{

    KAboutData aboutData("photolayoutseditor",
                         0,
                         ki18n("Photo Layouts Editor"),
                         QString("").toUtf8(),
                         ki18n(""),
                         KAboutData::License_GPL,
                         ki18n(""),
                         ki18n(""),
                         QString("").toUtf8());

    KCmdLineArgs::init(argc,argv,&aboutData);

    KApplication a;

    PhotoLayoutsEditor * w = PhotoLayoutsEditor::instance(0);
    w->setAttribute(Qt::WA_DeleteOnClose, true);
    int height = QApplication::desktop()->height()-500;
    w->resize(round(height*16.0/9.0),height);
    QDesktopWidget * d = a.desktop();
    w->move(d->rect().center()-w->frameGeometry().center());
    w->show();

    int result = a.exec();

    return result;
}
