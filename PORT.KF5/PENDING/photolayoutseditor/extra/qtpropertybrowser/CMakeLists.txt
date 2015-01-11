#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include(${QT_USE_FILE})
set(CMAKE_PACKAGE_QTGUI TRUE)

set(qtpropertyeditor_SRCS
    src/qtpropertybrowser.cpp
    src/qtpropertymanager.cpp
    src/qteditorfactory.cpp
    src/qtvariantproperty.cpp
    src/qttreepropertybrowser.cpp
    src/qtbuttonpropertybrowser.cpp
    src/qtgroupboxpropertybrowser.cpp
    src/qtpropertybrowserutils.cpp
   )

include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    src
                    ${QT_INCLUDE_DIR}
                   )

qt5_generate_moc(src/qtpropertybrowser.h
                 ${CMAKE_CURRENT_BINARY_DIR}/moc_qtpropertybrowser.cpp
                )

qt5_generate_moc(src/qtpropertymanager.h
                 ${CMAKE_CURRENT_BINARY_DIR}/moc_qtpropertymanager.cpp
                )

qt5_generate_moc(src/qteditorfactory.h
                 ${CMAKE_CURRENT_BINARY_DIR}/moc_qteditorfactory.cpp
                )

qt5_generate_moc(src/qtvariantproperty.h
                 ${CMAKE_CURRENT_BINARY_DIR}/moc_qtvariantproperty.cpp
                )

qt5_generate_moc(src/qttreepropertybrowser.h
                 ${CMAKE_CURRENT_BINARY_DIR}/moc_qttreepropertybrowser.cpp
                )

qt5_generate_moc(src/qtbuttonpropertybrowser.h
                 ${CMAKE_CURRENT_BINARY_DIR}/moc_qtbuttonpropertybrowser.cpp
                )

qt5_generate_moc(src/qtgroupboxpropertybrowser.h
                 ${CMAKE_CURRENT_BINARY_DIR}/moc_qtgroupboxpropertybrowser.cpp
                )

qt5_generate_moc(src/qtpropertymanager.cpp
                 ${CMAKE_CURRENT_BINARY_DIR}/qtpropertymanager.moc
                )

qt5_generate_moc(src/qteditorfactory.cpp
                 ${CMAKE_CURRENT_BINARY_DIR}/qteditorfactory.moc
                )

qt5_generate_moc(src/qttreepropertybrowser.cpp
                 ${CMAKE_CURRENT_BINARY_DIR}/qttreepropertybrowser.moc
                )

set(qtpropertyeditor_HEADERS_ONLY_MOC
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

set_source_files_properties(${qtpropertyeditor_HEADERS_ONLY_MOC}
                            PROPERTIES
                            HEADER_FILE_ONLY true
                           )

qt5_generate_moc(src/qtpropertybrowserutils_p.h
                 ${CMAKE_CURRENT_BINARY_DIR}/moc_qtpropertybrowserutils_p.cpp
                )

set(qtpropertyeditor_HEADERS_MOC
    ${CMAKE_CURRENT_BINARY_DIR}/moc_qtpropertybrowserutils_p.cpp
   )

set(qtpropertyeditor_MOC
    ${qtpropertyeditor_HEADERS_MOC}
    ${qtpropertyeditor_HEADERS_ONLY_MOC}
   )

qt5_add_resources(qtpropertyeditor_RESOURCES
                  src/qtpropertybrowser.qrc
                 )

qt5_add_resources(RESOURCE_SRCS src/qtpropertybrowser.qrc)

set(SRCS ${extension_MOC}
        ${qtpropertyeditor_MOC}
        ${qtpropertyeditor_SRCS}
        ${qtpropertyeditor_RESOURCES}
   )

foreach(_currentfile ${SRCS})
    if(NOT MSVC)
        set_source_files_properties(${_currentfile} PROPERTIES COMPILE_FLAGS "-fPIC")
    endif()
endforeach()

add_library(PropertyBrowser STATIC ${SRCS})

if(NOT MSVC)
    set_target_properties(PropertyBrowser PROPERTIES COMPILE_FLAGS "-fPIC")
endif()

target_link_libraries(PropertyBrowser ${QT_LIBRARIES})

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_DLL)
add_definitions(-DQT_PLUGIN)

# Do not install (yet), see https://bugs.kde.org/show_bug.cgi?id=319664
# install(TARGETS PropertyBrowser ARCHIVE DESTINATION lib ${INSTALL_TARGETS_DEFAULT_ARGS})
