# Use -DNEUROMAPP_DISABLE_HELLO=TRUE to disable compilation/installation of this mini-app
if(NOT NEUROMAPP_DISABLE_HH_MODELS)
    add_library (hh_fe forwardeuler.cpp)
    add_library (hh_cn cranknicholson.cpp)
    add_library (hh_afe adaptiveforwardeuler.cpp)

    install (TARGETS hh_fe DESTINATION lib)
    install (TARGETS hh_cn DESTINATION lib)
    install (TARGETS hh_afe DESTINATION lib)
    install (FILES hh_models.h DESTINATION include)

    set(NEUROMAPP_HH_MODELS_MAPP ON CACHE BOOL "True if hh_models mini-apps will be installed")
else()
    set(NEUROMAPP_HH_MODELS_MAPP OFF CACHE BOOL "True if hh_models mini-app will be installed")
endif()
