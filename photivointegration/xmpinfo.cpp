/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-06-21
 * Description : Integration of the Photivo RAW-Processor.
 *
 * Copyright (C) 2012 by Dominic Lyons <domlyons at googlemail dot com>
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

//STL includes
#include <iostream>

//KDE includes
#include <KDebug>

//KIPI includes
#include "xmpinfo.h"

namespace KIPIPhotivoIntegrationPlugin
{

// public /////////////////////////////////////////////////////////////////////

QString XmpInfo::isDerivate(const QString& image) const
{
    const KPMetadata meta;
    
    if (meta.load(image)) {
	QString orig;
	QString current;
	
	// Read sidecar at first: Embedded metadata has precedence, so if both 
	// exist the doubled entries will be overwritten by embedded data later on
	if (meta.hasSidecar(image)) {
	    //TODO
	}
	
	// Now read embedded XMP
	if (meta.hasXmp()) {
	    orig    = meta.getXmpTagString("Xmp.xmpMM.OriginalDocumentID");
	    current = meta.getXmpTagString("Xmp.xmpMM.DocumentID");
	}
	
	// Evaluate 
	if (!current.isEmpty() && !orig.isEmpty()) {
	    if (current == orig) return "Original";
	    return QString("Derived from ") + orig; //TODO:Mapping UUID -> filename
	}
    } else kWarning() << "Loading Metadata for file" << image << "failed";
    
    return "- unknown -";
}


// private ////////////////////////////////////////////////////////////////////

}