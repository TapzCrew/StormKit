project(ConsoleLogger VERSION 0.1.0 LANGUAGES CXX)

add_executable(
    console_logger
    MACOSX_BUNDLE
    src/main.cpp
)

target_link_libraries(
    console_logger
    PUBLIC
        StormKit::core-static
        StormKit::log-static
)

enable_cpp17(TARGET console_logger)

if(NOT ${STORM_OS_MACOS} AND NOT ${STORM_OS_IOS})
    install(
        TARGETS console_logger
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
else()
    if(STORM_OS_IOS)
        message(${CMAKE_OSX_SYSROOT})
        target_link_directories(console_logger
            PUBLIC
        ${CMAKE_OSX_SYSROOT}/System/Library/)
        export_ios_bundle(
            TARGET
            console_logger
            INFOSTRING
            "StormKit Log Console example"
            COPYRIGHT
            "Arthapz 2019 - 2020"
            RESOURCES ""
        )
    endif()
endif()
