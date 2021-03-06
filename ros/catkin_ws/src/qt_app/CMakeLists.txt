﻿cmake_minimum_required(VERSION 2.8.3)
project(qt_app)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  message_generation
  geometry_msgs
  tf
)

find_package(Qt5 COMPONENTS REQUIRED
  Widgets
  Core
  SerialPort
  SerialBus
)

set(CMAKE_AUTOMOC ON) # For meta object compiler
set(CMAKE_AUTORCC ON) # Resource files
set(CMAKE_AUTOUIC ON) # UI files
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
  ${catkin_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  /home/jiaolongdu/Qt/5.13.1/gcc_64/include
  /home/jiaolongdu/djl/graduate_project/ros/catkin_ws/src/qt_app/src/
  /home/jiaolongdu/djl/graduate_project/ros/catkin_ws/src/qt_app/hdr/
)

set(HEADERS
  hdr/mainwindow.h
  hdr/CustomButton.h
  hdr/settingsdialog.h
  hdr/crc.h
  hdr/command.h
  hdr/singleton.h
  hdr/settingparameters.h
  hdr/calculation.h
  hdr/serialportthread.h
  hdr/sigwatcher.h
  hdr/qnode.h
)

set(SOURCES
  src/main.cpp
  src/mainwindow.cpp
  src/CustomButton.cpp
  src/settingsdialog.cpp
  src/crc.cpp
  src/command.cpp
  src/calculation.cpp
  src/serialportthread.cpp
  src/sigwatcher.cpp
  src/qnode.cpp
)

qt5_wrap_ui(WRAP_FILES
  ui/settingsdialog.ui
  ui/mainwindow.ui
)

set(RCC_FILES
  qt_app.qrc
)

## Generate messages in the 'msg' folder
add_message_files(
   FILES
   turtle.msg
#   Message2.msg
)

## Generate services in the 'srv' folder
# add_service_files(
#   FILES
#   Service1.srv
#   Service2.srv
# )

## Generate added messages and services with any dependencies listed here
generate_messages(
  DEPENDENCIES
  std_msgs
  geometry_msgs
)

catkin_package(
  CATKIN_DEPENDS roscpp std_msgs message_runtime
)

#add_executable(talker_node src/talker.cpp)
#add_executable(listener_node src/listener.cpp)

## Specify libraries to link a library or executable target against

set(LIBRARIES ${LIBRARIES}
  ${catkin_LIBRARIES}
  Qt5::Widgets
  Qt5::Core
  Qt5::SerialPort
  Qt5::SerialBus
)

add_executable(qt_app_node ${HEADERS} ${SOURCES} ${WRAP_FILES} ${RCC_FILES})
target_link_libraries(qt_app_node
  ${LIBRARIES}
)
