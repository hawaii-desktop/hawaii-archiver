include_directories(
    ${KArchive_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
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

install(TARGETS hawaii-archiver DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_DATADIR}/hawaii-archiver/translations)
