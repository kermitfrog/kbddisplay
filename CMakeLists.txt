project(kbddisplay)
cmake_minimum_required(VERSION 2.8.11)
find_package(Qt5Core)
find_package(Qt5Svg)
find_package(Qt5Widgets)
find_package(Qt5Xml)
find_package(Qt5PrintSupport)
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
					styledialog.cpp
					style.cpp
					stylemodel.cpp
					)

set(kbddisplay_UIS kbddisplay.ui keydialog.ui styledialog.ui)
qt5_wrap_ui(kbddisplay_UIS ${kbddisplay_UIS})
#qt5_wrap_cpp(kbddisplay_MOCS ${kbddisplay_SRCS})
add_executable(kbddisplay ${kbddisplay_SRCS} ${kbddisplay_UIS})# ${kbddisplay_MOCS})
target_link_libraries(kbddisplay Qt5::Core Qt5::Widgets Qt5::Xml Qt5::Svg Qt5::PrintSupport)
install(TARGETS kbddisplay RUNTIME DESTINATION bin)

