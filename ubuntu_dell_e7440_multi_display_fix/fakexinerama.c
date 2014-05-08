#ifdef notdef
    set -x
    exec cc -O2 -std=c99 -pedantic -Wall $0 -fPIC -o libXinerama.so -shared
#endif
#define _XOPEN_SOURCE 600
#include <X11/Xlibint.h>
#include <X11/extensions/Xinerama.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <dlfcn.h>

typedef XineramaScreenInfo* (queryfn)(Display*, int*);
typedef Status (versionfn)(Display*, int*, int*);
typedef Bool (extensionfn)(Display*, int*, int*);
typedef Bool (activefn)(Display*);

static void*         libxinerama;
static queryfn*      queryscreens;
static extensionfn*  queryextension;
static versionfn*    queryversion;
static activefn*     isactive;

static void init() {
	libxinerama       = dlopen("/usr/lib/x86_64-linux-gnu/libXinerama.so.1", RTLD_GLOBAL | RTLD_LAZY);
	if(!libxinerama)
		abort();
	isactive          = (activefn*)    (uintptr_t)dlsym(libxinerama, "XineramaIsActive");
	queryversion      = (versionfn*)   (uintptr_t)dlsym(libxinerama, "XineramaQueryVersion");
	queryextension    = (extensionfn*) (uintptr_t)dlsym(libxinerama, "XineramaQueryExtension");
	queryscreens      = (queryfn*)     (uintptr_t)dlsym(libxinerama, "XineramaQueryScreens");
}

Bool XineramaQueryExtension(Display *dpy, int *event_base, int *error_base) {
	init();
	return queryextension(dpy, event_base, error_base);
}

Status XineramaQueryVersion(Display *dpy, int *major, int *minor) {
	init();
	return queryversion(dpy, major, minor);
}

Bool XineramaIsActive(Display *dpy) {
	init();
	return isactive(dpy);
}

XineramaScreenInfo* XineramaQueryScreens(Display *dpy, int *number)
{
    XineramaScreenInfo *ret, *work;

	init();

	ret = queryscreens(dpy, number);

	if(*number == 2 && ret[1].width == 3840) {
		work = Xmalloc(3 * sizeof(XineramaScreenInfo));
		memcpy(work, ret, sizeof(XineramaScreenInfo));
		work[1].screen_number = 1;
		work[1].x_org = 1920;
		work[1].y_org = 0;
		work[1].width = 3840/2;
		work[1].height = 1080;
		work[2].screen_number = 2;
		work[2].x_org = 1920 + 3840/2;
		work[2].y_org = 0;
		work[2].width = 3840/2;
		work[2].height = 1080;
		*number = 3;
		XFree(ret);
		ret = work;
	}

    return ret;
}
