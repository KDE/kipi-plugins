KDE4_ADD_EXECUTABLE(test_gpxparsing
                    test_gpxparsing.cpp
                    ../gpsdataparser.cpp
                   )

TARGET_LINK_LIBRARIES(test_gpxparsing
                        ${KDE4_KDECORE_LIBS}
                        ${QT_QTTEST_LIBRARY}
                        ${QT_QTGUI_LIBRARY}
                        ${KMAP_LIBRARIES}
                     )

KDE4_ADD_EXECUTABLE(test_rgparsing
                    test_rgparsing.cpp
                    )

TARGET_LINK_LIBRARIES(test_rgparsing
                      ${KDE4_KDECORE_LIBS}
                      ${QT_QTTEST_LIBRARY}
                      ${QT_QTGUI_LIBRARY}
                    )

KDE4_ADD_EXECUTABLE(test_rgtagmodel
                    test_rgtagmodel.cpp
                    simpletreemodel/simpletreemodel.cpp
                    ../borrowed/modeltest.cpp
                    ../rgtagmodel.cpp
                    )

TARGET_LINK_LIBRARIES(test_rgtagmodel
                      ${KDE4_KDECORE_LIBS}
                      ${QT_QTTEST_LIBRARY}
                      ${QT_QTGUI_LIBRARY}
                    )

KDE4_ADD_EXECUTABLE(test_kipiimageitem
                    test_kipiimageitem.cpp
                    ../kipiimageitem.cpp
                    ../kipiimagemodel.cpp
                   )

TARGET_LINK_LIBRARIES(test_kipiimageitem
                      ${KDE4_KDECORE_LIBS}
                      ${QT_QTTEST_LIBRARY}
                      ${QT_QTGUI_LIBRARY}
                      ${KEXIV2_LIBRARIES}
                      ${KMAP_LIBRARIES}
                      ${KIPI_LIBRARIES}
                     )

ADD_TEST(test_gpxparsing ${EXECUTABLE_OUTPUT_PATH}/test_gpxparsing)
ADD_TEST(test_gpsrgparsing ${EXECUTABLE_OUTPUT_PATH}/test_rgparsing)
ADD_TEST(test_gpsrgtagmodel ${EXECUTABLE_OUTPUT_PATH}/test_rgtagmodel)
ADD_TEST(test_gpsexif ${EXECUTABLE_OUTPUT_PATH}/test_gpsexif)

MACRO_OPTIONAL_ADD_SUBDIRECTORY(simpletreemodel)
