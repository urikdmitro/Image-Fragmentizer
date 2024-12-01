cmake_minimum_required(VERSION 3.10)

# Назва проекту
project(Fragmentizer)

# Встановлення стандарту C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Встановлення змінних для директорій
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendor")

# Додавання всіх вихідних файлів проекту
set(SRC_FILES
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/opengl_context.cpp"
    "${SRC_DIR}/resources.cpp"
    "${SRC_DIR}/model/fragment_info.cpp"
    "${SRC_DIR}/model/fragmentizer.cpp"
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
)

# Додавання платформозалежних файлів Native File Dialog
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

# Включення директорій для заголовків
include_directories(
    ${INCLUDE_DIR}
    "${INCLUDE_DIR}/model"
    "${INCLUDE_DIR}/view"
    "${INCLUDE_DIR}/controller"
    "${VENDOR_DIR}/imgui"
    "${VENDOR_DIR}/imgui/backends"
    "${VENDOR_DIR}/nativefiledialog/src/include"
    "${VENDOR_DIR}/stb"
)

# Перевірка GTK
if(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    include_directories(${GTK3_INCLUDE_DIRS})
    link_directories(${GTK3_LIBRARY_DIRS})
    add_definitions(${GTK3_CFLAGS_OTHER})
endif()

# Створення виконуваного файлу
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Лінкування бібліотек
if(WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32 glfw3 glew32)
elseif(UNIX)
    # Для Linux: OpenGL, GLFW, GLEW, GTK, NativeFileDialog
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
    )
endif()
