#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

PROJECT(qtunittests)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

FIND_PACKAGE(Qt5 5.4.0 COMPONENTS Core Network)

INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})

# ----------------------------------------------------------------------------------

MACRO(QT_UNIT_TESTS_BUILD)

    SET(_filename ${ARGV0})
    STRING(REPLACE "." ";" _temp ${_filename})
    LIST(GET _temp 0 _target)

    SET(${_target}_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/${_target}.cpp
    )

    ADD_EXECUTABLE(${_target} ${${_target}_SRCS})
    SET_TARGET_PROPERTIES(${_target} PROPERTIES OUTPUT_NAME ${_target})

    TARGET_LINK_LIBRARIES(${_target}
                          Qt5::Core
                          Qt5::Network
    )

ENDMACRO()

# ----------------------------------------------------------------------------------

# Add here new tools to compile.
QT_UNIT_TESTS_BUILD(test_qprocess_lambda.cpp)

