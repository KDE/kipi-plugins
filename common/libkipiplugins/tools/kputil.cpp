/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2014-09-12
 * Description : Simple helpher widgets collection
 *
 * Copyright (C) 2015      by Alexander Potashev <aspotashev at gmail dot com>
 * Copyright (C) 2014-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "kputil.h"

// C ANSI includes

extern "C"
{
#include <unistd.h>
}

// Qt includes

#include <QWidget>
#include <QByteArray>
#include <QBuffer>
#include <QImage>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QPushButton>
#include <QFileInfo>
#include <QPainter>
#include <QStandardPaths>
#include <QVector>
#include <QColorDialog>
#include <QStyleOptionButton>
#include <qdrawutil.h>
#include <QTime>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "kipiplugins_debug.h"

namespace KIPIPlugins
{

QDir makeTemporaryDir(const char* prefix)
{
    QString subDir = QString::fromLatin1("%1-%2").arg(QString::fromUtf8(prefix)).arg(getpid());
    QString path   = QDir(QDir::tempPath()).filePath(subDir);

    QDir().mkpath(path);

    return QDir(path);
}

// ------------------------------------------------------------------------------------

KPHBox::KPHBox(QWidget* const parent)
    : QFrame(parent)
{
    QHBoxLayout* const layout = new QHBoxLayout(this);
    layout->setContentsMargins(QMargins());
    layout->setSpacing(0);
    setLayout(layout);
}

KPHBox::KPHBox(bool /*vertical*/, QWidget* const parent)
    : QFrame(parent)
{
    QVBoxLayout* const layout = new QVBoxLayout(this);
    layout->setContentsMargins(QMargins());
    layout->setSpacing(0);
    setLayout(layout);
}

KPHBox::~KPHBox()
{
}

void KPHBox::childEvent(QChildEvent* e)
{
    switch (e->type())
    {
        case QEvent::ChildAdded:
        {
            QChildEvent* const ce = static_cast<QChildEvent*>(e);

            if (ce->child()->isWidgetType())
            {
                QWidget* const w = static_cast<QWidget*>(ce->child());
                static_cast<QBoxLayout*>(layout())->addWidget(w);
            }

            break;
        }

        case QEvent::ChildRemoved:
        {
            QChildEvent* const ce = static_cast<QChildEvent*>(e);

            if (ce->child()->isWidgetType())
            {
                QWidget* const w = static_cast<QWidget*>(ce->child());
                static_cast<QBoxLayout*>(layout())->removeWidget(w);
            }

            break;
        }

        default:
            break;
    }

    QFrame::childEvent(e);
}

QSize KPHBox::sizeHint() const
{
    KPHBox* const b = const_cast<KPHBox*>(this);
    QApplication::sendPostedEvents(b, QEvent::ChildAdded);

    return QFrame::sizeHint();
}

QSize KPHBox::minimumSizeHint() const
{
    KPHBox* const b = const_cast<KPHBox*>(this);
    QApplication::sendPostedEvents(b, QEvent::ChildAdded );

    return QFrame::minimumSizeHint();
}

void KPHBox::setSpacing(int spacing)
{
    layout()->setSpacing(spacing);
}

void KPHBox::setContentsMargins(const QMargins& margins)
{
    layout()->setContentsMargins(margins);
}

void KPHBox::setContentsMargins(int left, int top, int right, int bottom)
{
    layout()->setContentsMargins(left, top, right, bottom);
}

void KPHBox::setStretchFactor(QWidget* const widget, int stretch)
{
    static_cast<QBoxLayout*>(layout())->setStretchFactor(widget, stretch);
}

// ------------------------------------------------------------------------------------

KPVBox::KPVBox(QWidget* const parent)
  : KPHBox(true, parent)
{
}

KPVBox::~KPVBox()
{
}

// ---------------------------------------------------------------------------------------

KPWorkingPixmap::KPWorkingPixmap()
{
    QPixmap pix(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("kipiplugins/pics/process-working.png")));
    QSize   size(22, 22);

    if (pix.isNull())
    {
        qCWarning(KIPIPLUGINS_LOG) << "Invalid pixmap specified.";
        return;
    }

    if (!size.isValid())
    {
        size = QSize(pix.width(), pix.width());
    }

    if (pix.width() % size.width() || pix.height() % size.height())
    {
        qCWarning(KIPIPLUGINS_LOG) << "Invalid framesize.";
        return;
    }

    const int rowCount = pix.height() / size.height();
    const int colCount = pix.width()  / size.width();
    m_frames.resize(rowCount * colCount);

    int pos = 0;

    for (int row = 0; row < rowCount; ++row)
    {
        for (int col = 0; col < colCount; ++col)
        {
            QPixmap frm     = pix.copy(col * size.width(), row * size.height(), size.width(), size.height());
            m_frames[pos++] = frm;
        }
    }
}

KPWorkingPixmap::~KPWorkingPixmap()
{
}

bool KPWorkingPixmap::isEmpty() const
{
    return m_frames.isEmpty();
}

QSize KPWorkingPixmap::frameSize() const
{
    if (isEmpty())
    {
        qCWarning(KIPIPLUGINS_LOG) << "No frame loaded.";
        return QSize();
    }

    return m_frames[0].size();
}

int KPWorkingPixmap::frameCount() const
{
    return m_frames.size();
}

QPixmap KPWorkingPixmap::frameAt(int index) const
{
    if (isEmpty())
    {
        qCWarning(KIPIPLUGINS_LOG) << "No frame loaded.";
        return QPixmap();
    }

    return m_frames.at(index);
}

// ------------------------------------------------------------------------------------

class Q_DECL_HIDDEN KPFileSelector::Private
{
public:

    Private()
    {
        edit      = 0;
        btn       = 0;
        fdMode    = QFileDialog::ExistingFile;
        fdOptions = QFileDialog::DontUseNativeDialog;
    }

    QLineEdit*            edit;
    QPushButton*          btn;

    QFileDialog::FileMode fdMode;
    QString               fdFilter;
    QString               fdTitle;
    QFileDialog::Options  fdOptions;
};

KPFileSelector::KPFileSelector(QWidget* const parent)
    : KPHBox(parent),
      d(new Private)
{
    d->edit    = new QLineEdit(this);
    d->btn     = new QPushButton(i18n("Browse..."), this);
    setStretchFactor(d->edit, 10);

    connect(d->btn, SIGNAL(clicked()),
            this, SLOT(slotBtnClicked()));
}

KPFileSelector::~KPFileSelector()
{
    delete d;
}

QLineEdit* KPFileSelector::lineEdit() const
{
    return d->edit;
}

void KPFileSelector::setFileDlgMode(QFileDialog::FileMode mode)
{
    d->fdMode = mode;
}

void KPFileSelector::setFileDlgFilter(const QString& filter)
{
    d->fdFilter = filter;
}

void KPFileSelector::setFileDlgTitle(const QString& title)
{
    d->fdTitle = title;
}

void KPFileSelector::setFileDlgOptions(QFileDialog::Options opts)
{
    d->fdOptions = opts;
}

void KPFileSelector::slotBtnClicked()
{
    if (d->fdMode == QFileDialog::ExistingFiles)
    {
        qCDebug(KIPIPLUGINS_LOG) << "Multiple selection is not supported";
        return;
    }

    QFileDialog* const fileDlg = new QFileDialog();
    fileDlg->setOptions(d->fdOptions);
    fileDlg->setDirectory(QFileInfo(d->edit->text()).filePath());
    fileDlg->setFileMode(d->fdMode);

    if (!d->fdFilter.isNull())
        fileDlg->setNameFilter(d->fdFilter);

    if (!d->fdTitle.isNull())
        fileDlg->setWindowTitle(d->fdTitle);

    emit signalOpenFileDialog();

    if (fileDlg->exec() == QDialog::Accepted)
    {
        QStringList sel = fileDlg->selectedFiles();

        if (!sel.isEmpty())
        {
            d->edit->setText(sel.first());
            emit signalUrlSelected(QUrl::fromLocalFile(sel.first()));
        }
    }

    delete fileDlg;
}

// ------------------------------------------------------------------------------------

class Q_DECL_HIDDEN KPColorSelector::Private
{
public:

    Private()
    {
    }

    QColor color;
};

KPColorSelector::KPColorSelector(QWidget* const parent)
    : QPushButton(parent),
      d(new Private)
{
    connect(this, SIGNAL(clicked()),
            this, SLOT(slotBtnClicked()));
}

KPColorSelector::~KPColorSelector()
{
    delete d;
}

void KPColorSelector::setColor(const QColor& color)
{
    if (color.isValid())
    {
        d->color = color;
        update();
    }
}

QColor KPColorSelector::color() const
{
    return d->color;
}

void KPColorSelector::slotBtnClicked()
{
    QColor color = QColorDialog::getColor(d->color);

    if (color.isValid())
    {
        setColor(color);
        emit signalColorSelected(color);
    }
}

void KPColorSelector::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QStyle* const style = QWidget::style();

    QStyleOptionButton opt;

    opt.initFrom(this);
    opt.state    |= isDown() ? QStyle::State_Sunken : QStyle::State_Raised;
    opt.features  = QStyleOptionButton::None;
    opt.icon      = QIcon();
    opt.text.clear();

    style->drawControl(QStyle::CE_PushButtonBevel, &opt, &painter, this);

    QRect labelRect = style->subElementRect(QStyle::SE_PushButtonContents, &opt, this);
    int shift       = style->pixelMetric(QStyle::PM_ButtonMargin, &opt, this) / 2;
    labelRect.adjust(shift, shift, -shift, -shift);
    int x, y, w, h;
    labelRect.getRect(&x, &y, &w, &h);

    if (isChecked() || isDown())
    {
        x += style->pixelMetric(QStyle::PM_ButtonShiftHorizontal, &opt, this);
        y += style->pixelMetric(QStyle::PM_ButtonShiftVertical,   &opt, this);
    }

    QColor fillCol = isEnabled() ? d->color : palette().color(backgroundRole());
    qDrawShadePanel(&painter, x, y, w, h, palette(), true, 1, 0);

    if (fillCol.isValid())
    {
        const QRect rect(x + 1, y + 1, w - 2, h - 2);

        if (fillCol.alpha() < 255)
        {
            QPixmap chessboardPattern(16, 16);
            QPainter patternPainter(&chessboardPattern);
            patternPainter.fillRect(0, 0, 8, 8, Qt::black);
            patternPainter.fillRect(8, 8, 8, 8, Qt::black);
            patternPainter.fillRect(0, 8, 8, 8, Qt::white);
            patternPainter.fillRect(8, 0, 8, 8, Qt::white);
            patternPainter.end();
            painter.fillRect(rect, QBrush(chessboardPattern));
        }

        painter.fillRect(rect, fillCol);
    }

    if (hasFocus())
    {
        QRect focusRect = style->subElementRect(QStyle::SE_PushButtonFocusRect, &opt, this);
        QStyleOptionFocusRect focusOpt;
        focusOpt.init(this);
        focusOpt.rect            = focusRect;
        focusOpt.backgroundColor = palette().background().color();
        style->drawPrimitive(QStyle::PE_FrameFocusRect, &focusOpt, &painter, this);
    }
}

// -------------------------------------------------------------------------------------------

KPRandomGenerator::KPRandomGenerator()
{
}

KPRandomGenerator::~KPRandomGenerator()
{
}

QString KPRandomGenerator::randomString(const int& length)
{
   const QString possibleCharacters(
       QString::fromLatin1("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"));

   QString randomString;
   qsrand((uint)QTime::currentTime().msec());

   for(int i=0; i<length; ++i)
   {
       int index = qrand() % possibleCharacters.length();
       QChar nextChar = possibleCharacters.at(index);
       randomString.append(nextChar);
   }

   return randomString;
}

} // namespace KIPIPlugins
