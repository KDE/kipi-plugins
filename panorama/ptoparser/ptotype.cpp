/* ============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-04
 * Description : a plugin to create panorama by fusion of several images.
 *               This type is based on pto file format described here:
 *               http://hugin.sourceforge.net/docs/nona/nona.txt, and
 *               on pto files produced by Hugin's tools.
 *
 * Copyright (C) 2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#include "ptotype.h"

namespace KIPIPanoramaPlugin
{

void PTOType::setProject(PTOType::Project& p)
{
    project = Project(p);
    project.previousComments = QStringList(lastComments);
    lastComments = QStringList();
}

void PTOType::setStitcher(PTOType::Stitcher& s)
{
    stitcher = Stitcher(s);
    stitcher.previousComments = QStringList(lastComments);
    lastComments = QStringList();
}

void PTOType::addImage(PTOType::Image& i)
{
    images.push_back(i);
    int last = images.size() - 1;
    images[last].id = last;
    images[last].previousComments = QStringList(lastComments);
    lastComments = QStringList();
}

void PTOType::addMask(PTOType::Mask& m, int imageId)
{
    images[imageId].masks.push_back(m);
    int last = images[imageId].masks.size() - 1;
    images[imageId].masks[last].previousComments = QStringList(lastComments);
    lastComments = QStringList();
}

void PTOType::addOptimisation(PTOType::Optimisation& m, int imageId)
{
    images[imageId].optimisationParameters.push_back(m);
    int last = images[imageId].optimisationParameters.size() - 1;
    images[imageId].optimisationParameters[last].previousComments = QStringList(lastComments);
    lastComments = QStringList();
}

void PTOType::addControlPoint(PTOType::ControlPoint& c)
{
    controlPoints.push_back(c);
    int last = controlPoints.size() - 1;
    controlPoints[last].previousComments = QStringList(lastComments);
    lastComments = QStringList();
}

} // namespace KIPIPanoramaPlugin
