/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.kipi-plugins.org">http://www.kipi-plugins.org</a>
 *
 * @date   2010-06-21
 * @brief  Test for SimpleTreeModel.
 *
 * @author Copyright (C) 2010 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef TEST_SIMPLETREEMODEL_H
#define TEST_SIMPLETREEMODEL_H

// Qt includes

#include <QtTest/QtTest>

// KDE includes

// local includes

class TestSimpleTreeModel : public QObject
{
Q_OBJECT

private Q_SLOTS:

    void testNoOp();
    void testModel1();

};

#endif /* TEST_SIMPLETREEMODEL_H */

