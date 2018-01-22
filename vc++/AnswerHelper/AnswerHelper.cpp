/*
 *  @file  : AnswerHelper.cpp
 *  @author: Shilyx
 *  @date  : 2018-01-13 17:51:49.014
 *  @note  : Generated by SlxTemplates
 */

#include <slxcommon.h>
#include <charconv.h>
#include <Windows.h>
#include <GdiPlus.h>
#include <CommCtrl.h>
#include <Shlwapi.h>
#pragma warning(disable: 4786)
#include <tchar.h>
#include "resource.h"
#include <string>

using namespace Gdiplus;
using namespace std;

class CAnswerHelperDialog
{
#define ANSWERHELPER_OBJECT_PROP_NAME TEXT("__AnswerHelperObject")

public:
    CAnswerHelperDialog(HINSTANCE hInstance, LPCTSTR lpTemplate, HWND hParent)
        : m_hInstance(hInstance)
        , m_lpTemplate(lpTemplate)
        , m_hwndDlg(NULL)
        , m_hParent(hParent)
        , m_nServerPort(0)
    {
    }

    HWND CreateModellessDialog()
    {
        return CreateDialogParam(m_hInstance, m_lpTemplate, m_hParent, AnswerHelperDialogProc, (LPARAM)this);
    }

    operator HWND() const
    {
        return m_hwndDlg;
    }

    static int DemoMessageLoop(HWND hMainDlg)
    {
        MSG msg;

        while (TRUE)
        {
            int nRet = GetMessage(&msg, NULL, 0, 0);

            if (nRet < 0)
            {
                break;
            }

            if (nRet == 0)
            {
                return (int)msg.wParam;
            }

            if (!IsDialogMessage(hMainDlg, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        return 0;
    }

    static CAnswerHelperDialog *GetBindObject(HWND hwndDlg)
    {
        return (CAnswerHelperDialog *)GetProp(hwndDlg, ANSWERHELPER_OBJECT_PROP_NAME);
    }

private:
    static INT_PTR CALLBACK AnswerHelperDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_INITDIALOG)
        {
            SetProp(hwndDlg, ANSWERHELPER_OBJECT_PROP_NAME, (HANDLE)lParam);
        }

        CAnswerHelperDialog *pAnswerHelperDialog = GetBindObject(hwndDlg);

        if (pAnswerHelperDialog != NULL)
        {
            return pAnswerHelperDialog->AnswerHelperDialogPrivateProc(hwndDlg, uMsg, wParam, lParam);
        }

        return FALSE;
    }

    INT_PTR CALLBACK AnswerHelperDialogPrivateProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_INITDIALOG:{
            HICON hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_MAINFRAME));

            SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
            SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

            m_hwndDlg = hwndDlg;
            OnInitDialog();

            break;}

        case WM_CLOSE:
            DestroyWindow(hwndDlg);
            break;

        case WM_DESTROY:
            OnDestroy();
            PostQuitMessage(0);
            RemoveProp(hwndDlg, ANSWERHELPER_OBJECT_PROP_NAME);
            break;

        case WM_SIZE:
            {
                RECT rect;
                HWND hStatic = GetDlgItem(m_hwndDlg, IDC_STATIC_FILL);

                if (IsWindow(hStatic)) {
                    GetClientRect(m_hwndDlg, &rect);
                    InflateRect(&rect, -3, -3);
                    rect.top = 33;
                    MoveWindow(hStatic, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
                }
            }
            InvalidateRect(m_hwndDlg, NULL, TRUE);
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDC_QUIT) {
                DestroyWindow(hwndDlg);
            } else if (LOWORD(wParam) == IDC_DO) {
                OnDo();
            }
            break;

        case WM_LBUTTONDOWN:
            PostMessageW(m_hwndDlg, WM_NCLBUTTONDOWN, HTCAPTION, 0);
            break;

        case WM_RBUTTONUP:
            MessageBoxW(m_hwndDlg, L"WM_MBUTTONUP", NULL, MB_ICONEXCLAMATION);
            break;

        case WM_MBUTTONUP:
            SwitchCmdShown();
            break;

        case WM_CTLCOLORSTATIC:
            if (lParam == (LPARAM)GetDlgItem(m_hwndDlg, IDC_AIRESULT)) {
                break;
            }
            return (INT_PTR)GetStockObject(WHITE_BRUSH);
            break;

        case WM_TIMER:
            if (wParam == 1) {
                if (m_nShownRestTime > 0) {
                    if (--m_nShownRestTime <= 0) {
                        SetDlgItemTextW(m_hwndDlg, IDC_AIRESULT, NULL);
                    }
                }
            }
            break;

        default:
            break;
        }

        return FALSE;
    }

    void OnInitDialog()
    {
        LoadWindowPlacement(m_hwndDlg, L"AnswerHelper", NULL, FALSE, FALSE);
        InitServer();
        SetDlgItemTextW(m_hwndDlg, IDC_STATIC_FILL, NULL);
        ModifyExStyle(m_hwndDlg, WS_EX_LAYERED, 0);
        SetLayeredWindowAttributes(m_hwndDlg, RGB(255, 255, 255), 0, LWA_COLORKEY);
        SendMessageW(m_hwndDlg, WM_SIZE, 0, 0);
        SetTimer(m_hwndDlg, 1, 1000, NULL);
    }

    void SwitchCmdShown() {
        HWND hWindow = FindWindowExW(NULL, NULL, L"ConsoleWindowClass", NULL);

        while (IsWindow(hWindow)) {
            if (m_dwChildProcessId == GetWindowProcessId(hWindow)) {
                if (IsWindowVisible(hWindow)) {
                    ShowWindow(hWindow, SW_HIDE);
                } else {
                    ShowWindow(hWindow, SW_SHOW);
                    if (IsIconic(hWindow)) {
                        ShowWindow(hWindow, SW_RESTORE);
                    }
                }
                break;
            }

            hWindow = FindWindowExW(NULL, hWindow, L"ConsoleWindowClass", NULL);
        }
    }

    DWORD RecvAIResultProc(USHORT nPort) {
        SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        if (sock == INVALID_SOCKET) {
            return 0;
        }

        sockaddr_in sin;
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = inet_addr("127.18.1.13");
        sin.sin_port = htons(nPort);

        if (0 != ::bind(sock, (sockaddr *)&sin, sizeof(sin))) {
            closesocket(sock);
            return 0;
        }

        while (true) {
            char szBuffer[1024];
            int len = sizeof(sin);
            int ret = recvfrom(sock, szBuffer, sizeof(szBuffer), 0, (sockaddr *)&sin, &len);

            if (ret > 0) {
                m_nShownRestTime = 10;
                SetDlgItemTextW(m_hwndDlg, IDC_AIRESULT, UtoW(string(szBuffer, ret)).c_str());
            } else if (ret < 0) {
                break;
            }
        }

        closesocket(sock);
        return 0;
    }

    void OnDestroy()
    {
        SaveWindowPlacement(m_hwndDlg, L"AnswerHelper");

        // 删除图片文件
        DeleteFileW(m_strLastJpgPath.c_str());
    }

    void InitServer() {
        WCHAR szParams[1024];

        m_nServerPort = rand() % 8000 + 51000;
        CloseHandle(CreateClassThread(std::tr1::bind(&CAnswerHelperDialog::RecvAIResultProc, this, m_nServerPort + 1), 0, NULL));
        wnsprintfW(szParams, RTL_NUMBER_OF(szParams), L"/k node AnswerHelperServer\\AnswerHelperServer.js %u", (unsigned)m_nServerPort);

        HANDLE hChildProcess = ShellExecuteReturnProcess(NULL, L"open", L"cmd", szParams, NULL, SW_SHOW);
        if (hChildProcess) {
            m_dwChildProcessId = GetProcessId(hChildProcess);
            CloseHandle(hChildProcess);
        }
    }

    DWORD DisableSleepEnableDoButtonProc() {
        Sleep(777);
        EnableWindow(GetDlgItem(m_hwndDlg, IDC_DO), TRUE);
        return 0;
    }

    void OnDo() {
        HWND hStatic = GetDlgItem(m_hwndDlg, IDC_STATIC_FILL);
        RECT rect;
        HDC hScreenDc = GetDC(NULL);

        // 处理上次文件
        if (!m_strLastJpgPath.empty()) {
            DeleteFileW(m_strLastJpgPath.c_str());
        }

        // 构建路径
        WCHAR szTempPath[MAX_PATH];
        WCHAR szJpgPath[MAX_PATH];

        GetTempPathW(RTL_NUMBER_OF(szTempPath), szTempPath);
        wnsprintfW(szJpgPath, RTL_NUMBER_OF(szJpgPath), L"%ls\\__AnswerHelper_%ls.jpg", szTempPath, CreateGuidW().c_str());
        GetWindowRect(hStatic, &rect);

        if (SaveDcJpgFileStream(hScreenDc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, szJpgPath, 88)) {
            SendUdpString("127.18.1.13", m_nServerPort, WtoU(szJpgPath).c_str());
        }

        m_strLastJpgPath = szJpgPath;
        ReleaseDC(NULL, hScreenDc);

        SetDlgItemTextW(m_hwndDlg, IDC_AIRESULT, NULL);
        EnableWindow(GetDlgItem(m_hwndDlg, IDC_DO), FALSE);
        CloseHandle(CreateClassThread(std::tr1::bind(&CAnswerHelperDialog::DisableSleepEnableDoButtonProc, this), 0, NULL));
    }

    static void SendUdpString(const char *addr, unsigned short port, const char *text) {
        SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        sockaddr_in sin;

        if (sock != INVALID_SOCKET) {
            sin.sin_family = AF_INET;
            sin.sin_addr.s_addr = inet_addr(addr);
            sin.sin_port = htons(port);

            sendto(sock, text, lstrlenA(text), 0, (sockaddr *)&sin, sizeof(sin));
            closesocket(sock);
        }
    }

    static BOOL GetEncoderClsid(LPCWSTR lpFormat, CLSID *pClsid)
    {
        UINT uNum = 0;
        UINT uIndex = 0;
        UINT uSize = 0;
        HANDLE hHeap = NULL;
        ImageCodecInfo *pImageCodecInfo = NULL;

        GetImageEncodersSize(&uNum, &uSize);

        if(uSize != 0)
        {
            hHeap = GetProcessHeap();
            pImageCodecInfo = (ImageCodecInfo *)HeapAlloc(hHeap, 0, uSize);

            if(pImageCodecInfo != NULL)
            {
                GetImageEncoders(uNum, uSize, pImageCodecInfo);

                for(; uIndex < uNum; uIndex += 1)
                {
                    if(lstrcmpW(pImageCodecInfo[uIndex].MimeType, lpFormat) == 0)
                    {
                        *pClsid = pImageCodecInfo[uIndex].Clsid;

                        break;
                    }
                }

                HeapFree(hHeap, 0, pImageCodecInfo);
            }
        }

        return uIndex < uNum;
    }

    BOOL SaveDcJpgFileStream(HDC hdc, int x, int y, int width, int height, LPCTSTR lpFilePath, LONG quality)
    {
        BOOL bRet = GenericError;

        if(hdc != NULL)
        {
            HBITMAP hbmp = CreateCompatibleBitmap(hdc, width, height);
            HDC hmemdc = CreateCompatibleDC(hdc);

            if(hbmp != NULL && hmemdc != NULL)
            {
                HBITMAP hbmpHold = (HBITMAP)::SelectObject(hmemdc, hbmp);
                BitBlt(hmemdc, 0, 0, width, height, hdc, x, y, SRCCOPY);

                CLSID clsidJpeg;
                EncoderParameters encoderParameters;
                Bitmap *pBitmap = Bitmap::FromHBITMAP(hbmp, (HPALETTE)GetStockObject(DEFAULT_PALETTE));

                if(pBitmap != NULL)
                {
                    GetEncoderClsid(L"image/jpeg", &clsidJpeg);

                    encoderParameters.Count = 1;
                    encoderParameters.Parameter[0].Guid = EncoderQuality;
                    encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
                    encoderParameters.Parameter[0].NumberOfValues = 1;

                    // Save the image as a JPEG with quality level 0.
                    encoderParameters.Parameter[0].Value = &quality;

                    // 保存到文件
                    IStream *pStream = NULL;

                    if (S_OK == SHCreateStreamOnFile(lpFilePath, STGM_WRITE | STGM_SHARE_DENY_WRITE | STGM_CREATE, &pStream))
                    {
                        bRet = pBitmap->Save(pStream, &clsidJpeg, &encoderParameters);
                        pStream->Release();
                    }

                    delete pBitmap;
                }

                SelectObject(hmemdc, hbmpHold);
                DeleteObject(hmemdc);
                DeleteObject(hbmp);
            }
        }

        return bRet == Ok;
    }

private:
    HWND m_hwndDlg;
    HWND m_hParent;
    HINSTANCE m_hInstance;
    LPCTSTR m_lpTemplate;
    wstring m_strLastJpgPath;
    USHORT m_nServerPort;
    int m_nShownRestTime;
    DWORD m_dwChildProcessId;
};

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
    srand(GetTickCount());
    InitCommonControls();
    WSAStartup(MAKEWORD(2, 2), &WSADATA());

    JoinOneKillOnCloseJob(TRUE);

    // 初始化gdi+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;

    if (Ok != GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL))
    {
        return 0;
    }

    CAnswerHelperDialog dlg(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL);
    HWND hDlg = dlg.CreateModellessDialog();

    ShowWindow(hDlg, nShowCmd);
    UpdateWindow(hDlg);

    return CAnswerHelperDialog::DemoMessageLoop(hDlg);
}