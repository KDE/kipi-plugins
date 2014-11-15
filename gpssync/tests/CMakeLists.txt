#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

KDE4_ADD_EXECUTABLE(gpssync_test_correlator
                    test_correlator.cpp
                    ../track_correlator.cpp
                    ../track_correlator_thread.cpp
                   )

target_link_libraries(gpssync_test_correlator
                        ${KDE4_KDECORE_LIBS}
                        ${QT_QTTEST_LIBRARY}
                        ${QT_QTGUI_LIBRARY}
                        ${QT_QTXML_LIBRARY}
                        ${KGEOMAP_LIBRARIES}
                        kipiplugins
                     )

if(NOT WIN32)

    KDE4_ADD_EXECUTABLE(gpssync_test_rgparsing
               test_rgparsing.cpp
               )

    target_link_libraries(gpssync_test_rgparsing
                          ${KDE4_KDECORE_LIBS}
                          ${QT_QTTEST_LIBRARY}
                          ${QT_QTGUI_LIBRARY}
                          ${KGEOMAP_LIBRARIES}
                          kipiplugins
                         )

endif()

KDE4_ADD_EXECUTABLE(gpssync_test_rgtagmodel
                    test_rgtagmodel.cpp
                    simpletreemodel/simpletreemodel.cpp
                    ../borrowed/modeltest.cpp
                    ../rgtagmodel.cpp
                    )

target_link_libraries(gpssync_test_rgtagmodel
                      ${KDE4_KDECORE_LIBS}
                      ${QT_QTTEST_LIBRARY}
                      ${QT_QTGUI_LIBRARY}
                      ${KGEOMAP_LIBRARIES}
                      kipiplugins
                    )

KDE4_ADD_EXECUTABLE(gpssync_test_kipiimageitem
                    test_kipiimageitem.cpp
                    ../kipiimageitem.cpp
                    ../kipiimagemodel.cpp
                    ${borrowed_future_SRCS}
                   )

target_link_libraries(gpssync_test_kipiimageitem
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      ${QT_QTTEST_LIBRARY}
                      ${QT_QTGUI_LIBRARY}
                      ${KEXIV2_LIBRARIES}
                      ${KGEOMAP_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      kipiplugins
                     )

add_test(gpssync_test_correlator       ${EXECUTABLE_OUTPUT_PATH}/gpssync_test_correlator)
add_test(gpssync_test_gpsrgparsing     ${EXECUTABLE_OUTPUT_PATH}/gpssync_test_rgparsing)
add_test(gpssync_test_gpsrgtagmodel    ${EXECUTABLE_OUTPUT_PATH}/gpssync_test_rgtagmodel)
add_test(gpssync_test_gpskipiimageitem ${EXECUTABLE_OUTPUT_PATH}/gpssync_test_kipiimageitem)

MACRO_OPTIONAL_ADD_SUBDIRECTORY(simpletreemodel)
