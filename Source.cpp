#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>

TCHAR szClassName[] = TEXT("Window");

BOOL HasWordEllipsis(HWND hStatic)
{
	RECT rect;
	GetClientRect(hStatic, &rect);
	const int nClientWidth = rect.right;
	const int nTextLength = GetWindowTextLength(hStatic);
	LPTSTR lpszText = (LPTSTR)GlobalAlloc(GMEM_FIXED, (nTextLength + 1) * sizeof(TCHAR));
	GetWindowText(hStatic, lpszText, nTextLength + 1);
	const HDC hdc = GetDC(hStatic);
	const HFONT hFont = (HFONT)SendMessage(hStatic, WM_GETFONT, 0, 0);
	const HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
	DrawText(hdc, lpszText, nTextLength, &rect, DT_CALCRECT);
	SelectObject(hdc, hOldFont);
	ReleaseDC(hStatic, hdc);
	GlobalFree(lpszText);
	return nClientWidth < rect.right;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hStatic;
	static HWND hButton;
	switch (msg)
	{
	case WM_CREATE:
		hStatic = CreateWindow(TEXT("STATIC"), TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ"), WS_VISIBLE | WS_CHILD | SS_WORDELLIPSIS, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("確認"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		break;
	case WM_SIZE:
		MoveWindow(hStatic, 0, 0, LOWORD(lParam), 32, TRUE);
		MoveWindow(hButton, 0, 50, 256, 32, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			TCHAR szText[1024];
			BOOL bEllipsis = HasWordEllipsis(hStatic);
			wsprintf(szText, TEXT("%s"), bEllipsis ? TEXT("...ある") : TEXT("...ない"));
			MessageBox(hWnd, szText, TEXT("確認"), 0);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("スタティックコントロールに省略記号「...」が存在するかどうか判定"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
