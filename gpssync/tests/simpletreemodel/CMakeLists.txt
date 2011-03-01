
KDE4_ADD_EXECUTABLE(gpssync_test_simpletreemodel
                    test_simpletreemodel.cpp
                    simpletreemodel.cpp
                    ../../borrowed/modeltest.cpp
                   )

TARGET_LINK_LIBRARIES(gpssync_test_simpletreemodel
                        ${KDE4_KDECORE_LIBS}
                        ${QT_QTTEST_LIBRARY}
                        ${QT_QTGUI_LIBRARY}
                     )

ADD_TEST(gpssync_test_simpletreemodel ${EXECUTABLE_OUTPUT_PATH}/gpssync_test_simpletreemodel)

