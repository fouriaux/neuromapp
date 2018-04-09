# Use -DNEUROMAPP_DISABLE_CORENEURON=TRUE to disable compilation/installation of this mini-app
# Other mini-apps depend on some parts of these mini-apps, enabling them by now until
# mini-apps are decoupled
if(NOT NEUROMAPP_DISABLE_CONSTANT_CABLE)
    include_directories(${PROJECT_BINARY_DIR})
    include_directories(${PROJECT_SOURCE_DIR})

    add_library (constant_cable MODULE
                 module.cpp)

    install (TARGETS constant_cable DESTINATION lib)

    set(NEUROMAPP_CONSTANT_CABLE_MODULE ON CACHE BOOL "True if cable section mini-apps will be installed")
else()
    set(NEUROMAPP_CONSTANT_CABLE_MODULE OFF CACHE BOOL "True if cable section mini-apps will be installed")
endif()

