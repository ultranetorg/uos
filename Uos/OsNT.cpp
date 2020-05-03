#include "StdAfx.h"
#include "OsNT.h"

using namespace uos;

#define UOS_URL_PROTOCOL_STRING			L"URL:%s Protocol"
#define UOS_URL_PROTOCOL				L"URL Protocol"
#define UOS_URL_PROTOCOL_DEFAULTICON	L"DefaultIcon"
#define UOS_URL_PROTOCOL_COMMAND		L"Shell\\Open\\command"
#define UOS_URL_PROTOCOL_OPEN			L"Shell\\Open"
#define UOS_URL_PROTOCOL_SHELL			L"Shell"

COsNT::COsNT()
{
	Version = CVersion::FromFile(L"kernel32.dll");
}

COsNT::~COsNT()
{
}

bool COsNT::Is64()
{
	typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

#ifdef _WIN64
	return true;
#else
	HMODULE k32 = LoadLibrary(L"kernel32.dll");
	LPFN_ISWOW64PROCESS pIsWow64Process = (LPFN_ISWOW64PROCESS)::GetProcAddress(k32, "IsWow64Process");
	if(pIsWow64Process == null)
	{
		return false; // 95,98,Me,NT4,2000,XP
	}
	BOOL wow64 = FALSE;
	if(!pIsWow64Process(::GetCurrentProcess(), &wow64))
	{
		return false; // vista-32, win7-32
	}
	return (wow64==TRUE); // vista-64, win7-64
#endif
}

CVersion COsNT::GetVersion()
{
	return Version;
}

void COsNT::SaveDCToFile(LPCTSTR FileName, HDC hsourcedc, int Width, int Height)
{
	if(!FileName) return;

	HBITMAP OffscrBmp = null; // bitmap that is converted to a DIB
	HDC OffscrDC = null;      // offscreen DC that we can select OffscrBmp into
	LPBITMAPINFO lpbi = null; // bitmap format info; used by GetDIBits
	LPVOID lpvBits = null;    // pointer to bitmap bits array
	HANDLE BmpFile = INVALID_HANDLE_VALUE;    // destination .bmp file
	BITMAPFILEHEADER bmfh;  // .bmp file header

	try
	{

		// We need an HBITMAP to convert it to a DIB:
		OffscrBmp = CreateCompatibleBitmap(hsourcedc, Width, Height);

		// The bitmap is empty, so let's copy the contents of the surface to it.
		// For that we need to select it into a device context. We create one.
		OffscrDC = CreateCompatibleDC(hsourcedc);
		// Select OffscrBmp into OffscrDC:
		HBITMAP OldBmp = (HBITMAP)SelectObject(OffscrDC, OffscrBmp);
		// Now we can copy the contents of the surface to the offscreen bitmap:
		BitBlt(OffscrDC, 0, 0, Width, Height, hsourcedc, 0, 0, SRCCOPY);


		// GetDIBits requires format info about the bitmap. We can have GetDIBits
		// fill a structure with that info if we pass a NULL pointer for lpvBits:
		// Reserve memory for bitmap info (BITMAPINFOHEADER + largest possible
		// palette):
		lpbi = (LPBITMAPINFO)(new char[sizeof(BITMAPINFOHEADER) +	256 * sizeof(RGBQUAD)]);
		ZeroMemory(&lpbi->bmiHeader, sizeof(BITMAPINFOHEADER));
		lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		// Get info but first de-select OffscrBmp because GetDIBits requires it:
		SelectObject(OffscrDC, OldBmp);
		GetDIBits(OffscrDC, OffscrBmp, 0, Height, null, lpbi, DIB_RGB_COLORS);

		// Reserve memory for bitmap bits:
		lpvBits = new char[lpbi->bmiHeader.biSizeImage];

		// Have GetDIBits convert OffscrBmp to a DIB (device-independent bitmap):
		GetDIBits(OffscrDC, OffscrBmp, 0, Height, lpvBits, lpbi, DIB_RGB_COLORS);

		// Create a file to save the DIB to:
		BmpFile = CreateFile(FileName, GENERIC_WRITE, 0, null, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, null);

		DWORD Written;    // number of bytes written by WriteFile

		// Write a file header to the file:
		bmfh.bfType = 19778;        // 'BM'
		// bmfh.bfSize = ???        // we'll write that later
		bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
		// bmfh.bfOffBits = ???     // we'll write that later
		WriteFile(BmpFile, &bmfh, sizeof(bmfh), &Written, null);


		// Write BITMAPINFOHEADER to the file:
		WriteFile(BmpFile, &lpbi->bmiHeader, sizeof(BITMAPINFOHEADER), &Written, null);

		// Calculate size of palette:
		int PalEntries;
		// 16-bit or 32-bit bitmaps require bit masks:
		if(lpbi->bmiHeader.biCompression == BI_BITFIELDS) PalEntries = 3;
		else
			// bitmap is palettized?
			PalEntries = (lpbi->bmiHeader.biBitCount <= 8) ?
			// 2^biBitCount palette entries max.:
			(int)(1 << lpbi->bmiHeader.biBitCount)
			// bitmap is TrueColor -> no palette:
			: 0;
		// If biClrUsed use only biClrUsed palette entries:
		if(lpbi->bmiHeader.biClrUsed) PalEntries = lpbi->bmiHeader.biClrUsed;

		// Write palette to the file:
		if(PalEntries)
		{
			WriteFile(BmpFile, &lpbi->bmiColors, PalEntries * sizeof(RGBQUAD), &Written, null);
		}

		// The current position in the file (at the beginning of the bitmap bits)
		// will be saved to the BITMAPFILEHEADER:
		bmfh.bfOffBits = SetFilePointer(BmpFile, 0, 0, FILE_CURRENT);

		// Write bitmap bits to the file:
		WriteFile(BmpFile, lpvBits, lpbi->bmiHeader.biSizeImage, &Written, null);

		// The current pos. in the file is the final file size and will be saved:
		bmfh.bfSize = SetFilePointer(BmpFile, 0, 0, FILE_CURRENT);

		// We have all the info for the file header. Save the updated version:
		SetFilePointer(BmpFile, 0, 0, FILE_BEGIN);
		WriteFile(BmpFile, &bmfh, sizeof(bmfh), &Written, null);

	}
	catch(...)
	{
	}

	if(OffscrDC) DeleteDC(OffscrDC);
	if(OffscrBmp) DeleteObject(OffscrBmp);
	if(lpbi) delete[] lpbi;
	if(lpvBits) delete[] lpvBits;
	if(BmpFile != INVALID_HANDLE_VALUE) CloseHandle(BmpFile);
}


CList<CString> COsNT::OpenFileDialog(DWORD options, CArray<std::pair<CString,CString>> & types)
{
	CList<CString> o;
	IFileOpenDialog *pFileOpen;


	// Create the FileOpenDialog object.
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

	if(SUCCEEDED(hr))
	{
		DWORD dwOptions;

		hr = pFileOpen->GetOptions(&dwOptions);


		CArray<COMDLG_FILTERSPEC> s;

		for(auto & i : types)
		{
			s.push_back(COMDLG_FILTERSPEC{i.first.data(), i.second.data()});
		}

		pFileOpen->SetFileTypes((UINT)s.size(), s.data());


		if(SUCCEEDED(hr))
		{
			hr = pFileOpen->SetOptions(dwOptions | options);
		}

		hr = pFileOpen->Show(NULL);

		if(SUCCEEDED(hr))
		{
			IShellItemArray * pIItemArray;
			HRESULT hr = pFileOpen->GetResults(&pIItemArray);
			DWORD dwItemCount = 0;

			if(SUCCEEDED(hr))
			{
				hr = pIItemArray->GetCount(&dwItemCount);
				if(SUCCEEDED(hr))
				{
					for(DWORD i = 0; i < dwItemCount; i++)
					{
						IShellItem * pItem;
						hr = pIItemArray->GetItemAt(i, &pItem);
						if(SUCCEEDED(hr))
						{
							LPWSTR pszName = NULL;
							hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszName);

							if(SUCCEEDED(hr))
							{
								//dwRet += wcslen(pszName) + 1;
								o.push_back(pszName);
								::CoTaskMemFree(pszName);
							}
						}
					}
				}
				pIItemArray->Release();
			}
		}
		pFileOpen->Release();
	}

	return o;
}

CString COsNT::ComputerName()
{
	wchar_t compname[MAX_COMPUTERNAME_LENGTH];
	DWORD n = MAX_COMPUTERNAME_LENGTH;
	GetComputerNameEx(ComputerNameNetBIOS, compname, &n);

	return compname;
}

CString uos::COsNT::GetUserName()
{
	wchar_t b[UNLEN + 1];
	DWORD n = _countof(b);
	::GetUserName(b, &n);

	return b;
}


void COsNT::RegisterUrlProtocol(const CString & protocolName, const CString & pwCompanyName, const CString & pwAppPath)
{
	WCHAR szValue[MAX_PATH] = {0};
	HKEY hKey = NULL;
	HKEY hKeyDefaultIcon = NULL;
	HKEY hKeyCommand = NULL;
	bool IsRegAlreadyPresent = false;

	if(RegOpenKeyExW(HKEY_CLASSES_ROOT, protocolName.c_str(), 0L, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		RegCreateKeyExW(HKEY_CLASSES_ROOT, protocolName.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	}

	if(hKey)
	{
		swprintf_s(szValue, MAX_PATH, UOS_URL_PROTOCOL_STRING, protocolName.c_str());
		RegSetValueExW(hKey, L"", 0, REG_SZ, (BYTE *)&szValue, (DWORD)wcslen(szValue) * 2 + 2);
		RegSetValueExW(hKey, UOS_URL_PROTOCOL, 0, REG_SZ, (BYTE *)"", 1);

		if(RegOpenKeyExW(hKey, UOS_URL_PROTOCOL_DEFAULTICON, 0L, KEY_READ, &hKeyDefaultIcon) != ERROR_SUCCESS)
		{
			RegCreateKeyW(hKey, UOS_URL_PROTOCOL_DEFAULTICON, &hKeyDefaultIcon);
		}

		if(hKeyDefaultIcon)
		{
			RegSetValueExW(hKeyDefaultIcon, L"", 0, REG_SZ, (BYTE *)pwAppPath.data(), (DWORD)pwAppPath.size() * 2 + 2);
		}

		if(RegOpenKeyExW(hKey, UOS_URL_PROTOCOL_COMMAND, 0L, KEY_READ | KEY_WRITE, &hKeyCommand) != ERROR_SUCCESS)
		{
			RegCreateKeyW(hKey, UOS_URL_PROTOCOL_COMMAND, &hKeyCommand);
		}

		if(hKeyCommand)
		{
			DWORD n = sizeof(szValue);
			RegGetValue(hKeyCommand, NULL, NULL, RRF_RT_REG_SZ, NULL, (BYTE *)szValue, &n);

			auto p = CString::Format(L"\"%s\" \"%%1\"", pwAppPath);

			if(!p.EqualsInsensitive(szValue))
			{
				RegSetValueExW(hKeyCommand, L"", 0, REG_SZ, (BYTE *)p.data(), (DWORD)p.size() * 2 + 2);
				//Log->ReportWarning(this, L"Protocol overrided: %s - %s", protocolName, szValue);
			}

		}
	}

	if(hKeyCommand)
	{
		::RegCloseKey(hKeyCommand);
	}
	if(hKeyDefaultIcon)
	{
		::RegCloseKey(hKeyDefaultIcon);
	}
	if(hKey)
	{
		::RegCloseKey(hKey);
	}
}

BOOL COsNT::SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if(!LookupPrivilegeValue(	NULL,			// lookup privilege on local system
								lpszPrivilege,	// privilege to lookup 
								&luid))			// receives LUID of privilege
							{
								throw CException(HERE, CString::Format(L"LookupPrivilegeValue : %u", GetLastError()));
							}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if(bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.

	if(!AdjustTokenPrivileges(	hToken,
								FALSE,
								&tp,
								sizeof(TOKEN_PRIVILEGES),
								(PTOKEN_PRIVILEGES)NULL,
								(PDWORD)NULL))
	{
		throw CException(HERE, CString::Format(L"AdjustTokenPrivileges : %u", GetLastError()));
	}

	if(GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		throw CException(HERE, CString::Format(L"The token does not have the specified privilege"));
	}

	return TRUE;
}