

find_package(Qt6 COMPONENTS Core Gui Quick)

if(Qt6_FOUND)
	qt_standard_project_setup()
	qt6_add_qml_module(Bookings
		URI  Esterv.Dlockers.Bookings
		VERSION 1.0
		SOURCES
		"src/HourModel.cpp"
		"src/DayModel.cpp"
		"include/DayModel.hpp"
		"include/HourModel.hpp"
		"include/Booking.hpp"
		"src/Booking.cpp"
		QML_FILES
		"qml/DaySwipeView.qml"
		"qml/DayDelegate.qml"
		"qml/HourDelegate.qml"
		"qml/HourListView.qml"
		RESOURCE_PREFIX
		"/esterVtech.com/imports"
		OUTPUT_TARGETS out_targets_var
		OUTPUT_DIRECTORY
		${CMAKE_BINARY_DIR}/Esterv/Dlockers/Bookings
		IMPORT_PATH ${CMAKE_BINARY_DIR}
		)

	add_library(${PROJECT_NAME}::Bookings ALIAS Bookings)
	add_library(${PROJECT_NAME}::Bookingsplugin ALIAS Bookingsplugin)

	target_compile_definitions(Bookings PRIVATE WINDOWS_BOOKI)
	target_link_libraries(Bookings PUBLIC Qt6::Gui Qt6::Quick)

	target_link_libraries(Bookings PUBLIC EstervDesigns::SimpleStyle
		$<$<STREQUAL:$<TARGET_PROPERTY:EstervDesigns::SimpleStyle,TYPE>,STATIC_LIBRARY>:EstervDesigns::SimpleStyleplugin>
		)

	target_include_directories(Bookings PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Esterv/${PROJECT_NAME}>")

	install(TARGETS Bookings ${out_targets_var}
		EXPORT ${PROJECT_NAME}-config
		DESTINATION ${CMAKE_INSTALL_LIBDIR}
		COMPONENT Bookings
		)
	install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Esterv/${PROJECT_NAME}
		COMPONENT Bookings
		)
	install(DIRECTORY ${CMAKE_BINARY_DIR}/Esterv/Dlockers/Bookings
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/Esterv/Dlockers
		COMPONENT Bookings
		)
	install(TARGETS Bookingsplugin
		EXPORT ${PROJECT_NAME}-config
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/Esterv/Esterv/Dlockers
		COMPONENT Bookings
		)

endif(Qt6_FOUND)


