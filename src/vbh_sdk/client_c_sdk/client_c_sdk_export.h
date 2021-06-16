#ifndef CLIENT_C_SDK_EXPORT_H_328928987643289
#define CLIENT_C_SDK_EXPORT_H_328928987643289

#ifdef WIN32
#ifdef DECLARE_VBH_C_CLIENT_SDK_EXPORT
#define VBH_CLIENT_C_SDK_EXPORT __declspec(dllexport)
#else
#define VBH_CLIENT_C_SDK_EXPORT __declspec(dllimport)
#endif
#else
#ifdef __GNUC__
#define VBH_CLIENT_C_SDK_EXPORT __attribute__ ((visibility("default")))
#else
#define VBH_CLIENT_C_SDK_EXPORT
#endif
#endif

#endif
