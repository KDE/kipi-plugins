IF (NOT WIN32)

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../plugin
                        ${CMAKE_CURRENT_SOURCE_DIR}/../libcvblobs
                        ${OpenCV_INCLUDE_DIR}
                    )

    SET(testipp_SRCS testForIPP.cpp)

    KDE4_ADD_EXECUTABLE(testipp NOGUI
                        ${testipp_SRCS}
                    )

    TARGET_LINK_LIBRARIES(testipp
                        ${OpenCV_LIBRARIES}
                        )

ENDIF (NOT WIN32)
