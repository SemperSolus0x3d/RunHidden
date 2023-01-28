cmake_minimum_required(VERSION 3.21)

project(RunHidden VERSION 2.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CTest)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Program.cpp
    src/Stream.cpp
    src/InputStream.cpp
    src/OutputStream.cpp
    src/Pipe.cpp
    src/Utils.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX D)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
    UNICODE
    _UNICODE
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}
)
