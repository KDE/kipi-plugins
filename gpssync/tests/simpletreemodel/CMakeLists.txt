
KDE4_ADD_EXECUTABLE(test_simpletreemodel
                    test_simpletreemodel.cpp
                    simpletreemodel.cpp
                    ../../borrowed/modeltest.cpp
                   )

TARGET_LINK_LIBRARIES(test_simpletreemodel
                        ${KDE4_KDECORE_LIBS}
                        ${QT_QTTEST_LIBRARY}
                        ${QT_QTGUI_LIBRARY}
                     )

ADD_TEST(test_simpletreemodel ${EXECUTABLE_OUTPUT_PATH}/test_simpletreemodel)

