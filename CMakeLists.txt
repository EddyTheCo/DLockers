cmake_minimum_required(VERSION 3.24)

set(USE_QML ON)
option(BUILD_CLIENT "Build the Client application" ON)
option(BUILD_SERVER "Build the Server application" ON)
option(RPI_SERVER "Build real server or test Server" OFF)

include(${CMAKE_CURRENT_BINARY_DIR}/local_conf.cmake OPTIONAL)


include(FetchContent)
FetchContent_Declare(
	ccommon
	GIT_REPOSITORY https://github.com/EddyTheCo/Common.git
	GIT_TAG v0.1.1
	)
FetchContent_MakeAvailable(ccommon)
version_from_git(
	LOG       OFF
	TIMESTAMP "%Y%m%d%H%M%S"
	)

project(DLockers VERSION ${VERSION} DESCRIPTION "Decentralized Lockers" LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
include(GNUInstallDirs)


FetchContent_Declare(
	EstervDesigns
	GIT_REPOSITORY https://github.com/EddyTheCo/MyDesigns.git
        GIT_TAG v1.1.1
	FIND_PACKAGE_ARGS 1.1  CONFIG
	)
FetchContent_MakeAvailable(EstervDesigns)


FetchContent_Declare(
	IotaWallet
	GIT_REPOSITORY https://github.com/EddyTheCo/qWallet-IOTA.git
        GIT_TAG v0.2.1
	FIND_PACKAGE_ARGS 0.1 CONFIG
	)
FetchContent_MakeAvailable(IotaWallet)

add_subdirectory(Bookings)

if(BUILD_CLIENT)
	add_subdirectory(Client)
endif(BUILD_CLIENT)

if(BUILD_SERVER)
	add_subdirectory(Server)
endif(BUILD_SERVER)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CPACK_PACKAGE_CONTACT "estervtech")
    set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-v${SEMVER}-${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_VERSION}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_CXX_COMPILER_ID}")
    if(USE_THREADS)
        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-MThread")
    endif(USE_THREADS)
    include(CPack)
endif()
