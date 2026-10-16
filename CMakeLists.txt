cmake_minimum_required(VERSION 3.20)
project(cursepp VERSION 0.2.1 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(cursepp INTERFACE)
add_library(cursepp::cursepp ALIAS cursepp)
target_compile_features(cursepp INTERFACE cxx_std_20)
target_include_directories(cursepp INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Avoid building examples/tests when CURSE++ is a dependency via add_subdirectory.
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(CURSEPP_DEFAULT_BUILD ON)
else()
    set(CURSEPP_DEFAULT_BUILD OFF)
endif()
option(CURSEPP_BUILD_EXAMPLES "Build the cursed FizzBuzz example" ${CURSEPP_DEFAULT_BUILD})
option(CURSEPP_BUILD_TESTS "Build CURSE++ tests" ${CURSEPP_DEFAULT_BUILD})

if(CURSEPP_BUILD_EXAMPLES)
    add_executable(cursepp_fizzbuzz examples/fizzbuzz.cpp)
    target_link_libraries(cursepp_fizzbuzz PRIVATE cursepp::cursepp)
endif()

if(CURSEPP_BUILD_TESTS)
    enable_testing()
    add_executable(cursepp_tests tests/test_cursepp.cpp tests/odr_probe.cpp)
    target_link_libraries(cursepp_tests PRIVATE cursepp::cursepp)
    add_test(NAME cursepp_tests COMMAND cursepp_tests)
endif()

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS cursepp EXPORT curseppTargets)
install(EXPORT curseppTargets NAMESPACE cursepp::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cursepp)

configure_package_config_file(cmake/curseppConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/curseppConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/cursepp")
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/curseppConfigVersion.cmake"
    VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/curseppConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/curseppConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/cursepp")

# Ship the MIT license alongside installed package files.
install(FILES LICENSE DESTINATION "${CMAKE_INSTALL_DATADIR}/licenses/cursepp")
