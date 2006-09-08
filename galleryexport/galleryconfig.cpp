/* ============================================================
 * File  : galleryconfig.cpp
 * Author: Colin Guthrie <kde@colin.guthr.ie>
 * Date  : 2006-09-04
 * Copyright 2006 by Colin Guthrie
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Include files for Qt

#include <qlistview.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qlineedit.h>
#include <qprogressdialog.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlayout.h>

// Include files for KDE

#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <krun.h>
#include <kdebug.h>
#include <kconfig.h>

// KIPI include files

#include <libkipi/version.h>
#include <libkipi/interface.h>
#include <libkipi/imagedialog.h>

// Local includes.

#include "galleryconfig.h"
#include "galleries.h"

namespace KIPIGalleryExportPlugin
{

GalleriesList::GalleriesList(KIPI::Interface* pInterface, QWidget *pParent, Galleries* pGalleries)
    : KDialogBase(pParent, 0, true, i18n( "Remote Galleries" ), Close|User1|User2|User3, Close, false),
      m_interface(pInterface),
      mpGalleries(pGalleries)
{
  setButtonGuiItem( Close, KStdGuiItem::close() );
  setButtonGuiItem( User1, KStdGuiItem::remove() );
  setButtonGuiItem( User2, KStdGuiItem::configure() );
  setButtonGuiItem( User3, KStdGuiItem::add() );

  enableButton(User1, false);
  enableButton(User2, false);

  QFrame *page = new QFrame (this);
  QHBoxLayout *tll = new QHBoxLayout(page);
  page->setMinimumSize (400, 200);
  setMainWidget(page);

  QHBoxLayout *hb = new QHBoxLayout ();
  hb->setSpacing (KDialog::spacingHint());
  tll->addItem(hb);

  QLabel *label = new QLabel (page);
  hb->addWidget(label);
  // Todo: Fix the pixmap
  label->setPixmap (UserIcon("sessionchooser"));
  label->setFrameStyle (QFrame::Panel | QFrame::Sunken);
  label->setAlignment(Qt::AlignTop);
  QVBoxLayout *vb = new QVBoxLayout ();
  vb->setSpacing (KDialog::spacingHint());
  tll->addItem(vb);

  mpGalleryList = mpGalleries->asQListView(page);
  vb->addWidget(mpGalleryList);
  connect (mpGalleryList, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
}

GalleriesList::~GalleriesList()
{

}

void GalleriesList::selectionChanged()
{
  bool bln_selected = (mpGalleryList->selectedItem() ? true : false);
  enableButton(User1, bln_selected);
  enableButton(User2, bln_selected);
}

//==================   Add   =====
void GalleriesList::slotUser3(void)
{
  Gallery* p_gallery = new Gallery();
  GalleryEdit dlg(m_interface, this, p_gallery, i18n("New Remote Gallery"));
  if (QDialog::Accepted == dlg.exec())
  {
    mpGalleries->Add(p_gallery);
    mpGalleries->Save();
    p_gallery->asQListViewItem(mpGalleryList);
  }
  else
  {
    delete p_gallery;
  }
}


//==================   Edit  ======
void GalleriesList::slotUser2(void)
{
  QListViewItem* p_lvi = mpGalleryList->selectedItem();
  if (!p_lvi)
  {
    KMessageBox::error(kapp->activeWindow(), i18n("No gallery selected!"));
  }
  else
  {
    GalleryQListViewItem* p_glvi = dynamic_cast<GalleryQListViewItem*>(p_lvi);
    GalleryEdit dlg(m_interface, this, p_glvi->GetGallery(), i18n("Edit Remote Gallery"));
    if (QDialog::Accepted == dlg.exec())
    {
      p_glvi->Refresh();
      mpGalleries->Save();
    }
  }
}


//==================  Remove ======
void GalleriesList::slotUser1(void)
{
  QListViewItem* p_lvi = mpGalleryList->selectedItem();
  if (!p_lvi)
  {
    KMessageBox::error(kapp->activeWindow(), i18n("No gallery selected!"));
  }
  else
  {
    if (KMessageBox::Yes == 
          KMessageBox::warningYesNo(kapp->activeWindow(), 
            i18n("Are you sure you want to remove this gallery? "
                "All synchronisaton settings will be lost. "
                "You cannot undo this action."), 
            i18n("Remove Remote Gallery"), 
            KStdGuiItem::yes(), KStdGuiItem::no(),
            QString::null, KMessageBox::Dangerous))
    {
      GalleryQListViewItem* p_glvi = dynamic_cast<GalleryQListViewItem*>(p_lvi);
      Gallery* p_gallery = p_glvi->GetGallery();
      delete p_glvi;
      mpGalleries->Remove(p_gallery);
      mpGalleries->Save();
    }
  }
}


GalleryEdit::GalleryEdit(KIPI::Interface *pInterface, 
                         QWidget* pParent,
                         Gallery* pGallery,
                         QString title)
    : KDialogBase(pParent, 0, true, title, Ok|Cancel, Ok, false),
      mpInterface(pInterface),
      mpGallery(pGallery)
{
  setButtonGuiItem( Ok, KStdGuiItem::save() );

  QFrame *page = new QFrame (this);
  QHBoxLayout *tll = new QHBoxLayout(page);
  page->setMinimumSize (500, 200);
  setMainWidget(page);

  QVBoxLayout* vbox = new QVBoxLayout();
  vbox->setSpacing (KDialog::spacingHint());
  tll->addItem(vbox);

  mpHeaderLabel = new QLabel(page);
  mpHeaderLabel->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,
                                            QSizePolicy::Fixed));
  mpHeaderLabel->setText(title);
  vbox->addWidget(mpHeaderLabel);

  QFrame* hline = new QFrame(page, "hline");
  hline->setFrameShape(QFrame::HLine);
  hline->setFrameShadow(QFrame::Sunken);
  hline->setFrameShape(QFrame::HLine);
  vbox->addWidget(hline);

  QGridLayout* centerLayout = new QGridLayout(0, 1, 1, 5, 5);

  mpNameEdit = new QLineEdit( this );
  centerLayout->addWidget(mpNameEdit, 0, 1);

  mpUrlEdit = new QLineEdit( this );
  centerLayout->addWidget(mpUrlEdit, 1, 1);

  mpUsernameEdit = new QLineEdit( this );
  centerLayout->addWidget(mpUsernameEdit, 2, 1);

  mpPasswordEdit = new QLineEdit( this );
  mpPasswordEdit->setEchoMode(QLineEdit::Password);
  centerLayout->addWidget(mpPasswordEdit, 3, 1);

  QLabel* name_label = new QLabel(this);
  name_label->setText(i18n( "Name:" ));
  centerLayout->addWidget(name_label, 0, 0);

  QLabel* urlLabel = new QLabel(this);
  urlLabel->setText(i18n( "URL:" ));
  centerLayout->addWidget(urlLabel, 1, 0);

  QLabel* nameLabel = new QLabel(this);
  nameLabel->setText(i18n( "Username:" ));
  centerLayout->addWidget(nameLabel, 2, 0);

  QLabel* passwdLabel = new QLabel(this);
  passwdLabel->setText(i18n( "Password:" ));
  centerLayout->addWidget(passwdLabel, 3, 0);

  //---------------------------------------------
  mpGalleryVersion = new QCheckBox( i18n("Use &Gallery 2"), this);
  mpGalleryVersion->setChecked( 2 == pGallery->version() );
  centerLayout->addWidget( mpGalleryVersion, 4, 1 );
  //---------------------------------------------

  vbox->addLayout( centerLayout );

  resize( QSize(300, 150).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );

  mpNameEdit->setText(pGallery->name());
  mpUrlEdit->setText(pGallery->url());
  mpUsernameEdit->setText(pGallery->username());
  mpPasswordEdit->setText(pGallery->password());
}

GalleryEdit::~GalleryEdit()
{

}

void GalleryEdit::slotOk(void)
{
  if (mpNameEdit->isModified())
    mpGallery->setName(mpNameEdit->text());
  if (mpUrlEdit->isModified())
    mpGallery->setUrl(mpUrlEdit->text());
  if (mpUsernameEdit->isModified())
    mpGallery->setUsername(mpUsernameEdit->text());
  if (mpPasswordEdit->isModified())
    mpGallery->setPassword(mpPasswordEdit->text());
  if (mpGalleryVersion->isChecked())
    mpGallery->setVersion(2);
  else
    mpGallery->setVersion(1);
  accept();
}

}

#include "galleryconfig.moc"

