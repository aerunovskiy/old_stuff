cmake_minimum_required(VERSION 3.15.3)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

project(oop_sem4)

#[[ stl_01 ]]
# erunovskiy
add_executable(stl_01_01 	stl_01/task1.cpp)
add_executable(stl_01_02 	stl_01/task2.cpp)
add_executable(stl_01_03 	stl_01/task3.cpp)
add_executable(stl_01_04 	stl_01/task4.cpp)
add_executable(stl_01_05 	stl_01/task5.cpp)
add_executable(stl_01_06 	stl_01/task6.cpp)
add_executable(stl_01_stl_sort 	stl_01/stl_sort.cpp)

#[[ stl_02 ]]
# erunovskiy
add_executable(stl_02 stl_02/main.cpp)

#[[ stl_03 ]]
# erunovskiy
add_executable(stl_03 stl_03/main.cpp stl_03/queuewithpriority.cpp stl_03/queuewithpriority.cpp)

#[[ stl_04 ]]
# erunovskiy
add_executable(stl_04 stl_04/main.cpp stl_04/Contacts.cpp stl_04/Contact.cpp stl_04/Contacts.h stl_04/Contacts.h)

#[[ stl_05 ]]
# erunovskiy
add_executable(stl_05 stl_05/main.cpp)

#[[ stl_06 ]]
# erunovskiy
add_executable(stl_06_1 stl_06/text.cpp)
add_executable(stl_06_2 stl_06/draw.cpp)

#[[ stl_07 ]]
# erunovskiy
add_executable(stl_07 stl_07/main.cpp)

#[[ stl_08 ]]
# erunovskiy
add_executable(stl_08_1 stl_08/pi.cpp)
add_executable(stl_08_2 stl_08/shape.cpp)
