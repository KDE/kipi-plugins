/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-05-08
 * Description : this is a test program to check if
 *               OpenCV is using the IPP infrastructure
 *               of Intel CPUs
 *
 * Copyright (C) 2009 by Andi Clemens <andi dot clemens at googlemail dot com>
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

// C++ includes

#include <cstdio>

// Local includes

#include "libopencv.h"

int main (int /*argc*/, char const** /*argv[]*/)
{
    const char* libs    = 0;
    const char* modules = 0;
    cvGetModuleInfo(0, &libs, &modules);
    printf("Libraries: %s\nModules: %s\n", libs, modules);
    return 0;
}

