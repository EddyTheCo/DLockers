if(EMSCRIPTEN)
    set(map_sources     "src/wasmmap.cpp" "include/wasmmap.hpp")
else()
    set(map_sources     "src/objtomap.cpp" "include/objtomap.hpp")
    set(map_qml_files         "qml/ObjPopUp.qml" "qml/ObjDelegate.qml" "qml/ObjectMapView.qml")
    set(map_resources         "qml/icons/safety-box.png")
endif()

qt6_add_qml_module(ServerMap
    URI  Esterv.Dlockers.Map
    VERSION 1.0
    SOURCES ${map_sources}
    QML_FILES ${map_qml_files}
    RESOURCE_PREFIX
    "/esterVtech.com/imports"
    RESOURCES ${map_resources}
    OUTPUT_TARGETS out_targets_var
    OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/Esterv/Dlockers/Map
    IMPORT_PATH ${CMAKE_BINARY_DIR}
)

add_library(${PROJECT_NAME}::ServerMap ALIAS ServerMap)
add_library(${PROJECT_NAME}::ServerMapplugin ALIAS ServerMapplugin)

target_include_directories(ServerMap PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Esterv/${PROJECT_NAME}>")

if(EMSCRIPTEN)
    target_link_libraries(ServerMap PUBLIC Qt6::Gui Qt6::Quick Qt6::Core )
else()
    target_link_libraries(ServerMap PUBLIC Qt6::Gui Qt6::Quick Qt6::Core Qt6::Positioning  Qt6::Location EstervDesigns::SimpleStyle
        $<$<STREQUAL:$<TARGET_PROPERTY:EstervDesigns::SimpleStyle,TYPE>,STATIC_LIBRARY>:EstervDesigns::SimpleStyleplugin>
    )
endif()


install(TARGETS ${out_targets_var} ServerMap
    EXPORT ${PROJECT_NAME}-config
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT ServerMap
)

install(DIRECTORY ${CMAKE_BINARY_DIR}/Esterv/Dlockers/Map
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/Esterv/Dlockers
    COMPONENT ServerMap
)

install(TARGETS ServerMapplugin
    EXPORT ${PROJECT_NAME}-config
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/Esterv/Dlockers/Map
    COMPONENT ServerMap
)


