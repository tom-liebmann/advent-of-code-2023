target_sources( ${TARGET_NAME} PRIVATE
    main.cpp
)

find_package( fmt CONFIG REQUIRED )
target_link_libraries( ${TARGET_NAME} PRIVATE
    fmt::fmt
)
