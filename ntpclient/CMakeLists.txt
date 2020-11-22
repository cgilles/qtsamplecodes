#
# Copyright (c) 2010-2020, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

PROJECT(qtntpclient)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

FIND_PACKAGE(Qt5 5.4.0 COMPONENTS Core Network)

INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})

# ------------------------------------------------------------------------------------------

SET(ntpclient_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/ntpclient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ntppackage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ntptimestamp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ntpnotifier.cpp
)

ADD_LIBRARY(ntpclient STATIC ${ntpclient_SRCS})
TARGET_LINK_LIBRARIES(ntpclient Qt5::Core Qt5::Network)

# ------------------------------------------------------------------------------------------

SET(test_ntp_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/test_ntp.cpp)
ADD_EXECUTABLE(test_ntp ${test_ntp_SRCS})
TARGET_LINK_LIBRARIES(test_ntp ntpclient)
