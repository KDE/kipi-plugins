#include "GridSetupDialog.h"
#include "PLEConfigSkeleton.h"

#include <QFormLayout>

#include <klocalizedstring.h>

using namespace KIPIPhotoLayoutsEditor;

GridSetupDialog::GridSetupDialog(QWidget * parent) :
    KDialog(parent)
{
    PLEConfigSkeleton * skeleton = PLEConfigSkeleton::self();

    this->setCaption(i18n("Setup grid lines"));
    this->setModal(true);

    centralWidget = new QWidget(this);
    setMainWidget(centralWidget);

    QFormLayout * layout = new QFormLayout();
    layout->setSizeConstraint( QLayout::SetFixedSize );

    x = new QDoubleSpinBox(centralWidget);
    KConfigSkeletonItem * hgi = skeleton->findItem("horizontalGrid");
    if (hgi)
    {
        x->setMinimum(hgi->minValue().toDouble());
        x->setMaximum(hgi->maxValue().toDouble());
    }
    x->setSingleStep(1.0);
    x->setValue(PLEConfigSkeleton::horizontalGrid());
    connect(skeleton, SIGNAL(horizontalGridChanged(double)), x, SLOT(setValue(double)));
    layout->addRow(i18n("Horizontal distance"), x);

    y = new QDoubleSpinBox(centralWidget);
    KConfigSkeletonItem * vgi = skeleton->findItem("verticalGrid");
    if (vgi)
    {
        y->setMinimum(hgi->minValue().toDouble());
        y->setMaximum(hgi->maxValue().toDouble());
    }
    y->setSingleStep(1.0);
    y->setValue(PLEConfigSkeleton::verticalGrid());
    connect(skeleton, SIGNAL(verticalGridChanged(double)), y, SLOT(setValue(double)));
    layout->addRow(i18n("Vertical distance"), y);

    centralWidget->setLayout(layout);

    this->setFixedSize( this->sizeHint() );
}

void GridSetupDialog::setHorizontalDistance(qreal value)
{
    x->setValue(value);
}

void GridSetupDialog::setVerticalDistance(qreal value)
{
    y->setValue(value);
}

qreal GridSetupDialog::horizontalDistance() const
{
    return x->value();
}

qreal GridSetupDialog::verticalDistance() const
{
    return y->value();
}

int GridSetupDialog::exec()
{
    int result = KDialog::exec();
    if (result == Accepted)
    {
        PLEConfigSkeleton::setHorizontalGrid( this->horizontalDistance() );
        PLEConfigSkeleton::setVerticalGrid( this->verticalDistance() );
        PLEConfigSkeleton::self()->writeConfig();
    }
    return result;
}
