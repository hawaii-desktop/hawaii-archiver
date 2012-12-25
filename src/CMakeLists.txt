include_directories(
    ${VibeCore_INCLUDE_DIRS}
    ${VibeGui_INCLUDE_DIRS}
    ${VibeWidgets_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_definitions(
    ${VibeCore_DEFINITIONS}
    ${VibeWidgets_DEFINITIONS}
)

set(SOURCES
    main.cpp
    archiverwindow.cpp
    archivemodel.cpp
)

set(UI_SOURCES
    archiverwindow.ui
)

set(QM_SOURCES
    translations/it.ts
)

qt5_wrap_ui(UI_FILES ${UI_SOURCES})
qt5_add_translation(QM_FILES ${QM_SOURCES})

add_executable(hawaii-archiver ${SOURCES} ${UI_FILES} ${QM_FILES})
qt5_use_modules(hawaii-archiver Widgets)
target_link_libraries(hawaii-archiver ${VibeWidgets_LIBRARIES})

install(TARGETS hawaii-archiver DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_DATADIR}/hawaii-archiver/translations)
