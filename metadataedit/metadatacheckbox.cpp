/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-18
 * Description : a checkbox with a boolean valid parameter.
 * 
 * Copyright 2006 by Gilles Caulier
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

// Local includes.

#include "metadatacheckbox.h"
#include "metadatacheckbox.moc"

namespace KIPIMetadataEditPlugin
{

MetadataCheckBox::MetadataCheckBox(const QString& text, QWidget* parent)
                : QCheckBox(text, parent)
{
    m_valid = true;    

    connect(this, SIGNAL(toggled(bool)),
            this, SLOT(slotValid()));
}

MetadataCheckBox::~MetadataCheckBox() 
{
}

void MetadataCheckBox::setValid(bool v) 
{
    m_valid = v; 
}

bool MetadataCheckBox::isValid() const 
{
    return m_valid; 
}

void MetadataCheckBox::slotValid() 
{
    setValid(true); 
}

}  // namespace KIPIMetadataEditPlugin
