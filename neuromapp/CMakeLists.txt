add_subdirectory (utils)
option(NEUROMAPP_COMPRESSION "Build the compression miniapp" ON)
if(NEUROMAPP_COMPRESSION)
    find_package(ZLIB REQUIRED) 
    add_subdirectory (compression)
endif()
add_subdirectory (hello)
add_subdirectory (nest)
add_subdirectory (hdf5)
add_subdirectory (keyvalue)
add_subdirectory (coreneuron_1.0)
add_subdirectory (replib)
add_subdirectory (iobench)
add_subdirectory (app)
