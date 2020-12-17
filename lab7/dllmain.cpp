// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include "BigBrotherLibrary.h"
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <ctype.h>

using namespace std;

HINSTANCE hinstDLL;
HHOOK g_Hook;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	hinstDLL = (HINSTANCE)hModule;
	return TRUE;
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
	// MOUSEHOOKSTRUCT ms;
	char str[64] = { '\0' };
	if (nCode < 0) return CallNextHookEx(g_Hook, nCode, wParam, lParam);
	if (nCode == HC_ACTION)
		switch (wParam) {
		case WM_LBUTTONDBLCLK:
			sprintf_s(str, "%i %i", ((MOUSEHOOKSTRUCT*)lParam)->pt.x, ((MOUSEHOOKSTRUCT*)lParam)->pt.y);
			MessageBox(NULL, "L_CLICK", str, MB_OK);
			break;
		case WM_RBUTTONDBLCLK:
			sprintf_s(str, "%i %i", ((MOUSEHOOKSTRUCT*)lParam)->pt.x, ((MOUSEHOOKSTRUCT*)lParam)->pt.y);
			MessageBox(NULL, "R_CLICK", str, MB_OK);
			break;
		}

	return CallNextHookEx(g_Hook, nCode, wParam, lParam);
}

LRESULT CALLBACK LLKeyProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	char str[100] = { '\0' };
	
	if (nCode == HC_ACTION) {
		Beep(440, 50);
	}

	//if (nCode == HC_ACTION) {
	//	// switch (wParam) {
	//	//case VK_SPACE:
	//	sprintf_s(str, "ncode: %d wparam: %c", nCode, wParam);
	//	MessageBox(NULL, str, "KEYBOARD_CLICK", MB_OK);
	//	if ((wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN)) {
	//		sprintf_s(str, "ncode: %d wparam: %c", nCode, wParam);
	//		MessageBox(NULL, str, "KEYBOARD_CLICK", MB_OK);
	//	}
	//	else if ((wParam == WM_KEYUP) || (wParam == WM_SYSKEYUP)) {
	//		sprintf_s(str, "ncode: %d wparam: %c", nCode, wParam);
	//		MessageBox(NULL, str, "KEYBOARD_CLICK", MB_OK);
	//	}
	//		//break;
	//	//}


	//}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}


void SetFarHook(void) {
	/*Функция SetWindowsHookEx устанавливает определяемую программой процедуру фильтра (hook) 
	в цепочку фильтров (hook). Вы должны установить процедуру фильтра (hook) для того, чтобы 
	контролировать в системе определенного рода типы событий. Эти события связаны или с конкретным  
	потоком или со всеми потоками одного и того же рабочего стола, что и вызывающий поток.*/

	g_Hook = SetWindowsHookEx(WH_MOUSE, MouseProc, hinstDLL, 0);

	g_Hook = SetWindowsHookEx(WH_KEYBOARD, LLKeyProc, hinstDLL, 0);

	return;
}
void UninstallFarHook(void) {
	UnhookWindowsHookEx(g_Hook);
	return;
}
