#ifndef KCLICKABLEIMAGELABEL_H
#define KCLICKABLEIMAGELABEL_H

#include <QLabel>
#include <QUrl>

namespace KIPIDebianScreenshotsPlugin {

class KClickableImageLabel : public QLabel
{
    Q_OBJECT
public:
    explicit KClickableImageLabel(QWidget* parent = 0, Qt::WindowFlags f = 0);
    explicit KClickableImageLabel(const QString& text, QWidget* parent = 0, Qt::WindowFlags f = 0);
    void setUrl(const QUrl& url);
    QUrl url() const;

protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);

signals:

public slots:

private:
    QUrl m_url;
};

} // KIPIDebianScreenshotsPlugin

#endif // KCLICKABLEIMAGELABEL_H
