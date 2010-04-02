
KDE4_ADD_EXECUTABLE(test_gpxparsing
                    test_gpxparsing.cpp
                    ../gpsdataparser.cpp
                   )

TARGET_LINK_LIBRARIES(test_gpxparsing
                        ${KDE4_KDECORE_LIBS}
                        ${QT_QTTEST_LIBRARY}
                        ${QT_QTGUI_LIBRARY}
                        worldmapwidget2
                     )

ADD_TEST(test_gpxparsing ${EXECUTABLE_OUTPUT_PATH}/test_gpxparsing)

