project(FileLogger VERSION 0.1.0 LANGUAGES C CXX)

add_executable(
    file_logger
    MACOSX_BUNDLE
    src/main.cpp
)

target_link_libraries(
    file_logger
    PUBLIC
        StormKit::core-static
        StormKit::log-static
)

enable_cpp17(TARGET file_logger)

if(NOT ${STORM_OS_MACOS} AND NOT ${STORM_OS_IOS})
    install(
        TARGETS file_logger
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
else()
    if(STORM_OS_IOS)
        export_ios_bundle(
            TARGET
            file_logger
            INFOSTRING
            "StormKit Log File example"
            COPYRIGHT
            "Arthapz 2019 - 2020"
            RESOURCES ""
        )
    endif()
endif()
