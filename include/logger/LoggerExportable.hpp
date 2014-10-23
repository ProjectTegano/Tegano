#ifndef __WFSHARED_EXPORTABLE_H
#define __WFSHARED_EXPORTABLE_H

#ifndef _WIN32

#define WFSINGLETON_DLL_VISIBLE

#else

#ifdef WFSHARED_DLL_IMPLEMENTATION
#define WFSINGLETON_DLL_VISIBLE __declspec(dllexport)
#else
#define WFSINGLETON_DLL_VISIBLE __declspec(dllimport)
#endif

#endif

#endif
