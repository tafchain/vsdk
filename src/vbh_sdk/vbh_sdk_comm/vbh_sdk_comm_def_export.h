#ifndef VBH_SDK_COMM_DEF_EXPORT_H_487974946464654165132132
#define VBH_SDK_COMM_DEF_EXPORT_H_487974946464654165132132

#ifdef WIN32
	#ifdef DECLARE_VBH_SDK_COMM_DEF_EXPORT
		#define VBH_SDK_COMM_DEF_EXPORT __declspec(dllexport)
	#else
		#define VBH_SDK_COMM_DEF_EXPORT __declspec(dllimport)
	#endif
#else
	#if defined(__GNUC__) && defined(DECLARE_VBH_SDK_COMM_DEF_EXPORT)
		#define VBH_SDK_COMM_DEF_EXPORT __attribute__ ((visibility("default")))
	#else
		#define VBH_SDK_COMM_DEF_EXPORT
	#endif
#endif

#endif
