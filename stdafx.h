#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#pragma warning (disable:4820) //padded structs
#pragma warning (disable:4710) //fn not inlined
#pragma warning (disable:4365) //type cast, signed/unsigned mismatch
#pragma warning (disable:4191) //unsafe conversion from FARPROC
#pragma warning (disable:4265) //class has virtual fns but dtor is not virtual
#pragma warning (disable:4625) //copy ctor could not be generated
#pragma warning (disable:4626) //assignment operator could not be generated
#pragma warning (disable:4302) //truncation from A to B
#include <windows.h>
#include <windowsx.h>
#include <richedit.h>
#include <stdlib.h>
#include <shellapi.h>
#include <commdlg.h>
#include <shlwapi.h>
#include <tchar.h>
#include <strsafe.h>
#include <atlbase.h>
#include <atltypes.h>
#include <atlwin.h>
#include <atlapp.h>
#pragma warning (default:4820)
#pragma warning (default:4365)
#pragma warning (default:4191)
#pragma warning (default:4265)
#pragma warning (default:4625)
#pragma warning (default:4626)
#pragma warning (default:4302)