cmake_minimum_required(VERSION 3.24)
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
project(DLockersMap VERSION ${VERSION} DESCRIPTION "A QML module for showing Lockers in a map" LANGUAGES CXX)


set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
include(GNUInstallDirs)


find_package(Qt6 COMPONENTS Core Gui Quick Location Positioning)
if (Qt6_FOUND)
	qt_standard_project_setup()

	qt6_add_qml_module( CoordToMap
                URI  Esterv.Dlockers.CoordToMap
		VERSION 1.0
                SOURCES
                "src/objtomap.cpp"
                "include/objtomap.hpp"
		QML_FILES
                "qml/ObjPopUp.qml"
                "qml/ObjDelegate.qml"
		"qml/ObjectMapView.qml"
		RESOURCE_PREFIX
		"/esterVtech.com/imports"
		OUTPUT_TARGETS out_targets_var
		OUTPUT_DIRECTORY
                ${CMAKE_BINARY_DIR}/Esterv/Dlockers/CoordToMap
		IMPORT_PATH ${CMAKE_BINARY_DIR} 
		)

	add_library(${PROJECT_NAME}::CoordToMap ALIAS CoordToMap)
	add_library(${PROJECT_NAME}::CoordToMapplugin ALIAS CoordToMapplugin)

        target_include_directories(CoordToMap PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Esterv/${PROJECT_NAME}>")
        target_link_libraries(CoordToMap PUBLIC Qt6::Gui Qt6::Quick Qt6::Core Qt6::Positioning  Qt6::Location )

	install(TARGETS ${out_targets_var} CoordToMap 
		EXPORT ${PROJECT_NAME}-config
		DESTINATION ${CMAKE_INSTALL_LIBDIR}
		COMPONENT CoordToMap
		)

	install(DIRECTORY ${CMAKE_BINARY_DIR}/Esterv/CustomControls/CoordToMap
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/Esterv/CustomControls
		COMPONENT CoordToMap
		)

	install(TARGETS CoordToMapplugin
		EXPORT ${PROJECT_NAME}-config
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/Esterv/CustomControls/CoordToMap
		COMPONENT CoordToMap
		)

	install(EXPORT ${PROJECT_NAME}-config
		FILE ${PROJECT_NAME}-config.cmake
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Esterv
		NAMESPACE ${PROJECT_NAME}::
		COMPONENT All
		)
	include(CMakePackageConfigHelpers)
	configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
		"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
		INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Esterv
		)
	write_basic_package_version_file(
		"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
		VERSION ${VERSION}
		COMPATIBILITY SameMajorVersion
		)
	install(FILES
		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Esterv
		)
	export(EXPORT ${PROJECT_NAME}-config
		FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
		)
	if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
		add_subdirectory(examples)
		set(CPACK_PACKAGE_CONTACT "estervtech")
		set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-v${SEMVER}-${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_VERSION}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_CXX_COMPILER_ID}")
		include(CPack)
	endif()
endif()

if(BUILD_DOCS)
	get_target_property(build_docs cmake_build_docs SOURCES)
	include(${build_docs})
endif()
