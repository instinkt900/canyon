#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
  #if defined(CANYON_BUILD_SHARED)
    #if defined(CANYON_BUILD)
      #define CANYON_API __declspec(dllexport)
    #else
      #define CANYON_API __declspec(dllimport)
    #endif
  #else
    #define CANYON_API
  #endif
#else
  #if defined(CANYON_BUILD_SHARED)
    #if defined(CANYON_BUILD)
      #define CANYON_API __attribute__((visibility("default")))
    #else
      #define CANYON_API
    #endif
  #else
    #define CANYON_API
  #endif
#endif

