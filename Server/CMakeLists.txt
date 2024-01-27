
FetchContent_Declare(
        qrCode
        GIT_REPOSITORY https://github.com/EddyTheCo/qrCode.git
        GIT_TAG  improvements
        FIND_PACKAGE_ARGS 1.0 CONFIG
        )
set(BUILD_QRDEC OFF)
FetchContent_MakeAvailable(qrCode)

FetchContent_Declare(
	OMQml
	GIT_REPOSITORY https://github.com/EddyTheCo/OMClient.git
        GIT_TAG v0.2.0
	FIND_PACKAGE_ARGS 0.1 CONFIG
	)
FetchContent_MakeAvailable(OMQml)
FetchContent_Declare(
	lgpio
	GIT_REPOSITORY https://github.com/EddyTheCo/lg.git
	GIT_TAG v0.0.1
	FIND_PACKAGE_ARGS 0 CONFIG
	)
FetchContent_MakeAvailable(lgpio)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml  OPTIONAL_COMPONENTS Positioning Serialport)

qt_standard_project_setup()


qt_add_executable(lockersServer main.cpp)
target_link_libraries(lockersServer PRIVATE lgpio)
target_compile_definitions(lockersServer PRIVATE RPI_SERVER
	SERIAL_PORT_NAME=$<IF:$<BOOL:${RPI_SERIAL_PN}>,${RPI_SERIAL_PN},"serial:/dev/ttyUSB1">
	GPIO_NUMBER=$<IF:$<BOOL:${RPI_SERVER_GPIO}>,${RPI_SERVER_GPIO},17>
	)

if(NOT TARGET Qt6::Positioning)
	FetchContent_Declare(
		qtpositioning
		GIT_REPOSITORY git://code.qt.io/qt/qtpositioning.git
		GIT_TAG 6.6
		)
	FetchContent_MakeAvailable(qtpositioning)
endif(NOT TARGET Qt6::Positioning)

target_link_libraries(lockersServer PRIVATE Qt6::Positioning )

qt6_add_qml_module(lockersServer
	URI Esterv.DLockers.Server
	VERSION 1.0
	SOURCES
	"src/server.cpp"
      	"include/server.hpp"
	QML_FILES
	"qml/window.qml"
        "qml/ConfDrawer.qml"
        "qml/OpenBox.qml"
	RESOURCE_PREFIX
	"/esterVtech.com/imports"
	RESOURCES
	"qml/fonts/DeliciousHandrawn-Regular.ttf"
	IMPORT_PATH ${CMAKE_BINARY_DIR}
	)

target_link_libraries(lockersServer PRIVATE IotaWallet::wallet qrCode::QtQrGen Bookings
	$<$<STREQUAL:$<TARGET_PROPERTY:IotaWallet::wallet,TYPE>,STATIC_LIBRARY>:IotaWallet::walletplugin>
	$<$<STREQUAL:$<TARGET_PROPERTY:qrCode::QtQrGen,TYPE>,STATIC_LIBRARY>:qrCode::QtQrGenplugin>
        $<$<STREQUAL:$<TARGET_PROPERTY:Bookings,TYPE>,STATIC_LIBRARY>:Bookingsplugin>
	)
target_link_libraries(lockersServer PRIVATE Qt::Core Qt::Gui Qt::Qml Qt::Quick)


target_include_directories(lockersServer PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

set_target_properties(lockersServer PROPERTIES
	WIN32_EXECUTABLE ON
	MACOSX_BUNDLE ON
	)


install(TARGETS lockersServer
	BUNDLE  DESTINATION .
	DESTINATION ${CMAKE_INSTALL_BINDIR}
	)

if(QTDEPLOY)
	qt_generate_deploy_qml_app_script(
                TARGET lockersServer
		OUTPUT_SCRIPT deploy_script
		)
	install(SCRIPT ${deploy_script})
endif(QTDEPLOY)

