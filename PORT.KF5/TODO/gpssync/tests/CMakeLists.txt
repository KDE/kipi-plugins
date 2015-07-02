#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

add_executable(gpssync_test_correlator
               test_correlator.cpp
               ../track_correlator.cpp
               ../track_correlator_thread.cpp
              )

target_link_libraries(gpssync_test_correlator
                      KF5::KDELibs4Support
                      Qt5::Test
                      Qt5::Gui
                      Qt5::Xml
                      ${KGEOMAP_LIBRARIES}
                      KF5kipiplugins
                     )

if(NOT WIN32)

    add_executable(gpssync_test_rgparsing
                   test_rgparsing.cpp
                  )

    target_link_libraries(gpssync_test_rgparsing
                          KF5::KDELibs4Support
                          Qt5::Test
                          Qt5::Gui
                          ${KGEOMAP_LIBRARIES}
                          KF5kipiplugins
                         )

endif()

add_executable(gpssync_test_rgtagmodel
               test_rgtagmodel.cpp
               simpletreemodel/simpletreemodel.cpp
               ../borrowed/modeltest.cpp
               ../rgtagmodel.cpp
              )

target_link_libraries(gpssync_test_rgtagmodel
                      KF5::KDELibs4Support
                      Qt5::Test
                      Qt5::Gui
                      ${KGEOMAP_LIBRARIES}
                      KF5kipiplugins
                     )

add_executable(gpssync_test_kipiimageitem
               test_kipiimageitem.cpp
               ../kipiimageitem.cpp
               ../kipiimagemodel.cpp
               ${borrowed_future_SRCS}
              )

target_link_libraries(gpssync_test_kipiimageitem
                      KF5::KDELibs4Support

                      Qt5::Test
                      Qt5::Gui
                      KF5::KExiv2
                      ${KGEOMAP_LIBRARIES}
                      KF5::Kipi
                      KF5kipiplugins
                     )

add_test(gpssync_test_correlator       ${EXECUTABLE_OUTPUT_PATH}/gpssync_test_correlator)
add_test(gpssync_test_gpsrgparsing     ${EXECUTABLE_OUTPUT_PATH}/gpssync_test_rgparsing)
add_test(gpssync_test_gpsrgtagmodel    ${EXECUTABLE_OUTPUT_PATH}/gpssync_test_rgtagmodel)
add_test(gpssync_test_gpskipiimageitem ${EXECUTABLE_OUTPUT_PATH}/gpssync_test_kipiimageitem)

ecm_optional_add_subdirectory(simpletreemodel)
