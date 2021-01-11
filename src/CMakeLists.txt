cmake_minimum_required(VERSION 3.19)

project(encmetric VERSION 1.0.2 DESCRIPTION "Library to manage encoded strings" LANGUAGES CXX)

#require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(encmetric/config.hpp.in encmetric/config.hpp)

#Riunisce file headers
file(GLOB headers LIST_DIRECTORIES false "encmetric/*.hpp")
file(GLOB t_headers LIST_DIRECTORIES false "encmetric/*.tpp")

add_library(encmetric encoding.cpp utf8_enc.cpp)

#headers
target_include_directories(encmetric PUBLIC "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")

#install
install(TARGETS encmetric DESTINATION lib)
install(FILES ${headers} ${t_headers} DESTINATION include/encmetric)
install(FILES "${PROJECT_BINARY_DIR}/encmetric/config.hpp" DESTINATION include/encmetric)

#optional testing
option(BUILD_TEST "Building test program" OFF)

if(BUILD_TEST)
	message("Building executable")
	add_executable(encmetric_test ../test/main.cpp)
	target_link_libraries(encmetric_test encmetric)
endif()
