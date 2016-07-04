project(kbddisplay)
cmake_minimum_required(VERSION 2.8.11)
find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Xml)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

ADD_DEFINITIONS(
    -std=c++11
    )


include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})


set(kbddisplay_SRCS kbddisplay.cpp
					main.cpp
					keyitemmodel.cpp
					qgraphicskeyitem.cpp
					stylechooser.cpp
					keyboardview.cpp
					keydialog.cpp
					)

set(kbddisplay_UIS kbddisplay.ui keydialog.ui)
qt5_wrap_ui(kbddisplay_UIS ${kbddisplay_UIS})
#qt5_wrap_cpp(kbddisplay_MOCS ${kbddisplay_SRCS})
add_executable(kbddisplay ${kbddisplay_SRCS} ${kbddisplay_UIS})# ${kbddisplay_MOCS})
target_link_libraries(kbddisplay Qt5::Core Qt5::Widgets Qt5::Xml)
install(TARGETS kbddisplay RUNTIME DESTINATION bin)

