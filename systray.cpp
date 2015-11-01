/*
** systray.c: Sex system tray icon support
** Copyright (C) 1998, Nullsoft Inc.
** Free for noncommercial use.
** http://www.nullsoft.com/
**/
#include "stdafx.h"

BOOL systray_add(HWND hwnd, UINT uID, HICON hIcon, LPCSTR lpszTip);
BOOL systray_del(HWND hwnd, UINT uID);

BOOL systray_add(HWND hwnd, UINT uID, HICON hIcon, LPCSTR lpszTip)
{
  NOTIFYICONDATA tnid = {0};
  tnid.cbSize = sizeof(NOTIFYICONDATA);
  tnid.hWnd = hwnd;
  tnid.uID = uID;
  tnid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
  tnid.uCallbackMessage = WM_USER;
  tnid.hIcon = hIcon;
  StringCchCopy(tnid.szTip, _countof(tnid.szTip), lpszTip);
  return (Shell_NotifyIcon(NIM_ADD, &tnid));
}

BOOL systray_del(HWND hwnd, UINT uID) {
  NOTIFYICONDATA tnid = {0};
  tnid.cbSize = sizeof(NOTIFYICONDATA);
  tnid.hWnd = hwnd;
  tnid.uID = uID;
  return(Shell_NotifyIcon(NIM_DELETE, &tnid));
}
