// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SPLINE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SPLINE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SPLINE_EXPORTS
#define SPLINE_API extern "C" __declspec(dllexport)
#else
#define SPLINE_API extern "C" __declspec(dllimport)
#endif

struct spline_points {
	double* ts;
	double* xs;
	double* ys;
	uint32_t n;

	void* private_data;
};

SPLINE_API void spline_interp(double step,
                              double d1_x,
                              double d1_y,
                              double dn_x,
                              double dn_y,
                              double* ts,
                              double* xs,
                              double* ys,
                              uint32_t n,
                              struct spline_points* result);

SPLINE_API void spline_free(struct spline_points* points);
