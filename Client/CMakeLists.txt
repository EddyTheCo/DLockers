

FetchContent_Declare(
        qrCode
        GIT_REPOSITORY https://github.com/EddyTheCo/qrCode.git
        GIT_TAG  v1.0.2
        FIND_PACKAGE_ARGS 1.0 CONFIG
        )
set(BUILD_QRDEC ON)
FetchContent_MakeAvailable(qrCode)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml Multimedia)

if(EMSCRIPTEN OR ANDROID)
	find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml Multimedia QuickControls2)
	find_package(EstervDesigns 1.1 REQUIRED COMPONENTS FlatControl CONFIG )
endif(EMSCRIPTEN OR ANDROID)

qt_standard_project_setup()
qt_add_executable(client main.cpp)


qt6_add_qml_module(client
	URI Esterv.DLockers.Client
	VERSION 1.0
	SOURCES
	"src/client.cpp"  "include/client.hpp"
	QML_FILES
	"qml/window.qml"
	"qml/ConfDrawer.qml"
	"qml/Enter_Pin_client.qml"
	"qml/BookDetailed.qml"
	RESOURCE_PREFIX
	"/esterVtech.com/imports"
	RESOURCES
	"qml/fonts/DeliciousHandrawn-Regular.ttf"
	IMPORT_PATH ${CMAKE_BINARY_DIR}
	)

target_include_directories(client PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

if(TARGET Qt6::Multimedia)
	target_link_libraries(client PRIVATE Qt6::Multimedia)
endif(TARGET Qt6::Multimedia)
if(EMSCRIPTEN OR ANDROID)
	target_link_libraries(client PRIVATE EstervDesigns::FlatControl Qt6::QuickControls2
		$<$<STREQUAL:$<TARGET_PROPERTY:EstervDesigns::FlatControl,TYPE>,STATIC_LIBRARY>:EstervDesigns::FlatControlplugin>
		)
	target_compile_definitions(client PRIVATE FORCE_STYLE="Esterv.Controls.Flat")

	if(ANDROID)
		set_property(TARGET client PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
		set_property(TARGET client APPEND PROPERTY QT_ANDROID_MIN_SDK_VERSION 30)
		set_property(TARGET client APPEND PROPERTY QT_ANDROID_TARGET_SDK_VERSION 34)
		set_property(TARGET client APPEND PROPERTY QT_ANDROID_SDK_BUILD_TOOLS_REVISION 34.0.0)

		FetchContent_Declare(
			android_openssl
			DOWNLOAD_EXTRACT_TIMESTAMP true
			URL      https://github.com/KDAB/android_openssl/archive/refs/heads/master.zip
			)
		FetchContent_GetProperties(android_openssl)
		if(NOT android_openssl_POPULATED)
			FetchContent_Populate(android_openssl)
			include(${android_openssl_SOURCE_DIR}/android_openssl.cmake)
			add_android_openssl_libraries(client)
		endif(NOT android_openssl_POPULATED)

	endif(ANDROID)
endif(EMSCRIPTEN OR ANDROID)

target_link_libraries(client PRIVATE Qt::Core Qt::Gui Qt::Qml Qt::Quick)

target_link_libraries(client PRIVATE IotaWallet::wallet qrCode::QtQrGen qrCode::QtQrDec booking_model
	$<$<STREQUAL:$<TARGET_PROPERTY:IotaWallet::wallet,TYPE>,STATIC_LIBRARY>:IotaWallet::walletplugin>
	$<$<STREQUAL:$<TARGET_PROPERTY:qrCode::QtQrGen,TYPE>,STATIC_LIBRARY>:qrCode::QtQrGenplugin>
	$<$<STREQUAL:$<TARGET_PROPERTY:qrCode::QtQrDec,TYPE>,STATIC_LIBRARY>:qrCode::QtQrDecplugin>
	$<$<STREQUAL:$<TARGET_PROPERTY:booking_model,TYPE>,STATIC_LIBRARY>:booking_modelplugin>
	)

set_target_properties(client PROPERTIES
	WIN32_EXECUTABLE ON
	MACOSX_BUNDLE ON
	)


install(TARGETS client
	BUNDLE  DESTINATION .
	DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	)
if(QTDEPLOY)
	qt_generate_deploy_qml_app_script(
		TARGET client
		OUTPUT_SCRIPT deploy_script
		)
	install(SCRIPT ${deploy_script})
endif(QTDEPLOY)



