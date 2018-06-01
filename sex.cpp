/*
** sex.c: Sex main window stuff.
** Copyright (C) 1998, Nullsoft Inc.
** Free for noncommercial use.
** http://www.nullsoft.com/
**/

#include "stdafx.h"

#include "resource.h"

static int moved=0;
static int config_w = 300;
static int config_h = 200;
static int config_x = 50;
static int config_y = 50;
static int config_border = 4;
static COLORREF config_color = RGB(255, 255, 0);
static COLORREF config_bcolor1 = RGB(150, 150, 150);
static COLORREF config_bcolor2 = 0;

static const char app_name[] = "Sex";
static char text_file[MAX_PATH] = "";
static char ini_file[MAX_PATH] = "";

static void config_read();
static void read_text();
static void write_text();
static void config_write();

static HMENU hmenu_main;
static HWND hwnd_rich;
static HWND hwnd_main;

BOOL systray_add(HWND hwnd, UINT uID, HICON hIcon, LPCSTR lpszTip);
BOOL systray_del(HWND hwnd, UINT uID);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst,LPSTR lpszCmdParam, int nCmdShow);
static BOOL InitApplication(HINSTANCE hInstance);
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInst*/, LPSTR /*lpszCmdParam*/, int nCmdShow) 
{
	MSG msg;
	HACCEL hAccel = AtlLoadAccelerators(IDR_ACCELERATOR1);
	if (!LoadLibrary("RICHED32.DLL"))
	{
		MessageBox(nullptr, "Could not load RICHED32.DLL", nullptr, MB_OK);
		return (FALSE);
	}

	if (!InitApplication(hInstance)) 
	{
		MessageBox(nullptr, "Could not initialize application", nullptr, MB_OK);
		return (FALSE);
	}

	if (!InitInstance(hInstance, nCmdShow)) 
	{
		MessageBox(nullptr, "Could not create window", nullptr, MB_OK);
		return (FALSE);
	}

	// message loop
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(hwnd_main,hAccel,&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	} // while(GetMessage...
	
	return static_cast<int>(msg.wParam);
} // WinMain

static BOOL InitApplication(HINSTANCE hInstance)
{
	WNDCLASS wc;	
	wc.style = CS_DBLCLKS|CS_VREDRAW|CS_HREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = app_name;
	if (!RegisterClass(&wc)) { return FALSE; }
    return TRUE;
}

static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    const DWORD style = 0;
    const DWORD exStyle = WS_EX_TOOLWINDOW;
	HWND hwnd = CreateWindowEx(exStyle,app_name,app_name,style,0,0,1,1,NULL, NULL,hInstance,NULL);
	
	if (!hwnd) { return FALSE; }

    if (nCmdShow ==	SW_SHOWMAXIMIZED)
	{
	    ShowWindow(hwnd,SW_SHOWNORMAL);
	}
	else
	{
	    ShowWindow(hwnd,nCmdShow);
    }

    return TRUE;
}

static BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
static void OnDestroy(HWND hwnd);
static void OnClose(HWND hwnd);
static void OnActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized);
static UINT OnNCHitTest(HWND hwnd, int x, int y);
static void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void OnSize(HWND hwnd, UINT state, int cx, int cy);
static void OnMove(HWND hwnd, int x, int y);
static void OnPaint(HWND hwnd);

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CWindow wnd{hwnd};
	switch (uMsg) 
	{
		HANDLE_MSG(hwnd,WM_CREATE,OnCreate);
		HANDLE_MSG(hwnd,WM_DESTROY,OnDestroy);
		HANDLE_MSG(hwnd,WM_CLOSE,OnClose);
		HANDLE_MSG(hwnd,WM_PAINT,OnPaint);
		HANDLE_MSG(hwnd,WM_MOVE,OnMove);
		HANDLE_MSG(hwnd,WM_SIZE,OnSize);
		HANDLE_MSG(hwnd,WM_COMMAND,OnCommand);
		HANDLE_MSG(hwnd,WM_ACTIVATE,OnActivate);
		HANDLE_MSG(hwnd,WM_NCHITTEST,OnNCHitTest);
		case WM_USER:
			switch (LOWORD(lParam))
			{
				case WM_LBUTTONDOWN:
					SetForegroundWindow(hwnd);
				break;
				case WM_RBUTTONUP:
					{
					CPoint p;
					GetCursorPos(&p);
					SetForegroundWindow(hwnd);
					TrackPopupMenu(hmenu_main,TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,p.x,p.y,0,hwnd_main,NULL);
					}
				break;
			}
		return 0;
		default: break;
	}
	return (DefWindowProc(hwnd, uMsg, wParam, lParam));

}

static WNDPROC Rich_OldWndProc;

LRESULT CALLBACK Rich_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg==WM_RBUTTONUP)
	{
		CPoint p;
		GetCursorPos(&p);
		TrackPopupMenu(hmenu_main,TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,p.x,p.y,0,hwnd_main,NULL);
	}
	return CallWindowProc(Rich_OldWndProc,hwnd,uMsg,wParam,lParam);
}


static BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	HINSTANCE hInstance = lpCreateStruct->hInstance;
	hwnd_main=hwnd;
	hmenu_main=AtlLoadMenu(IDR_MENU1);
	hmenu_main=GetSubMenu(hmenu_main,0);
	config_read();
	CWindow wnd(hwnd);
	wnd.ModifyStyle(WS_CAPTION, 0);
	wnd.SetWindowPos(nullptr, 0,0, 0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_DRAWFRAME|SWP_NOACTIVATE);
	wnd.SetWindowPos(nullptr, config_x, config_y, config_w, config_h, SWP_NOACTIVATE|SWP_NOZORDER);
	systray_add(hwnd,1024,AtlLoadIcon(IDI_ICON1),app_name);
	hwnd_rich=CreateWindowEx(WS_EX_CLIENTEDGE,"RichEdit","",
		WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL|WS_HSCROLL|WS_VSCROLL,
		config_border,config_border,config_w-config_border*2,config_h-config_border*2,
		hwnd, NULL,hInstance,NULL);
	CWindow wnd_rich{hwnd_rich};
	Rich_OldWndProc = (WNDPROC) wnd_rich.GetWindowLongPtr(GWLP_WNDPROC);
	wnd_rich.SetWindowLongPtr(GWLP_WNDPROC,(LONG_PTR)Rich_WndProc);
	if (!hwnd_rich) 
	{
		wnd.MessageBox("Error creating RichEdit control","Error",MB_OK);
		return 0;
	}
	wnd_rich.SendMessage(EM_SETBKGNDCOLOR,FALSE,static_cast<LPARAM>(config_color));
	read_text();
	return 1;
}

static void OnDestroy(HWND hwnd)
{
	write_text();
	config_write();
	systray_del(hwnd,1024);
	PostQuitMessage(0);
}

static void OnClose(HWND hwnd)
{
	CWindow wnd{hwnd};
	wnd.SetWindowPos(HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_DRAWFRAME | SWP_NOACTIVATE);
//	DestroyWindow(hwnd);
}

static void OnActivate(HWND hwnd, UINT state, HWND /*hwndActDeact*/, BOOL /*fMinimized*/)
{
	if (state==WA_INACTIVE)
	{
		if (moved)
		{
			moved=0;
			config_write();
		}
		write_text();
		SetPriorityClass(GetWindowInstance(hwnd),IDLE_PRIORITY_CLASS);
	}
	else
	{
		SetPriorityClass(GetWindowInstance(hwnd),NORMAL_PRIORITY_CLASS);
	}
}

static UINT OnNCHitTest(HWND hwnd, int x, int y)
{
	CWindow wnd(hwnd);
	CRect r;
    (void)wnd.GetClientRect(&r);
	CPoint p(x, y);
	(void)wnd.ScreenToClient(&p);
	if (p.x <= config_border && p.y <= config_border*3) { return HTTOPLEFT; }
    if (p.x <= config_border*3 && p.y <= config_border) { return HTTOPLEFT; }
    if (p.x >= r.right-config_border && p.y >= r.bottom-config_border*3) { return HTBOTTOMRIGHT; }
    if (p.x >= r.right-config_border*3 && p.y >= r.bottom-config_border) { return HTBOTTOMRIGHT; }
    if (p.x >= r.right-config_border && p.y <= config_border*3) { return HTTOPRIGHT; }
    if (p.x >= r.right-config_border*3 && p.y <= config_border) { return HTTOPRIGHT; }
    if (p.x <= config_border && p.y >= r.bottom-config_border*3) { return HTBOTTOMLEFT; }
    if (p.x <= config_border*3 && p.y >= r.bottom-config_border) { return HTBOTTOMLEFT; }
    if (p.y <= config_border) { return HTCAPTION; }
    if (p.x <= config_border) { return HTLEFT; }
    if (p.y >= r.bottom-config_border) { return HTBOTTOM; }
    if (p.x >= r.right-config_border) { return HTRIGHT; }
    return HTCLIENT;
}

static void OnCommand(HWND hwnd, int id, HWND /*hwndCtl*/, UINT /*codeNotify*/)
{
	CWindow wnd{hwnd};
	CWindow wnd_rich{hwnd_rich};
	switch (id)
	{
		case IDM_ABOUT:
			wnd.MessageBox("Sex v0.1\n"\
				            "Copyright (C) 1998, Nullsoft Inc.","About sex",MB_OK);
		return;
		case IDM_CLOSE:
			wnd.SendMessage(WM_CLOSE,0,0);
		return;
		case IDM_QUIT:
			wnd.DestroyWindow();
		return;
		case IDM_FONT:
			{
				LOGFONT lf={0,};
				CHOOSEFONT cf={sizeof(cf),hwnd,0,&lf,0,
					CF_EFFECTS|CF_SCREENFONTS|CF_INITTOLOGFONTSTRUCT,
					0,};
				CHARFORMAT fmt={sizeof(fmt),};
				wnd_rich.SendMessage(EM_GETCHARFORMAT,1,(LPARAM)&fmt);
				if (fmt.dwMask & CFM_FACE)
				{
				    StringCchCopy(lf.lfFaceName, _countof(lf.lfFaceName), fmt.szFaceName);
				}
				else { lf.lfFaceName[0]=0; }
			    if (fmt.dwMask & CFM_SIZE)
				{
				    lf.lfHeight=fmt.yHeight/15;
				}
				else { lf.lfHeight=0; }
			    if (fmt.dwMask & CFM_COLOR)
			    {
			        cf.rgbColors=fmt.crTextColor;
			    }
			    else { cf.rgbColors=0xffffff; }
			    lf.lfItalic=static_cast<BYTE>((fmt.dwEffects&CFE_ITALIC)?1:0);
				lf.lfWeight=(fmt.dwEffects&CFE_BOLD)?FW_BOLD:FW_NORMAL;
				lf.lfUnderline=static_cast<BYTE>((fmt.dwEffects&CFE_UNDERLINE)?1:0);
				lf.lfStrikeOut=static_cast<BYTE>((fmt.dwEffects&CFE_STRIKEOUT)?1:0);
				lf.lfCharSet=DEFAULT_CHARSET;
				lf.lfOutPrecision=OUT_DEFAULT_PRECIS;
				lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;
				lf.lfQuality=DEFAULT_QUALITY;
				lf.lfPitchAndFamily=fmt.bPitchAndFamily;
				
				if (ChooseFont(&cf))
				{
					fmt.dwMask=CFM_BOLD|CFM_COLOR|CFM_ITALIC|CFM_STRIKEOUT|CFM_UNDERLINE;
					if (lf.lfFaceName[0]) { fmt.dwMask|=CFM_FACE; }
				    if (lf.lfHeight) { fmt.dwMask|=CFM_SIZE; }
				    fmt.dwEffects=0;
					if (lf.lfItalic) { fmt.dwEffects |= CFE_ITALIC; }
				    if (lf.lfUnderline) { fmt.dwEffects |= CFE_UNDERLINE; }
				    if (lf.lfStrikeOut) { fmt.dwEffects |= CFE_STRIKEOUT; }
				    if (lf.lfWeight!=FW_NORMAL) { fmt.dwEffects |= CFE_BOLD; }
				    fmt.yHeight = cf.iPointSize*2;
					fmt.crTextColor=cf.rgbColors;
					fmt.bPitchAndFamily=lf.lfPitchAndFamily;
					fmt.bCharSet = lf.lfCharSet;
					StringCchCopy(fmt.szFaceName, _countof(fmt.szFaceName), lf.lfFaceName);
					wnd_rich.SendMessage(EM_SETCHARFORMAT,SCF_SELECTION,(LPARAM)&fmt);
				}
			}
		return;
		case IDM_BGCOLOR:
			{
				static COLORREF custcolors[16];
				CHOOSECOLOR cs = {0};
				cs.lStructSize = sizeof(cs);
				cs.hwndOwner = hwnd;
				cs.hInstance = 0;
				cs.rgbResult=config_color;
				cs.lpCustColors = custcolors;
				cs.Flags = CC_RGBINIT|CC_FULLOPEN;
				if (ChooseColor(&cs))
				{
					config_color=cs.rgbResult;
					config_write();
					wnd_rich.SendMessage(EM_SETBKGNDCOLOR,FALSE,static_cast<LPARAM>(config_color));
				}
			}
		return;
	}
}

static void OnSize(HWND /*hwnd*/, UINT /*state*/, int cx, int cy)
{
	moved=1;
	SetWindowPos(hwnd_rich, 0, config_border,config_border, cx-config_border*2,cy-config_border*2, SWP_NOACTIVATE|SWP_NOZORDER);
}

static void OnMove(HWND /*hwnd*/, int /*x*/, int /*y*/)
{
	moved=1;
}

static void OnPaint(HWND hwnd)
{
	CRect r;
	CPaintDC hdc(hwnd);
	CWindow wnd(hwnd);
    (void)wnd.GetClientRect(&r);
	HPEN hPen=CreatePen(PS_SOLID,0,config_bcolor2);
	LOGBRUSH lb={BS_SOLID,config_bcolor1};
	HBRUSH hBrush=CreateBrushIndirect(&lb);

	HPEN hOldPen=hdc.SelectPen(hPen);
	HBRUSH hOldBrush=hdc.SelectBrush(hBrush);
	hdc.Rectangle(&r);
	hdc.SelectPen(hOldPen);
	hdc.SelectBrush(hOldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}


static int _r_i(char *name, int def)
{
	return static_cast<int>(GetPrivateProfileInt(app_name,name,def,ini_file));
}
#define RI(x) (( x ) = _r_i(#x,( x )))
static void _w_i(char *name, int d)
{
	char str[120];
	StringCchPrintf(str, _countof(str), "%d",d);
	WritePrivateProfileString(app_name,name,str,ini_file);
}
#define WI(x) _w_i(#x,( x ))

static void _r_s(char *name,char *data, DWORD mlen)
{
	char buf[2048];
	StringCchCopy(buf,_countof(buf), data);
	GetPrivateProfileString(app_name,name,buf,data,mlen,ini_file);
}
#define RS(x) (_r_s(#x,x,sizeof(x)))

static void _w_s(char *name, char *data)
{
	WritePrivateProfileString(app_name,name,data,ini_file);
}
#define WS(x) (_w_s(#x,x))



static void config_read()
{
	GetModuleFileName(GetModuleHandle(nullptr),ini_file,sizeof(ini_file));
	StringCchCopy(text_file, _countof(text_file), ini_file);
	PathRenameExtension(ini_file, ".ini");
	PathRenameExtension(text_file, ".rtf");
	RI(config_x);
	RI(config_y);
	RI(config_w);
	RI(config_h);
	RI(config_border);
	RI(config_color);
	RI(config_bcolor1);
	RI(config_bcolor2);
}

static HANDLE esFile;

DWORD CALLBACK esCb(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	if (dwCookie == 1) // write
	{
		WriteFile(esFile,pbBuff,(DWORD)cb,(DWORD*)pcb,NULL);
	}
	else // read
	{
		ReadFile(esFile,pbBuff,(DWORD)cb,(DWORD*)pcb,NULL);
	}
	return 0;
}
 


static void read_text()
{
	esFile=CreateFile(text_file,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (esFile != INVALID_HANDLE_VALUE)
	{
		EDITSTREAM es;
		es.dwCookie=0;
		es.pfnCallback=esCb;
		SendMessage(hwnd_rich,EM_STREAMIN,SF_RTF,(LPARAM) &es);
		CloseHandle(esFile);
	}
}

static void write_text()
{
	esFile=CreateFile(text_file,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if (esFile != INVALID_HANDLE_VALUE) 
	{
		EDITSTREAM es;
		es.dwCookie=1;
		es.pfnCallback=esCb;
		SendMessage(hwnd_rich,EM_STREAMOUT,SF_RTF,(LPARAM) &es);
		CloseHandle(esFile);
	} else { MessageBox(hwnd_main,"Error writing .rtf", "Error",0); }
}

static void config_write()
{
	CRect r;
	CWindow wnd(hwnd_main);
    (void)wnd.GetWindowRect(&r);
	config_x=r.left;
	config_y=r.top;
	config_w=r.right-r.left;
	config_h=r.bottom-r.top;
	WI(config_x);
	WI(config_y);
	WI(config_w);
	WI(config_h);
	WI(config_border);
	WI(config_color);
	WI(config_bcolor1);
	WI(config_bcolor2);
}
