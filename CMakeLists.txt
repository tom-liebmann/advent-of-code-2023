cmake_minimum_required( VERSION 3.10 )

set( CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_BINARY_DIR}/conan_toolchain.cmake" )

cmake_policy( SET CMP0076 NEW )
cmake_policy( SET CMP0079 NEW )

project( advent_of_code )

set( solutions_dir "${CMAKE_CURRENT_LIST_DIR}/solutions" )

file( GLOB solution_names
    LIST_DIRECTORIES true
    RELATIVE "${solutions_dir}"
    "${solutions_dir}/*"
)

foreach( solution_name ${solution_names} )
    set( solution_dir "${solutions_dir}/${solution_name}" )

    if( IS_DIRECTORY ${solution_dir} )
        message( STATUS "Addind executable for solution ${solution_name}" )

        set( TARGET_NAME "run_${solution_name}" )
        add_executable( ${TARGET_NAME} )
        add_subdirectory( "${solution_dir}" )
    endif()
endforeach()



