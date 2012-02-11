KDE4_ADD_EXECUTABLE(gpssync_test_gpxparsing
                    test_gpxparsing.cpp
                    ../gpsdataparser.cpp
                   )

TARGET_LINK_LIBRARIES(gpssync_test_gpxparsing
                        ${KDE4_KDECORE_LIBS}
                        ${QT_QTTEST_LIBRARY}
                        ${QT_QTGUI_LIBRARY}
                        ${QT_QTXML_LIBRARY}
                        ${KGEOMAP_LIBRARIES}
                        kipiplugins
                     )

KDE4_ADD_EXECUTABLE(gpssync_test_rgparsing
                    test_rgparsing.cpp
                    )

TARGET_LINK_LIBRARIES(gpssync_test_rgparsing
                      ${KDE4_KDECORE_LIBS}
                      ${QT_QTTEST_LIBRARY}
                      ${QT_QTGUI_LIBRARY}
                      kipiplugins
                    )

KDE4_ADD_EXECUTABLE(gpssync_test_rgtagmodel
                    test_rgtagmodel.cpp
                    simpletreemodel/simpletreemodel.cpp
                    ../borrowed/modeltest.cpp
                    ../rgtagmodel.cpp
                    )

TARGET_LINK_LIBRARIES(gpssync_test_rgtagmodel
                      ${KDE4_KDECORE_LIBS}
                      ${QT_QTTEST_LIBRARY}
                      ${QT_QTGUI_LIBRARY}
                      kipiplugins
                    )

KDE4_ADD_EXECUTABLE(gpssync_test_kipiimageitem
                    test_kipiimageitem.cpp
                    ../kipiimageitem.cpp
                    ../kipiimagemodel.cpp
                    ${borrowed_future_SRCS}
                   )

TARGET_LINK_LIBRARIES(gpssync_test_kipiimageitem
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      ${QT_QTTEST_LIBRARY}
                      ${QT_QTGUI_LIBRARY}
                      ${KEXIV2_LIBRARIES}
                      ${KGEOMAP_LIBRARIES}
                      ${KIPI_LIBRARIES}
                      kipiplugins
                     )

ADD_TEST(gpssync_test_gpxparsing ${EXECUTABLE_OUTPUT_PATH}/gpssync_test_gpxparsing)
ADD_TEST(gpssync_test_gpsrgparsing ${EXECUTABLE_OUTPUT_PATH}/gpssync_test_rgparsing)
ADD_TEST(gpssync_test_gpsrgtagmodel ${EXECUTABLE_OUTPUT_PATH}/gpssync_test_rgtagmodel)
ADD_TEST(gpssync_test_gpskipiimageitem ${EXECUTABLE_OUTPUT_PATH}/gpssync_test_kipiimageitem)

MACRO_OPTIONAL_ADD_SUBDIRECTORY(simpletreemodel)
