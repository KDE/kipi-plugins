#ifndef TEMPLATESMODEL_H
#define TEMPLATESMODEL_H

#include <QAbstractItemModel>
#include <QMap>
#include <QImage>
#include <QString>
#include <QtSvg/QSvgRenderer>
#include <QPainter>
#include <QFile>

namespace KIPIPhotoLayoutsEditor
{
    class TemplateItem : public QObject
    {
            QString fpath, fname;
            QImage image;

        public:

            TemplateItem(const QString & path, const QString & name) :
                fpath(path),
                fname(name)
            {
                if (fpath.isEmpty())
                    return;
                QSvgRenderer renderer(fpath);
                if (renderer.isValid())
                {
                    image = QImage( renderer.viewBoxF().size().toSize(), QImage::Format_ARGB32 );
                    image.fill(Qt::white);
                    QPainter p(&image);
                    renderer.render(&p);
                    p.end();
                    image = image.scaled(QSize(100, 100), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                }
            }

            QString name() const
            {
                return fname;
            }

            QString path() const
            {
                return fpath;
            }

            QImage icon() const
            {
                return image;
            }
    };

    class TemplatesModel : public QAbstractItemModel
    {
            Q_OBJECT

            QList<TemplateItem*> templates;

        public:

            explicit TemplatesModel(QObject * parent = 0);

            virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
            virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
            virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
            virtual bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
            virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());
            virtual QVariant data(const QModelIndex & index, int role) const;
            virtual QModelIndex parent(const QModelIndex & child) const;

            void addTemplate(const QString & path, const QString & name);

        signals:

        public slots:

    };
}

#endif // TEMPLATESMODEL_H
