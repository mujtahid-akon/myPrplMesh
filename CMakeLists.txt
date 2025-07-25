cmake_minimum_required(VERSION 3.12)
cmake_policy(VERSION 3.12)
include(cmake/multiap-helpers.cmake)
project(prplmesh
        LANGUAGES C CXX
        VERSION ${prplmesh_VERSION})

if(NOT DEFINED TARGET_PLATFORM)
    set(TARGET_PLATFORM "linux")
endif()

option(KEEP_UCI_GENERAL_OPTIONS "Use UCI for management and certification mode" ON)
if (KEEP_UCI_GENERAL_OPTIONS)
    add_definitions(-DKEEP_UCI_GENERAL_OPTIONS)
endif()

## Function to generate defines for ODLs
function(process_root_dm ROOT_DM_VAR ROOT_DM_LITERAL_VAR ROOT_DM_HEADER_VAR ROOT_DM_FOOTER_VAR)
    string(REPLACE "." "\\." ${ROOT_DM_LITERAL_VAR} ${${ROOT_DM_VAR}})
    set(${ROOT_DM_HEADER_VAR} "")
    set(${ROOT_DM_FOOTER_VAR} "")

    string(REPLACE "." ";" ROOT_DM_SPLIT ${${ROOT_DM_VAR}})
    
    set(indent "")
    set(header "")
    set(footer "")

    foreach(object IN LISTS ROOT_DM_SPLIT)
        set(header "${header}${indent}%persistent object ${object} {\n")
        set(footer "}\n${footer}")
        set(indent "    ${indent}")
    endforeach()

    set(${ROOT_DM_HEADER_VAR} "${header}" PARENT_SCOPE)
    set(${ROOT_DM_FOOTER_VAR} "${footer}" PARENT_SCOPE)
endfunction()

## Device.WiFi.DataElements
if (NOT DEFINED DATAELEMENTS_ROOT_DM)
    set(DATAELEMENTS_ROOT_DM "Device.WiFi.DataElements")
endif()

## PrplMesh Controller datamodel root path
if (NOT DEFINED CONTROLLER_ROOT_DM)
    set(CONTROLLER_ROOT_DM "X_PRPLWARE-COM_Controller")
endif()

## PrplMesh Agent datamodel root path
if (NOT DEFINED AGENT_ROOT_DM)
    set(AGENT_ROOT_DM "X_PRPLWARE-COM_Agent")
endif()

add_definitions(-DDATAELEMENTS_ROOT_DM="${DATAELEMENTS_ROOT_DM}")
add_definitions(-DCONTROLLER_ROOT_DM="${CONTROLLER_ROOT_DM}")
add_definitions(-DAGENT_ROOT_DM="${AGENT_ROOT_DM}")


process_root_dm(DATAELEMENTS_ROOT_DM DATAELEMENTS_ROOT_DM_literal DATAELEMENTS_ROOT_DM_Header DATAELEMENTS_ROOT_DM_Footer)
process_root_dm(CONTROLLER_ROOT_DM CONTROLLER_ROOT_DM_literal CONTROLLER_ROOT_DM_Header CONTROLLER_ROOT_DM_Footer)
process_root_dm(AGENT_ROOT_DM AGENT_ROOT_DM_literal AGENT_ROOT_DM_Header AGENT_ROOT_DM_Footer)


## Build options

option (BUILD_AGENT "Build EasyMesh agent" ON)
option (BUILD_CONTROLLER "Build EasyMesh controller" ON)

option(BUILD_SHARED_LIBS "Build shared libraries (.so) instead of static ones (.a)" ON)
option (ENABLE_NBAPI "Build the northbound (high-level) API" ON)
option (ENABLE_VBSS "Build with Virtual BSS support" ON)

option(USE_PRPLMESH_WHM "Use prplMesh Wireless Hardware Manager" ON)

## Generic checks and defaults

include(FindPkgConfig)
include(GNUInstallDirs)

# Add global project cmake modules path
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE)
endif()

# Update output directories for easy deployment
set(CMAKE_MULTIAP_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/bin)

message(STATUS "CMAKE_FIND_ROOT_PATH - ${CMAKE_FIND_ROOT_PATH}")
message(STATUS "CMAKE_INSTALL_PREFIX - ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_MODULE_PATH - ${CMAKE_MODULE_PATH}")
message(STATUS "CMAKE_PREFIX_PATH - ${CMAKE_PREFIX_PATH}")
message(STATUS "CMAKE_MULTIAP_OUTPUT_DIRECTORY - ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Do not fall back to C++98

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  # When compiling with openWrt, FORTIFY_SOURCE=1 always gets added.
  # Since we anyway use FORTIFY_SOURCE=2 and it cannot be set two times
  # with a different value, we remove the one set externally, and add our own
  string(REGEX REPLACE "-D_FORTIFY_SOURCE=[0-9]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  string(REGEX REPLACE "-D_FORTIFY_SOURCE=[0-9]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  string(REGEX REPLACE "-D_FORTIFY_SOURCE=[0-9]" "" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
  string(REGEX REPLACE "-D_FORTIFY_SOURCE=[0-9]" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
  add_definitions("-D_FORTIFY_SOURCE=2")
endif()

# Default Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -pthread -Wno-comment -Wno-unused-parameter -Wno-missing-field-initializers -fno-exceptions")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s -O2 -Wformat -Wformat-security -Wl,-S -fPIC -fPIE -fno-exceptions")

# Compiler specific flags
if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 9.1)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-address-of-packed-member")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fstack-protector-strong")
else()
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fstack-protector")
endif()

# Release flags
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_C_FLAGS_RELEASE}")

# Linker Options
# - Setting the "rpath-link" linker variable to help the linker resolve secondery dependecies
#   (Libraries that are not directly referenced by the executable, but a linked library)
# - Setting "rpath" linker to add the cmake installed library folder to the linker search path
#   (Removes the need to set LD_LIBRARY_PATH manually when installing to a none standard location)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath-link=${PLATFORM_STAGING_DIR}/usr/lib")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z noexecstack -z relro -z now -pie")
endif()

list (FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" isSystemDir)
if ("${isSystemDir}" STREQUAL "-1")
    # Make the executables relocatable
    set(CMAKE_INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
endif ("${isSystemDir}" STREQUAL "-1")
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(INSTALL_PATH "/opt/prplmesh" CACHE PATH "prplMesh installation directory")
set(TMP_PATH "/tmp/beerocks")

if(TARGET_PLATFORM STREQUAL "linux")
    set(INSTALL_PATH "${CMAKE_INSTALL_PREFIX}")
endif()

# Default values
set(BEEROCKS_REPEATER_MODE 0)
set(BEEROCKS_MONITOR_POLLING_RATE_MSEC 250)

# Platform specific flags
if (TARGET_PLATFORM STREQUAL "openwrt")
    if (TARGET_PLATFORM_TYPE STREQUAL "ugw")
        add_definitions(-DBEEROCKS_UGW)
        list(APPEND FEATURES "FEATURE_PRE_ASSOCIATION_STEERING")
        add_definitions(-DFEATURE_PRE_ASSOCIATION_STEERING)
        set(BEEROCKS_BH_WIRE_IFACE "eth0_1" CACHE STRING "Backhaul wire iface")
        set(BEEROCKS_REPEATER_MODE 1)
        set(BEEROCKS_MONITOR_POLLING_RATE_MSEC 1000)
    elseif (TARGET_PLATFORM_TYPE STREQUAL "freedom")
        set(BEEROCKS_BH_WIRE_IFACE "wan" CACHE STRING "Backhaul wire iface")
        set(BEEROCKS_WLAN2_IFACE "wlan1")
        set(BEEROCKS_WLAN2_STEERING_VAPS "wlan1.0")
    else()
        set(BEEROCKS_BH_WIRE_IFACE "wan" CACHE STRING "Backhaul wire iface")
        set(BEEROCKS_WLAN2_IFACE "wlan1")
        set(BEEROCKS_WLAN2_STEERING_VAPS "wlan1.0")
    endif()
elseif (TARGET_PLATFORM STREQUAL "rdkb")
    add_definitions(-DBEEROCKS_RDKB)
    list(APPEND FEATURES "FEATURE_PRE_ASSOCIATION_STEERING")
    add_definitions(-DFEATURE_PRE_ASSOCIATION_STEERING)
    set(BEEROCKS_BRIDGE_IFACE  "brlan0" CACHE STRING "LAN bridge")
    set(BEEROCKS_BH_WIRE_IFACE "nsgmii0" CACHE STRING "Backhaul wire iface")
    # Disable fronthaul wired ports
    set(BEEROCKS_FH_WIRE_IFACE "" CACHE STRING "Fronthaul wire iface")
    # Disable file and enable syslog logging
    set(BEEROCKS_LOG_FILES_ENABLED "false" CACHE STRING "Write prplMesh logs into files")
    set(BEEROCKS_LOG_SYSLOG_ENABLED "true" CACHE STRING "Send logs to syslog")
    set(BEEROCKS_LOG_FILES_AUTO_ROLL "false" CACHE STRING "Auto rollback prplMesh logs")
    set(BEEROCKS_LOG_FILES_PATH "/rdklogs/logs" CACHE PATH "prplMesh logs directory")
    set(BEEROCKS_LOG_FILES_SUFFIX ".txt.0" CACHE STRING "prplMesh logs filenames suffix")
    set(BEEROCKS_MONITOR_POLLING_RATE_MSEC 1000)
    set(CMAKE_SKIP_RPATH TRUE)
elseif (TARGET_PLATFORM STREQUAL "linux")
    add_definitions(-DBEEROCKS_LINUX)
    set(BEEROCKS_BH_WIRE_IFACE "eth1" CACHE STRING "Backhaul wire iface")
endif()

if (TARGET_PLATFORM_TYPE STREQUAL "turris-omnia")
    add_compile_definitions(MISALIGNED_DWORD_ACCESS_NEEDS_SPECIAL_HANDLING)
endif()

# Default values - cached (are set only if not previously set)
set(BEEROCKS_WLAN1_IFACE   "wlan0" CACHE STRING "1st WLAN iface")
set(BEEROCKS_WLAN2_IFACE   "wlan2" CACHE STRING "2nd WLAN iface")
set(BEEROCKS_WLAN3_IFACE   "wlan4" CACHE STRING "3rd WLAN iface")
set(BEEROCKS_BRIDGE_IFACE  "br-lan" CACHE STRING "LAN bridge")
set(BEEROCKS_BH_WIRE_IFACE "eth1" CACHE STRING "Backhaul wire iface")

# Logging configuration - cached (are set only if not previously set)
set(BEEROCKS_LOG_FILES_ENABLED   "true" CACHE STRING "Write prplMesh logs into files")
set(BEEROCKS_LOG_FILES_PATH      "${TMP_PATH}/logs" CACHE PATH "prplMesh logs directory")
set(BEEROCKS_LOG_FILES_SUFFIX    ".log" CACHE STRING "prplMesh logs filenames suffix")
set(BEEROCKS_LOG_FILES_AUTO_ROLL "true" CACHE STRING "Auto rollback prplMesh logs")
set(BEEROCKS_LOG_STDOUT_ENABLED  "false" CACHE STRING "Print logs to stdout")
set(BEEROCKS_LOG_SYSLOG_ENABLED  "false" CACHE STRING "Send logs to syslog")
if (TARGET_PLATFORM STREQUAL "linux")
    set(BEEROCKS_CONTROLLER_LOG_SIZE "30000000" CACHE STRING "Max controller log size")
    set(BEEROCKS_AGENT_LOG_SIZE "2000000" CACHE STRING "Max agent log size")
else()
    set(BEEROCKS_CONTROLLER_LOG_SIZE "1000000" CACHE STRING "Max controller log size")
    set(BEEROCKS_AGENT_LOG_SIZE "1000000" CACHE STRING "Max agent log size")
endif()

# Test support
option(BUILD_TESTS "build multiap unit tests" OFF)
if (BUILD_TESTS)
    enable_testing()
    message(STATUS "Tests are enabled")

    # Compile GoogleTest statically (so we don't have to install libraries)
    set(BUILD_SHARED_LIBS_SAVED "${BUILD_SHARED_LIBS}")
    set(BUILD_SHARED_LIBS OFF)

    # GoogleTest
    # https://github.com/google/googletest/blob/master/googletest/README.md#incorporating-into-an-existing-cmake-project
    include(cmake/googletest.cmake)

    # Revert it
    set(BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS_SAVED}")
endif()

# Code coverage analysis support
option(COVERAGE "add run-time instrumentation for test coverage analysis" OFF)
if (COVERAGE)
    message(STATUS "Coverage support is enabled")
else()
    message(STATUS "Coverage support is not enabled")
endif()

add_definitions(-DBEEROCKS_VERSION="${prplmesh_VERSION}")
add_definitions(-DBEEROCKS_BUILD_DATE="${BUILD_DATE}")
add_definitions(-DBEEROCKS_REVISION="${PRPLMESH_REVISION}")

## Components

add_subdirectory(framework)
add_subdirectory(common)
if (BUILD_AGENT)
    add_subdirectory(agent)
    add_subdirectory(vendor_message)
endif()
if (BUILD_CONTROLLER)
    add_subdirectory(controller)
endif()

## Version
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/version.in"
    "${CMAKE_CURRENT_BINARY_DIR}/version"
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/version DESTINATION config)
file(COPY ${CMAKE_CURRENT_BINARY_DIR}/version DESTINATION "${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/config")

## ACL file path in the final prplOS image
set(ACL_DIR /etc/acl/admin)

if (BUILD_CONTROLLER AND ENABLE_NBAPI)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/acl/admin/prplmesh.json.in"
        "${CMAKE_CURRENT_BINARY_DIR}/prplmesh.json"
    )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/prplmesh.json DESTINATION ${ACL_DIR})
endif()
