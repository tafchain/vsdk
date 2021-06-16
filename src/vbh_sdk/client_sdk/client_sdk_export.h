#ifndef CLIENT_SDK_EXPORT_H_43280831943276432765326576
#define CLIENT_SDK_EXPORT_H_43280831943276432765326576

#ifdef WIN32
	#ifdef DECLARE_VBH_CLIENT_SDK_EXPORT
		#define VBH_CLIENT_SDK_EXPORT __declspec(dllexport)
	#else
		#define VBH_CLIENT_SDK_EXPORT __declspec(dllimport)
	#endif
#else
	 #ifdef __GNUC__
		#define VBH_CLIENT_SDK_EXPORT __attribute__ ((visibility("default")))
	#else
		#define VBH_CLIENT_SDK_EXPORT
	#endif
#endif

#endif
