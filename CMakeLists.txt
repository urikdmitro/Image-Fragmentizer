cmake_minimum_required(VERSION 3.10)

# Project name
project(Fragmentizer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set directories for source, includes, and vendors
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendor")
add_definitions(-DCL_HPP_TARGET_OPENCL_VERSION=300)
# Add all project source files
set(SRC_FILES
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/opengl_context.cpp"
    "${SRC_DIR}/resources.cpp"
    "${SRC_DIR}/model/fragment_info.cpp"
    "${SRC_DIR}/model/fragment_cutter.cpp"
    "${SRC_DIR}/model/opencl_fragment_cutter.cpp"
    "${SRC_DIR}/model/fragmentizer.cpp"
    "${SRC_DIR}/model/fragment_plotter.cpp"
    "${SRC_DIR}/model/image.cpp"
    "${SRC_DIR}/view/gui.cpp"
    "${SRC_DIR}/view/texture.cpp"
    "${SRC_DIR}/controller/fragmentizer_controller.cpp"
    "${VENDOR_DIR}/imgui/imgui.cpp"
    "${VENDOR_DIR}/imgui/imgui_draw.cpp"
    "${VENDOR_DIR}/imgui/imgui_tables.cpp"
    "${VENDOR_DIR}/imgui/imgui_widgets.cpp"
    "${VENDOR_DIR}/imgui/backends/imgui_impl_glfw.cpp"
    "${VENDOR_DIR}/imgui/backends/imgui_impl_opengl3.cpp"
    "${VENDOR_DIR}/implot/implot.cpp"
    "${VENDOR_DIR}/implot/implot_items.cpp"
)

# Add platform-dependent Native File Dialog files
if(WIN32)
    list(APPEND SRC_FILES
        "${VENDOR_DIR}/nativefiledialog/src/nfd_common.c"
        "${VENDOR_DIR}/nativefiledialog/src/nfd_win.cpp"
    )
elseif(UNIX)
    list(APPEND SRC_FILES
        "${VENDOR_DIR}/nativefiledialog/src/nfd_common.c"
        "${VENDOR_DIR}/nativefiledialog/src/nfd_gtk.c"
    )
endif()

# Include directories for headers
include_directories(
    ${INCLUDE_DIR}
    "${INCLUDE_DIR}/model"
    "${INCLUDE_DIR}/view"
    "${INCLUDE_DIR}/controller"
    "${VENDOR_DIR}/imgui"
    "${VENDOR_DIR}/imgui/backends"
    "${VENDOR_DIR}/implot"
    "${VENDOR_DIR}/nativefiledialog/src/include"
    "${VENDOR_DIR}/stb"
    "${CMAKE_SOURCE_DIR}/vendor/OpenCL-Headers/CL"  # Add OpenCL-Headers include path
    "${CMAKE_SOURCE_DIR}/vendor/easyloggingpp/src"
)

# Check for GTK on Unix systems
if(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    include_directories(${GTK3_INCLUDE_DIRS})
    link_directories(${GTK3_LIBRARY_DIRS})
    add_definitions(${GTK3_CFLAGS_OTHER})
endif()

add_library(easyloggingpp STATIC "${CMAKE_SOURCE_DIR}/vendor/easyloggingpp/src/easylogging++.cc")

# Add executable file
add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(easyloggingpp PUBLIC
    vendor/easyloggingpp/src
)
target_link_libraries(${PROJECT_NAME} easyloggingpp)

# Find OpenCL
find_package(OpenCL REQUIRED)

if(OpenCL_FOUND)
    message(STATUS "OpenCL found at ${OpenCL_INCLUDE_DIRS}")
    include_directories(${OpenCL_INCLUDE_DIRS})  # Include OpenCL headers
    target_link_libraries(${PROJECT_NAME} ${OpenCL_LIBRARIES})  # Link OpenCL libraries
else()
    message(FATAL_ERROR "OpenCL not found")
endif()

# Link libraries based on platform
if(WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32 glfw3 glew32)
elseif(UNIX)
    find_package(OpenGL REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW REQUIRED glfw3)
    include_directories(${GLFW_INCLUDE_DIRS})

    target_link_libraries(${PROJECT_NAME}
        ${OPENGL_LIBRARIES}
        ${GLFW_LIBRARIES}
        GLEW
        ${GTK3_LIBRARIES}
        dl
        ${OpenCL_LIBRARIES}  # Link OpenCL
    )
endif()
