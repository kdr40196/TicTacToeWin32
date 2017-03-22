// TicTacToeWindows.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TicTacToeWindows.h"
#include "GameController.hpp"
#include <windowsx.h>
#include <string>
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TICTACTOEWINDOWS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICTACTOEWINDOWS));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TICTACTOEWINDOWS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    //wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TICTACTOEWINDOWS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBRUSH winBrush;
    switch (message)
    {
	case WM_CREATE:
		{
			GameController::getInstance()->reset();
			winBrush = CreateSolidBrush(RGB(200, 200, 0));
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case ID_FILE_NEWGAME: {
					int ret = MessageBox(hWnd, L"Are you sure you want to start a new game?", L"New Game", MB_YESNO | MB_ICONQUESTION);
					if (ret == IDYES) {
						GameController::getInstance()->reset();
						InvalidateRect(hWnd, NULL, TRUE);
						UpdateWindow(hWnd);
					}
				}
				
				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

	case WM_GETMINMAXINFO: {
			MINMAXINFO* minMax = (MINMAXINFO*)lParam;
			minMax->ptMinTrackSize.x = CELL_SIZE * 5;
			minMax->ptMinTrackSize.y = CELL_SIZE * 5;
		}
		break;

	case WM_LBUTTONDOWN: {
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		int cellNo = GameController::getInstance()->getCell(hWnd, xPos, yPos);
		if (cellNo == -1) break;
		HDC hdc = GetDC(hWnd);
		if (hdc != NULL) {
			RECT rect;
			if (GameController::getInstance()->getCellRect(hWnd, cellNo, rect)) {
				int result = GameController::getInstance()->mark(hdc, rect, cellNo);

				// win
				if (result == 1) {
					std::wstring sMessage;
					GameController::getInstance()->showTurn(hWnd, hdc, &sMessage);
					for (int i = 0; i < 3; i++) {
						GameController::getInstance()->getCellRect(hWnd, GameController::getInstance()->winningMoves[i], rect);
						FillRect(hdc, &rect, winBrush);
					}
					std::wstring caption = L"GAME OVER!";
					MessageBeep(SOUND_SYSTEM_BEEP);
					MessageBox(hWnd, sMessage.c_str(), caption.c_str(), MB_OK | MB_ICONINFORMATION);
				}
				// board full - TIE
				else if (result == -1) {
					std::wstring sMessage;
					GameController::getInstance()->showTurn(hWnd, hdc, &sMessage);
					std::wstring caption = L"GAME OVER!";
					MessageBeep(SOUND_SYSTEM_BEEP);
					MessageBox(hWnd, sMessage.c_str(), caption.c_str(), MB_OK | MB_ICONINFORMATION);
				}

				else {
					GameController::getInstance()->showTurn(hWnd, hdc, nullptr);
				}
			}
		}
		break;
	}

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            
			// draw board
			RECT rect;
			GetClientRect(hWnd, &rect);
			
			SetBkMode(hdc, TRANSPARENT);			// for transparent text background
			WCHAR szPlayer1[] = L"Player 1";
			WCHAR szPlayer2[] = L"Player 2";
			SetTextColor(hdc, RGB(200, 0, 0));
			TextOut(hdc, 20, 20, szPlayer1, ARRAYSIZE(szPlayer1));
			SetTextColor(hdc, RGB(0, 0, 200));
			TextOut(hdc, rect.right - 75, 20, szPlayer2, ARRAYSIZE(szPlayer2));

			if (GameController::getInstance()->getBoard(hWnd, rect)) {
				FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
				int max = 3 * CELL_SIZE;
				for (int i = CELL_SIZE; i < max; i += CELL_SIZE) {

					// Vertical Line
					GameController::getInstance()->drawLine(hdc, rect.left + i, rect.top, rect.left + i, rect.bottom);
					
					// Horizontal Line
					GameController::getInstance()->drawLine(hdc, rect.left, rect.top + i, rect.right, rect.top + i);
				}
			}
			GameController::getInstance()->showTurn(hWnd, hdc, nullptr);

			// paint player moves, if any
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					if (GameController::getInstance()->board[i][j] != nullptr && GameController::getInstance()->getCellRect(hWnd, j + i * 3, rect)) {
						FillRect(hdc, &rect, GameController::getInstance()->board[i][j]->brush);
						//DrawIcon
					}
				}
			}

			// highlight winning moves, if any
			if (GameController::getInstance()->winningMoves[0] != -1) {
				for (int i = 0; i < 3; i++) {
					if (GameController::getInstance()->getCellRect(hWnd, GameController::getInstance()->winningMoves[i], rect)) {
						FillRect(hdc, &rect, winBrush);
					}
				}
			}
            
			EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		GameController::getInstance()->resetInstance();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
