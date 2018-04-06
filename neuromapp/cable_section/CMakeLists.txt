# Use -DNEUROMAPP_DISABLE_CORENEURON=TRUE to disable compilation/installation of this mini-app
# Other mini-apps depend on some parts of these mini-apps, enabling them by now until
# mini-apps are decoupled
if(NOT NEUROMAPP_DISABLE_CABLE_SECTION)
    include_directories(${PROJECT_BINARY_DIR})
    include_directories(${PROJECT_SOURCE_DIR})

    add_library (cable_section_solver STATIC
                 module.cpp)

    install (TARGETS cable_section_solver DESTINATION lib)

    install (FILES  cable_module.h DESTINATION include)

    set(NEUROMAPP_CABLE_SECTION_MODULE ON CACHE BOOL "True if cable section mini-apps will be installed")
else()
    set(NEUROMAPP_CABLE_SECTION_MODULE OFF CACHE BOOL "True if cable section mini-apps will be installed")
endif()

