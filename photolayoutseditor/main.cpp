#include "photolayoutseditor.h"
#include "PLEAboutData.h"

#include <QDesktopWidget>
#include <QResource>
#include <qmath.h>
#include <QString>

#include <klocalizedstring.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

using namespace KIPIPhotoLayoutsEditor;

int main(int argc, char *argv[])
{

    PLEAboutData aboutData;

    KCmdLineArgs::init(argc,argv,&aboutData);
    KCmdLineOptions options;
    options.add("+file", ki18n("Input file"));
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication a;

    PhotoLayoutsEditor * w = PhotoLayoutsEditor::instance(0);
    w->setAttribute(Qt::WA_DeleteOnClose, true);

    KCmdLineArgs * args = KCmdLineArgs::parsedArgs();
    if (args->count())
    {
        KUrl url = args->url(0);
        if (url.isValid())
            w->open(url);
    }

    w->show();

    int result = a.exec();

    return result;
}
