cmake_minimum_required( VERSION 3.10 )

set( CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_BINARY_DIR}/conan_toolchain.cmake" )

cmake_policy( SET CMP0076 NEW )
cmake_policy( SET CMP0079 NEW )

project( advent_of_code )

include( CTest )

set( solutions_dir "${CMAKE_CURRENT_LIST_DIR}/solutions" )

file( GLOB solution_names
    LIST_DIRECTORIES true
    RELATIVE "${solutions_dir}"
    "${solutions_dir}/*"
)

# Build utility library
set( TARGET_NAME "utils" )
add_library( ${TARGET_NAME} STATIC )
target_compile_features( ${TARGET_NAME} PRIVATE cxx_std_23 )
add_subdirectory( "${CMAKE_CURRENT_LIST_DIR}/utils" )

foreach( solution_name ${solution_names} )
    set( solution_dir "${solutions_dir}/${solution_name}" )

    if( IS_DIRECTORY ${solution_dir} )
        message( STATUS "Adding executable for solution ${solution_name}" )

        set( TARGET_NAME "run_${solution_name}" )
        add_executable( ${TARGET_NAME} )
        target_compile_features( ${TARGET_NAME} PRIVATE cxx_std_23 )
        target_sources( ${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/utils/application.cpp" )
        add_subdirectory( "${solution_dir}" )

        find_package( OpenMP )
        target_link_libraries( ${TARGET_NAME} PRIVATE
            utils
            OpenMP::OpenMP_CXX
        )

        set( TARGET_NAME "test_${solution_name}" )
        add_executable( ${TARGET_NAME} )
        target_compile_features( ${TARGET_NAME} PRIVATE cxx_std_23 )
        target_sources( ${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/utils/application_test.cpp" )
        add_subdirectory( "${solution_dir}" "build_test_${solution_name}" )

        find_package( OpenMP )
        target_link_libraries( ${TARGET_NAME} PRIVATE
            utils
            OpenMP::OpenMP_CXX
        )

        add_test( NAME ${TARGET_NAME} COMMAND ${TARGET_NAME} )
    endif()
endforeach()



