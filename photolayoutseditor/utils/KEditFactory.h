#ifndef QTSLIDEREDITFACTORY_H
#define QTSLIDEREDITFACTORY_H

#if QT_VERSION >= 0x040400
QT_BEGIN_NAMESPACE
#endif

#include <qteditorfactory.h>
#include <qtvariantproperty.h>
#include <QMap>
#include <QList>
#include <QtProperty>


class KSpinBoxFactory : public QtSpinBoxFactory
{
    Q_OBJECT
public:
    KSpinBoxFactory(QObject * parent = 0);
protected:
    QWidget * createEditor(QtIntPropertyManager * manager, QtProperty * property, QWidget * parent);
};

class KSliderFactory : public QtSliderFactory
{
    Q_OBJECT
public:
    KSliderFactory(QObject *parent = 0);
};

class KScrollBarFactory : public QtScrollBarFactory
{
    Q_OBJECT
public:
    KScrollBarFactory(QObject *parent = 0);
};

class KCheckBoxFactory : public QtCheckBoxFactory
{
    Q_OBJECT
public:
    KCheckBoxFactory(QObject *parent = 0);
};

class KDoubleSpinBoxFactory : public QtDoubleSpinBoxFactory
{
    Q_OBJECT
public:
    KDoubleSpinBoxFactory(QObject *parent = 0);
};

class KLineEditFactory : public QtLineEditFactory
{
    Q_OBJECT
public:
    KLineEditFactory(QObject *parent = 0);
};

class KDateEditFactory : public QtDateEditFactory
{
    Q_OBJECT
public:
    KDateEditFactory(QObject *parent = 0);
};

class KTimeEditFactory : public QtTimeEditFactory
{
    Q_OBJECT
public:
    KTimeEditFactory(QObject *parent = 0);
};

class KDateTimeEditFactory : public QtDateTimeEditFactory
{
    Q_OBJECT
public:
    KDateTimeEditFactory(QObject *parent = 0);
};

class KKeySequenceEditorFactory : public QtKeySequenceEditorFactory
{
    Q_OBJECT
public:
    KKeySequenceEditorFactory(QObject *parent = 0);
};

class KCharEditorFactory : public QtCharEditorFactory
{
    Q_OBJECT
public:
    KCharEditorFactory(QObject *parent = 0);
};

class KEnumEditorFactory : public QtEnumEditorFactory
{
    Q_OBJECT
public:
    KEnumEditorFactory(QObject *parent = 0);
};

class KCursorEditorFactory : public QtCursorEditorFactory
{
    Q_OBJECT
public:
    KCursorEditorFactory(QObject *parent = 0);
};

class KColorEditorFactory : public QtColorEditorFactory
{
    Q_OBJECT
public:
    KColorEditorFactory(QObject *parent = 0);
protected:
    QWidget * createEditor(QtColorPropertyManager * manager, QtProperty * property, QWidget * parent);
};

class KFontEditorFactory : public QtFontEditorFactory
{
    Q_OBJECT
public:
    KFontEditorFactory(QObject *parent = 0);
};

class KVariantEditorFactory : public QtVariantEditorFactory
{
    Q_OBJECT
public:
    KVariantEditorFactory(QObject *parent = 0);
protected:
    QWidget * createEditor(QtVariantPropertyManager *manager, QtProperty *property, QWidget *parent);
};

class KSliderEditFactory : public QtAbstractEditorFactory<QtIntPropertyManager>
{
        Q_OBJECT

    public:

        explicit KSliderEditFactory(QObject * parent = 0);

    protected:

        virtual void connectPropertyManager(QtIntPropertyManager * manager);
        virtual QWidget * createEditor(QtIntPropertyManager * manager, QtProperty * property, QWidget * parent);
        virtual void disconnectPropertyManager(QtIntPropertyManager *manager);

    private slots:

        void slotEditorDestroyed(QObject *object);

    private:

        QtSliderFactory * originalFactory;
        QMap<QtProperty *, QList<QWidget *> > createdEditors;
        QMap<QWidget *, QtProperty *> editorToProperty;
};

#if QT_VERSION >= 0x040400
QT_END_NAMESPACE
#endif

#endif // QTSLIDEREDITFACTORY_H
