include_directories(
    ${VibeCore_INCLUDE_DIRS}
    ${VibeGui_INCLUDE_DIRS}
    ${VibeWidgets_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_definitions(${VibeCore_DEFINITIONS} ${VibeWidgets_DEFINITIONS})

set(SOURCES
    main.cpp
    archiverwindow.cpp
    archivemodel.cpp
)

set(UI_SOURCES
    archiverwindow.ui
)

qt5_wrap_ui(UI_FILES ${UI_SOURCES})

add_executable(archiver ${SOURCES} ${UI_FILES})
set_target_properties(archiver PROPERTIES COMPILE_FLAGS ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS})
target_link_libraries(archiver ${VibeGui_LIBRARIES} ${VibeWidgets_LIBRARIES})

install(FILES archiver.desktop DESTINATION ${CMAKE_INSTALL_APPSDIR})
install(TARGETS archiver DESTINATION ${CMAKE_INSTALL_PROGSDIR})
