#modular design example
include_directories(${PROJECT_SOURCE_DIR})
add_library(passive_membrane SHARED passive_membrane.cpp )
add_library(fixed_pulse SHARED fixed_pulse.cpp  )
add_library(basic_validator SHARED basic_validator.cpp  )
add_executable(pipeline main.cpp)
set_target_properties(pipeline PROPERTIES COMPILE_FLAGS "-Wall -Wextra -pedantic-errors")
target_link_libraries(pipeline neuromapp)
target_link_libraries(pipeline ${Boost_LIBRARIE})
