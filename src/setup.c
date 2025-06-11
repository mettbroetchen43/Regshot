/*
    Copyright 2004 tulipfan
    Copyright 2011-2012 Regshot Team

    This file is part of Regshot.

    Regshot is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2.1 of the License, or
    (at your option) any later version.

    Regshot is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Regshot.  If not, see <http://www.gnu.org/licenses/>.
*/

/* This file orignal coded by tulipfan
   Change function/variable name to more proper ones and fix for x64 by tianwei
*/

#include "global.h"

// 1.8.2 move definition from global.h to this place
#define MAX_INI_SKIPITEMS 100  // 0..99

// setup based on regshot.ini by tulipfan (tfx)
LPTSTR lpszIniSetup          = TEXT("Setup");
LPTSTR lpszIniFlag           = TEXT("Flag");
LPTSTR lpszIniExtDir         = TEXT("ExtDir");
LPTSTR lpszIniLanguage       = TEXT("Language");
LPTSTR lpszIniOutDir         = TEXT("OutDir");
LPTSTR lpszIniSkipRegKey     = TEXT("SkipRegKey");
LPTSTR lpszIniSkipdir        = TEXT("SkipDir");
LPTSTR lpszIniUseLongRegHead = TEXT("UseLongRegHead");

LPTSTR lpgrszRegSkipStrings;
LPTSTR lpgrszFileSkipStrings;

LPTSTR *lprgszRegSkipStrings;
LPTSTR *lprgszFileSkipStrings;

BOOL bUseLongRegHead;  // since 1.8.1 tianwei: Flag for compatibility with Regshot 1.61e5 and undoreg 1.46


BOOL LoadSettingsFromIni(HWND hDlg) // tfx get ini info
{
    int   i;
    BYTE  nFlag;
    DWORD cchSection;
    TCHAR szIniKey[17];

    // Get registry skip strings
    lprgszRegSkipStrings = MYALLOC0((MAX_INI_SKIPITEMS + 1) * sizeof(LPTSTR));
    lprgszRegSkipStrings[MAX_INI_SKIPITEMS] = NULL;  // saftey NULL pointer
    lpgrszRegSkipStrings = MYALLOC0(MAX_INI_SECTION_CHARS * sizeof(TCHAR));
    cchSection = GetPrivateProfileSection(lpszIniSkipRegKey, lpgrszRegSkipStrings, MAX_INI_SECTION_CHARS, lpszRegshotIni);  // length incl. double NULL character
    if (0 < cchSection) {
        for (i = 0; MAX_INI_SKIPITEMS > i; i++) {
            _sntprintf(szIniKey, 17, TEXT("%d%s\0"), i, TEXT("="));
            szIniKey[16] = (TCHAR)'\0';  // saftey NULL char
            lprgszRegSkipStrings[i] = FindKeyInIniSection(lpgrszRegSkipStrings, szIniKey, cchSection, _tcslen(szIniKey));
            if (NULL == lprgszRegSkipStrings[i]) {
                break;  // not found, so do not look any further
            }
        }
    }

    // Get file skip strings
    lprgszFileSkipStrings = MYALLOC0((MAX_INI_SKIPITEMS + 1) * sizeof(LPTSTR));
    lprgszFileSkipStrings[MAX_INI_SKIPITEMS] = NULL;  // saftey NULL pointer
    lpgrszFileSkipStrings = MYALLOC0(MAX_INI_SECTION_CHARS * sizeof(TCHAR));
    cchSection = GetPrivateProfileSection(lpszIniSkipdir, lpgrszFileSkipStrings, MAX_INI_SECTION_CHARS, lpszRegshotIni);  // length incl. double NULL character
    if (0 < cchSection) {
        for (i = 0; MAX_INI_SKIPITEMS > i; i++) {
            _sntprintf(szIniKey, 17, TEXT("%d%s\0"), i, TEXT("="));
            szIniKey[16] = (TCHAR)'\0';  // saftey NULL char
            lprgszFileSkipStrings[i] = FindKeyInIniSection(lpgrszFileSkipStrings, szIniKey, cchSection, _tcslen(szIniKey));
            if (NULL == lprgszFileSkipStrings[i]) {
                break;  // not found, so do not look any further
            }
        }
    }

    // Flags
    nFlag = (BYTE)GetPrivateProfileInt(lpszIniSetup, lpszIniFlag, 1, lpszRegshotIni); // default from 0 to 1 in 1.8.2 (TEXT)
    /*if (nFlag != 0) {  // deleted in 1.8.1 */
    SendMessage(GetDlgItem(hDlg, IDC_RADIO1), BM_SETCHECK, (WPARAM)(nFlag & 0x01), (LPARAM)0);  // Text output
    SendMessage(GetDlgItem(hDlg, IDC_RADIO2), BM_SETCHECK, (WPARAM)((nFlag & 0x01) ^ 0x01), (LPARAM)0);  // HTML output
    //SendMessage(GetDlgItem(hDlg, IDC_CHECKDIR), BM_SETCHECK, (WPARAM)((nFlag&0x04)>>1), (LPARAM)0); // 1.7
    SendMessage(GetDlgItem(hDlg, IDC_CHECKDIR), BM_SETCHECK, (WPARAM)((nFlag & 0x02) >> 1), (LPARAM)0);
    /*} else {  // deleted in 1.8.1
        SendMessage(GetDlgItem(hDlg, IDC_RADIO1), BM_SETCHECK, (WPARAM)0x01, (LPARAM)0);
        SendMessage(GetDlgItem(hDlg, IDC_RADIO2), BM_SETCHECK, (WPARAM)0x00, (LPARAM)0);
        SendMessage(GetDlgItem(hDlg, IDC_CHECKDIR), BM_SETCHECK, (WPARAM)0x00, (LPARAM)0);
    }
    */

    // UseLongRegHead
    bUseLongRegHead = GetPrivateProfileInt(lpszIniSetup, lpszIniUseLongRegHead, 0, lpszRegshotIni) != 0 ? TRUE : FALSE;

    // Scan Dirs
    if (0 != GetPrivateProfileString(lpszIniSetup, lpszIniExtDir, NULL, lpszExtDir, MAX_PATH, lpszRegshotIni)) {  // length incl. NULL character
        SetDlgItemText(hDlg, IDC_EDITDIR, lpszExtDir);
    } else {
        SetDlgItemText(hDlg, IDC_EDITDIR, lpszWindowsDirName);
    }

    // Output Dir
    if (0 != GetPrivateProfileString(lpszIniSetup, lpszIniOutDir, NULL, lpszOutputPath, MAX_PATH, lpszRegshotIni)) {  // length incl. NULL character
        SetDlgItemText(hDlg, IDC_EDITPATH, lpszOutputPath);
    } else {
        SetDlgItemText(hDlg, IDC_EDITPATH, lpszTempPath);
    }

    SendMessage(hDlg, WM_COMMAND, (WPARAM)IDC_CHECKDIR, (LPARAM)0);

    return TRUE;
}


BOOL SaveSettingsToIni(HWND hDlg) // tfx save settings to ini
{
    BYTE    nFlag;
    LPTSTR  lpszValue;
    HANDLE  hTest;

    // 1.8.2, someone might not want to create a regshot.ini when there isn't one. :O
    hTest = CreateFile(lpszRegshotIni, GENERIC_READ | GENERIC_WRITE,
                       FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hTest == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    CloseHandle(hTest);

    //nFlag = (BYTE)(SendMessage(GetDlgItem(hDlg, IDC_RADIO1), BM_GETCHECK, (WPARAM)0, (LPARAM)0) // 1.7
    //  |SendMessage(GetDlgItem(hDlg, IDC_RADIO2), BM_GETCHECK, (WPARAM)0, (LPARAM)0)<<1
    //  |SendMessage(GetDlgItem(hDlg, IDC_CHECKDIR), BM_GETCHECK, (WPARAM)0, (LPARAM)0)<<2);
    nFlag = (BYTE)(SendMessage(GetDlgItem(hDlg, IDC_RADIO1), BM_GETCHECK, (WPARAM)0, (LPARAM)0) |
                   SendMessage(GetDlgItem(hDlg, IDC_CHECKDIR), BM_GETCHECK, (WPARAM)0, (LPARAM)0) << 1);

    lpszValue = MYALLOC0(EXTDIRLEN * sizeof(TCHAR));  // EXTDIRLEN due to IDC_EDITDIR
    //_sntprintf(lpszValue, EXTDIRLEN, TEXT("%s = %d\0"), lpszIniFlag, nFlag);                   // 1.7 solokey
    //lpszValue[EXTDIRLEN - 1] = (TCHAR)'\0';  // safety NULL char
    //WritePrivateProfileSection(lpszIniSetup, lpszValue, lpszRegshotIni);  // 1.7 solokey, can only have one key.

    // 1.8.1
    _sntprintf(lpszValue, EXTDIRLEN, TEXT("%d\0"), nFlag);
    lpszValue[EXTDIRLEN - 1] = (TCHAR)'\0';  // safety NULL char
    WritePrivateProfileString(lpszIniSetup, lpszIniFlag, lpszValue, lpszRegshotIni);

    _sntprintf(lpszValue, EXTDIRLEN, TEXT("%d\0"), bUseLongRegHead);
    lpszValue[EXTDIRLEN - 1] = (TCHAR)'\0';  // safety NULL char
    WritePrivateProfileString(lpszIniSetup, lpszIniUseLongRegHead, lpszValue, lpszRegshotIni);

    if (GetDlgItemText(hDlg, IDC_EDITDIR, lpszValue, EXTDIRLEN) != 0) {  // length incl. NULL character
        lpszValue[EXTDIRLEN - 1] = (TCHAR)'\0';  // safety NULL char
        WritePrivateProfileString(lpszIniSetup, lpszIniExtDir, lpszValue, lpszRegshotIni);
    }

    if (GetDlgItemText(hDlg, IDC_EDITPATH, lpszValue, EXTDIRLEN) != 0) {  // length incl. NULL character
        lpszValue[EXTDIRLEN - 1] = (TCHAR)'\0';  // safety NULL char
        WritePrivateProfileString(lpszIniSetup, lpszIniOutDir, lpszValue, lpszRegshotIni);
    }

    // 1.9.0: Write language selection to regshot ini (was in language ini before)
    WritePrivateProfileString(lpszIniSetup, lpszIniLanguage, lpszLanguage, lpszRegshotIni);

    MYFREE(lpszValue);

    /* Clean up global memory and from LoadSettingsFromIni() plus SetTextsToSelectedLanguage()*/
    MYFREE(lpszRegshotIni);
    if (NULL != lpgrszRegSkipStrings) {
        MYFREE(lpgrszRegSkipStrings);
    }
    if (NULL != lpgrszFileSkipStrings) {
        MYFREE(lpgrszFileSkipStrings);
    }
    if (NULL != lpgrszLangSection) {
        MYFREE(lpgrszLangSection);
    }

    return TRUE;
}


BOOL IsInSkipList(LPTSTR lpszString, LPTSTR rgszSkipList[])  // tfx skip the list
{
    int i;

    for (i = 0; (MAX_INI_SKIPITEMS > i) && (NULL != rgszSkipList[i]); i++) {
        if (0 == _tcsicmp(lpszString, rgszSkipList[i])) {
            return TRUE;
        }
    }
    return FALSE;
}
