#include "photolayoutseditor.h"
#include "PLEAboutData.h"

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

    PLEAboutData aboutData;

    KCmdLineArgs::init(argc,argv,&aboutData);

    KApplication a;

    PhotoLayoutsEditor * w = PhotoLayoutsEditor::instance(0);
    w->setAttribute(Qt::WA_DeleteOnClose, true);
    w->show();

    int result = a.exec();

    return result;
}
