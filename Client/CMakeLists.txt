

FetchContent_Declare(
    qrCode
    GIT_REPOSITORY https://github.com/EddyTheCo/qrCode.git
    GIT_TAG  v1.0.3
    FIND_PACKAGE_ARGS 1.0 CONFIG
)
set(BUILD_QRDEC ON)
FetchContent_MakeAvailable(qrCode)


if(EMSCRIPTEN)
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml)
else()
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml Multimedia Location Positioning)
endif()


if(EMSCRIPTEN OR ANDROID)
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml  QuickControls2)
    find_package(EstervDesigns 1.1 REQUIRED COMPONENTS FlatControl CONFIG )
endif(EMSCRIPTEN OR ANDROID)

qt_standard_project_setup()

add_subdirectory(ServerMap)

qt_add_executable(lockersClient main.cpp)


qt6_add_qml_module(lockersClient
    URI Esterv.DLockers.Client
    VERSION 1.0
    SOURCES
    "src/client.cpp"  "include/client.hpp"
    QML_FILES
    "qml/window.qml"
    "qml/AccountMenu.qml"
    "qml/ConfDrawer.qml"
    "qml/OpenBox.qml"
    "qml/ServerDetailed.qml"
    RESOURCE_PREFIX
    "/esterVtech.com/imports"
    RESOURCES
    "qml/fonts/DeliciousHandrawn-Regular.ttf"
    IMPORT_PATH ${CMAKE_BINARY_DIR}
)

target_include_directories(lockersClient PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

if(TARGET Qt6::Multimedia)
    target_link_libraries(lockersClient PRIVATE Qt6::Multimedia)
endif(TARGET Qt6::Multimedia)

if(EMSCRIPTEN OR ANDROID)
    target_link_libraries(lockersClient PRIVATE EstervDesigns::FlatControl Qt6::QuickControls2
        $<$<STREQUAL:$<TARGET_PROPERTY:EstervDesigns::FlatControl,TYPE>,STATIC_LIBRARY>:EstervDesigns::FlatControlplugin>
    )
target_compile_definitions(lockersClient PRIVATE FORCE_STYLE="Esterv.Controls.Flat")

if(ANDROID)
    set_property(TARGET lockersClient PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    set_property(TARGET lockersClient APPEND PROPERTY QT_ANDROID_MIN_SDK_VERSION 30)
    set_property(TARGET lockersClient APPEND PROPERTY QT_ANDROID_TARGET_SDK_VERSION 34)
    set_property(TARGET lockersClient APPEND PROPERTY QT_ANDROID_SDK_BUILD_TOOLS_REVISION 34.0.0)
endif(ANDROID)
endif(EMSCRIPTEN OR ANDROID)

target_link_libraries(lockersClient PRIVATE Qt::Core Qt::Gui Qt::Qml Qt::Quick)

target_link_libraries(lockersClient PRIVATE IotaWallet::wallet qrCode::QtQrGen qrCode::QtQrDec Bookings ServerMap
    $<$<STREQUAL:$<TARGET_PROPERTY:IotaWallet::wallet,TYPE>,STATIC_LIBRARY>:IotaWallet::walletplugin>
    $<$<STREQUAL:$<TARGET_PROPERTY:qrCode::QtQrGen,TYPE>,STATIC_LIBRARY>:qrCode::QtQrGenplugin>
    $<$<STREQUAL:$<TARGET_PROPERTY:qrCode::QtQrDec,TYPE>,STATIC_LIBRARY>:qrCode::QtQrDecplugin>
    $<$<STREQUAL:$<TARGET_PROPERTY:ServerMap,TYPE>,STATIC_LIBRARY>:ServerMapplugin>
    $<$<STREQUAL:$<TARGET_PROPERTY:Bookings,TYPE>,STATIC_LIBRARY>:Bookingsplugin>
)

set_target_properties(lockersClient PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)


install(TARGETS lockersClient
    BUNDLE  DESTINATION .
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
if(QTDEPLOY)
    qt_generate_deploy_qml_app_script(
        TARGET lockersClient
        OUTPUT_SCRIPT deploy_script
    )
install(SCRIPT ${deploy_script})
endif(QTDEPLOY)

if(EMSCRIPTEN)
    set(QTDEPLOY OFF)
    target_compile_definitions(lockersClient PRIVATE USE_EMSCRIPTEN)
    add_custom_command(
        TARGET lockersClient
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy ${CMAKE_CURRENT_BINARY_DIR}/lockersClient.js ${CMAKE_CURRENT_BINARY_DIR}/lockersClient.wasm ${CMAKE_CURRENT_BINARY_DIR}/qtloader.js  ${CMAKE_SOURCE_DIR}/wasm/Client
    )
endif(EMSCRIPTEN)



