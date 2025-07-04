project(beerocks_controller VERSION ${prplmesh_VERSION})
message("${BoldWhite}Preparing ${BoldGreen}${PROJECT_NAME}${BoldWhite} for the ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
set(ACTION_PATH ${MODULE_PATH}/../../../nbapi)
set(WHM_PATH ${MODULE_PATH}/../../../whm)
set(VBSS_PATH ${MODULE_PATH}/../../../vbss)

file(GLOB controller_sources ${MODULE_PATH}/*.cpp)
file(GLOB controller_tasks_sources ${MODULE_PATH}/tasks/*.cpp)
file(GLOB controller_operations_sources ${MODULE_PATH}/periodic/*.cpp)
file(GLOB controller_db_sources ${MODULE_PATH}/db/*.cpp)

file(GLOB controller_vbss ${VBSS_PATH}/*.cpp)

if (ENABLE_NBAPI)
    file(GLOB controller_actions ${ACTION_PATH}/*.cpp)
endif()

if (USE_PRPLMESH_WHM)
    file(GLOB controller_whm ${WHM_PATH}/*.cpp)
endif()

if (ENABLE_VBSS)
    file(GLOB controller_vbss ${VBSS_PATH}/*.cpp)
endif()


#FEATURE_PRE_ASSOCIATION_STEERING
list(FIND FEATURES "FEATURE_PRE_ASSOCIATION_STEERING" _feature_pre_association_steering_index)
if(${_feature_pre_association_steering_index} GREATER -1)
    file(GLOB RDKB_SOURCES ${MODULE_PATH}/tasks/pre_association_steering/*.cpp)
    list(APPEND controller_tasks_sources  ${RDKB_SOURCES})
endif()

add_executable(${PROJECT_NAME} ${controller_sources} ${controller_tasks_sources}
${controller_operations_sources} ${controller_db_sources} ${controller_actions} ${controller_whm} ${controller_vbss}) 

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-z,defs")

list(APPEND LINKED_LIBS bpl bcl btl tlvf elpp btlvf nbapi ieee1905_transport_lib)
if (ENABLE_NBAPI)
    # Include AMBIORIX in the build
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_NBAPI ${UBUS_SOCK}")
endif()

if (ENABLE_NBAPI OR USE_PRPLMESH_WHM)
    find_package(amxc REQUIRED)
    find_package(amxd REQUIRED)
    find_package(amxo REQUIRED)
    find_package(amxp REQUIRED)
    find_package(libamxrt REQUIRED)
    list(APPEND LINKED_LIBS amxc amxd amxo amxp libamxrt)
endif()

if (USE_PRPLMESH_WHM)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_PRPLMESH_WHM ${WBAPI_SOCK}")

    list(APPEND LINKED_LIBS wbapi)
endif()

if (ENABLE_VBSS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_VBSS")
endif()

if (TARGET_PLATFORM STREQUAL "rdkb")
    list(APPEND LINKED_LIBS breakpadwrapper)
endif()

target_link_libraries(${PROJECT_NAME} ${LINKED_LIBS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${MODULE_PATH}/../bml
    if (ENABLE_NBAPI)
        ${ACTION_PATH}
    endif()
    if (USE_PRPLMESH_WHM)
        ${WHM_PATH}
    endif()
    if (ENABLE_VBSS)
        ${VBSS_PATH}
    endif()
)

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})

add_subdirectory("db/unit_tests")
