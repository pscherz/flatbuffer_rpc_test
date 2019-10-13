cmake_minimum_required(VERSION 3.0)

if(NOT VCPKG_ROOT)
	set(VCPKG_ROOT $ENV{VCPKG_ROOT})
endif()

set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

project(flatbuffer_rpc VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

find_package(Flatbuffers CONFIG REQUIRED)
find_package(ZeroMQ CONFIG REQUIRED)

include_directories(${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

####
####

add_custom_command(OUTPUT rpc_generated.h
	COMMAND flatbuffers::flatc --cpp ${CMAKE_CURRENT_SOURCE_DIR}/rpc.fbs
	DEPENDS rpc.fbs
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

####
####

# Might have to use libzmq instead of lizmq-static on Windows
add_executable(server server.cpp rpc_generated.h)
target_link_libraries(server PRIVATE flatbuffers::flatbuffers libzmq-static)

####
####

add_executable(client client.cpp rpc_generated.h)
target_link_libraries(client PRIVATE flatbuffers::flatbuffers libzmq-static)

####
####

include(GNUInstallDirs)
install(TARGETS server client
	EXPORT ${PROJECT_NAME}-targets
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(EXPORT ${PROJECT_NAME}-targets
	NAMESPACE ${PROJECT_NAME}::
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}
)
