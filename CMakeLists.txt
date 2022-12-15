cmake_minimum_required(VERSION 3.21)

project(RunHidden VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CTest)

add_executable(${PROJECT_NAME} src/main.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX D)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}
)
