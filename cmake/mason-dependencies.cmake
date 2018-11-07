# All dependencies in a single separate file so its checksum can be used in a CI cache key name

mason_use(cheap-ruler VERSION 2.5.3 HEADER_ONLY)
mason_use(pixelmatch VERSION 0.10.0 HEADER_ONLY)

if(MBGL_PLATFORM STREQUAL "android")
    mason_use(jni.hpp VERSION 4.0.1 HEADER_ONLY)
elseif(MBGL_PLATFORM STREQUAL "ios")
    # noop
elseif(MBGL_PLATFORM STREQUAL "linux")
    mason_use(glfw VERSION 2018-06-27-0be4f3f)
    mason_use(libuv VERSION 1.9.1)
    mason_use(libpng VERSION 1.6.25)
    mason_use(libjpeg-turbo VERSION 1.5.0)
    mason_use(args VERSION 6.2.0 HEADER_ONLY)

    if(WITH_EGL)
        mason_use(swiftshader VERSION 2018-05-31)
    endif()
elseif(MBGL_PLATFORM STREQUAL "macos")
    mason_use(glfw VERSION 2018-06-27-0be4f3f)
    mason_use(args VERSION 6.2.0 HEADER_ONLY)

    if(WITH_EGL)
        mason_use(swiftshader VERSION 2018-05-31)
    endif()
elseif(MBGL_PLATFORM STREQUAL "qt")
    if(NOT WITH_QT_DECODERS)
        mason_use(libjpeg-turbo VERSION 1.5.0)
        mason_use(libpng VERSION 1.6.25)
    endif()
endif()
