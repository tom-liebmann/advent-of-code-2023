target_sources( ${TARGET_NAME} PRIVATE
    string_utils.cpp
    stream_utils.cpp
    math_utils.cpp
)

target_include_directories( ${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

find_package( fmt CONFIG REQUIRED )
target_link_libraries( ${TARGET_NAME} PUBLIC
    fmt::fmt
)
