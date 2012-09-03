/*
 *  Copyright (C) 2011 Tuomo Penttinen, all rights reserved.
 *
 *  Author: Tuomo Penttinen <tp@herqq.org>
 *
 *  This file is part of Herqq UPnP Av (HUPnPAv) library.
 *
 *  Herqq UPnP Av is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Herqq UPnP Av is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Herqq UPnP Av. If not, see <http://www.gnu.org/licenses/>.
 */

#include "hmoviegenre.h"
#include "hmoviegenre_p.h"

namespace Herqq
{

namespace Upnp
{

namespace Av
{

/*******************************************************************************
 * HMovieGenrePrivate
 ******************************************************************************/
HMovieGenrePrivate::HMovieGenrePrivate(
    const QString& clazz, HObject::CdsType cdsType) :
        HGenreContainerPrivate(clazz, cdsType)
{
}

/*******************************************************************************
 * HMovieGenre
 ******************************************************************************/
HMovieGenre::HMovieGenre(const QString& clazz, CdsType cdsType) :
    HGenreContainer(*new HMovieGenrePrivate(clazz, cdsType))
{
}

HMovieGenre::HMovieGenre(HMovieGenrePrivate& dd) :
    HGenreContainer(dd)
{
}

HMovieGenre::HMovieGenre(
    const QString& title, const QString& parentId, const QString& id) :
        HGenreContainer(*new HMovieGenrePrivate(sClass(), sType()))
{
    init(title, parentId, id);
}

HMovieGenre::~HMovieGenre()
{
}

HMovieGenre* HMovieGenre::newInstance() const
{
    return new HMovieGenre();
}

}
}
}
