#include "kclickableimagelabel.h"
#include <QDesktopServices>
#include <QRect>
#include <QCursor>
#include <QDebug>

namespace KIPIDebianScreenshotsPlugin {

KClickableImageLabel::KClickableImageLabel(QWidget* parent, Qt::WindowFlags f) :
    QLabel(parent, f), m_url(QUrl())
{
    setMouseTracking(true);
}

KClickableImageLabel::KClickableImageLabel(const QString& text, QWidget* parent, Qt::WindowFlags f) :
    QLabel(text, parent, f), m_url(QUrl())
{
}

void KClickableImageLabel::setUrl(const QUrl& url) {
    m_url = url;
}

QUrl KClickableImageLabel::url() const {
    return m_url;
}

void KClickableImageLabel::mousePressEvent(QMouseEvent* /* ev */) {
    if(m_url.isEmpty()) {
        return;
    }

    QDesktopServices::openUrl(m_url);
}

void KClickableImageLabel::mouseMoveEvent(QMouseEvent* /* ev */) {

    if(pixmap()->isNull()) {
        return;
    }

    if( rect().contains(mapFromGlobal(QCursor::pos())) ) {
        setCursor(Qt::PointingHandCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
}

} // KIPIDebianScreenshotsPlugin
