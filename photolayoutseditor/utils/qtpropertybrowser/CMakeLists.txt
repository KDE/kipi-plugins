INCLUDE(${QT_USE_FILE})
SET(CMAKE_PACKAGE_QTGUI TRUE)

SET(qtpropertyeditor_SRCS
    src/qtpropertybrowser.cpp
    src/qtpropertymanager.cpp
    src/qteditorfactory.cpp
    src/qtvariantproperty.cpp
    src/qttreepropertybrowser.cpp
    src/qtbuttonpropertybrowser.cpp
    src/qtgroupboxpropertybrowser.cpp
    src/qtpropertybrowserutils.cpp
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}/src
                    src
                    ${QT_INCLUDE_DIR}
                   )

QT4_GENERATE_MOC(src/qtpropertybrowser.h
                 ${CMAKE_CURRENT_BINARY_DIR}/moc_qtpropertybrowser.cpp
                )

QT4_GENERATE_MOC(src/qtpropertymanager.h
                 ${CMAKE_CURRENT_BINARY_DIR}/moc_qtpropertymanager.cpp
                )

QT4_GENERATE_MOC(src/qteditorfactory.h
                 ${CMAKE_CURRENT_BINARY_DIR}/moc_qteditorfactory.cpp
                )

QT4_GENERATE_MOC(src/qtvariantproperty.h
                 ${CMAKE_CURRENT_BINARY_DIR}/moc_qtvariantproperty.cpp
                )

QT4_GENERATE_MOC(src/qttreepropertybrowser.h
                 ${CMAKE_CURRENT_BINARY_DIR}/moc_qttreepropertybrowser.cpp
                )

QT4_GENERATE_MOC(src/qtbuttonpropertybrowser.h
                 ${CMAKE_CURRENT_BINARY_DIR}/moc_qtbuttonpropertybrowser.cpp
                )

QT4_GENERATE_MOC(src/qtgroupboxpropertybrowser.h
                 ${CMAKE_CURRENT_BINARY_DIR}/moc_qtgroupboxpropertybrowser.cpp
                )

QT4_GENERATE_MOC(src/qtpropertymanager.cpp
                 ${CMAKE_CURRENT_BINARY_DIR}/qtpropertymanager.moc
                )

QT4_GENERATE_MOC(src/qteditorfactory.cpp
                 ${CMAKE_CURRENT_BINARY_DIR}/qteditorfactory.moc
                )

QT4_GENERATE_MOC(src/qttreepropertybrowser.cpp
                 ${CMAKE_CURRENT_BINARY_DIR}/qttreepropertybrowser.moc
                )

SET(qtpropertyeditor_HEADERS_ONLY_MOC
    ${CMAKE_CURRENT_BINARY_DIR}/moc_qtpropertybrowser.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/moc_qtpropertymanager.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/moc_qteditorfactory.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/moc_qtvariantproperty.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/moc_qttreepropertybrowser.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/moc_qtbuttonpropertybrowser.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/moc_qtgroupboxpropertybrowser.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/qtpropertymanager.moc
    ${CMAKE_CURRENT_BINARY_DIR}/qteditorfactory.moc
    ${CMAKE_CURRENT_BINARY_DIR}/qttreepropertybrowser.moc
   )

SET_SOURCE_FILES_PROPERTIES(${qtpropertyeditor_HEADERS_ONLY_MOC}
                            PROPERTIES
                            HEADER_FILE_ONLY true
                           )

QT4_GENERATE_MOC(src/qtpropertybrowserutils_p.h
                 ${CMAKE_CURRENT_BINARY_DIR}/moc_qtpropertybrowserutils_p.cpp
                )

SET(qtpropertyeditor_HEADERS_MOC
    ${CMAKE_CURRENT_BINARY_DIR}/moc_qtpropertybrowserutils_p.cpp
   )

SET(qtpropertyeditor_MOC
  ${qtpropertyeditor_HEADERS_MOC}
  ${qtpropertyeditor_HEADERS_ONLY_MOC}
)

QT4_ADD_RESOURCES(qtpropertyeditor_RESOURCES
                  src/qtpropertybrowser.qrc
                 )

QT4_ADD_RESOURCES(RESOURCE_SRCS src/qtpropertybrowser.qrc)

SET(SRCS ${extension_MOC}
        ${qtpropertyeditor_MOC}
        ${qtpropertyeditor_SRCS}
        ${qtpropertyeditor_RESOURCES}
   )

FOREACH(_currentfile ${SRCS})
    IF(NOT MSVC)
        SET_SOURCE_FILES_PROPERTIES(${_currentfile} PROPERTIES COMPILE_FLAGS "-fPIC")
    ENDIF(NOT MSVC)
ENDFOREACH(_currentfile ${SRCS})

ADD_LIBRARY(PropertyBrowser STATIC ${SRCS})

IF(NOT MSVC)
    SET_TARGET_PROPERTIES(PropertyBrowser PROPERTIES COMPILE_FLAGS "-fPIC")
ENDIF(NOT MSVC)

TARGET_LINK_LIBRARIES(PropertyBrowser ${QT_LIBRARIES})

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_DLL)
ADD_DEFINITIONS(-DQT_PLUGIN)

INSTALL( TARGETS PropertyBrowser ARCHIVE DESTINATION lib ${INSTALL_TARGETS_DEFAULT_ARGS} )
