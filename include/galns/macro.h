#ifndef _GALNS_MACRO_H_
#define _GALNS_MACRO_H_

// https://stackoverflow.com/a/8249232/22880994
#ifdef _WIN32
#   define GALNS_PLATFORM_WINDOWS
#elif defined(__linux__)
#   define GALNS_PLATFORM_LINUX
#elif defined(__APPLE__) || defined(__MACH__)
#   define GALNS_PLATFORM_MACOS // currently, this is not supported yet
#endif

// https://stackoverflow.com/a/32251412/22880994
#ifdef GALNS_API_EXPORT
#   ifdef GALNS_PLATFORM_WINDOWS
#       define GALNS_API __declspec(dllexport)
#   else
#       define GALNS_API
#   endif
#elif defined(GALNS_API_IMPORT)
#   ifdef GALNS_PLATFORM_WINDOWS
#       define GALNS_API __declspec(dllimport)
#   else
#       define GALNS_API
#   endif
#else
#   define GALNS_API
#endif

#endif