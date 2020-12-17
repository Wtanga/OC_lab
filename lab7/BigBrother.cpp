// BigBrother.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "BigBrother.h"
#include "BigBrotherLibrary.h"

LRESULT CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	DialogBox(hInstance, (LPCTSTR)IDD_DLGTEST, NULL, (DLGPROC)DlgProc);
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


	return msg.wParam;


}

LRESULT CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	// char strText[100];
	switch (message) {
	case WM_INITDIALOG: RegisterHotKey(hDlg, 0xB001, MOD_CONTROL | MOD_ALT, 'W'); return TRUE;
	case WM_HOTKEY: ShowWindow(hDlg, SW_SHOW); break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			PostQuitMessage(0);
			return TRUE;
		case IDC_BTN1:
			SetFarHook();
			// MessageBox(NULL, "HOOK\0", "", MB_OK);
			// ShowWindow(hDlg, SW_HIDE);
			break;
		case IDC_BTN2:
			UninstallFarHook();
			break;
		} break;
	default:
		return FALSE;
	}
	return TRUE;
}
