#include "PLEConfigDialog.h"

#include "PLEConfigSkeleton.h"
#include "PLEConfigViewWidget.h"

#include <klocalizedstring.h>

using namespace KIPIPhotoLayoutsEditor;

class KIPIPhotoLayoutsEditor::PLEConfigDialogPrivate
{
    PLEConfigViewWidget * confVWdg;

    friend class PLEConfigDialog;
};

PLEConfigDialog::PLEConfigDialog(QWidget * parent) :
    KConfigDialog(parent, "settings", PLEConfigSkeleton::self()),
    d(new PLEConfigDialogPrivate)
{
    d->confVWdg = new PLEConfigViewWidget( 0, i18n("View") );
    this->addPage( d->confVWdg, i18n("View") )->setIcon(KIcon(QIcon(":/view.png")));
}

void PLEConfigDialog::updateSettings()
{
    d->confVWdg->updateSettings();
}

void PLEConfigDialog::updateWidgets()
{
    d->confVWdg->updateWidgets();
}
