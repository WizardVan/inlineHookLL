#include <windows.h>

#include <string.h>

#include <stdio.h>



void HookFunction(const char *pDllName, char* funcName, LPDWORD newFunction, LPDWORD *oldFunction);

LPDWORD FoundIAT(const char *pDllName, char* funcName);



int WINAPI HookMessageBoxA(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);

typedef int(WINAPI *pOldMBox)(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);

HMODULE WINAPI HOOKLoadLib(LPCTSTR dllName);

typedef HMODULE(WINAPI *poldLoadLib)(LPCTSTR dllName);

pOldMBox pMBox = NULL;

poldLoadLib pLoadLib = NULL;

void main()
{
	HookFunction("Kernel32.dll","LoadLibraryA", (LPDWORD)&HOOKLoadLib, &pLoadLib);
	//LoadLibrary("User32.dll");
	//HookFunction("User32.dll","MessageBoxA", (LPDWORD)&HookMessageBoxA, &pMBox);
	//MessageBoxA(NULL, "hi!", "how are you?", MB_ICONWARNING | MB_CANCELTRYCONTINUE | MB_DEFBUTTON2);

	getchar();
}




void HookFunction(const char *pDllName,char* funcName, LPDWORD newFunction, LPDWORD *oldFunction)

{

	LPDWORD pOldFunction = FoundIAT(pDllName,funcName);

	DWORD accessProtectionValue, accessProtec;

	*oldFunction = (pOldMBox)*pOldFunction;

	int vProtect = VirtualProtect(pOldFunction, sizeof(LPDWORD), PAGE_READWRITE, &accessProtectionValue);

	*pOldFunction = (DWORD)newFunction;

	vProtect = VirtualProtect(pOldFunction, sizeof(LPDWORD), accessProtectionValue, &accessProtec);

}


HMODULE WINAPI HOOKLoadLib(LPCTSTR dllName)
{
	printf("LALALALALALALA\n");
	if (NULL == pLoadLib)
	{
		return LoadLibrary(dllName);
	}
	else
	{
		return pLoadLib(dllName);
	}
}


int WINAPI HookMessageBoxA(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)

{
	printf("AHAHAHAHAHA");
	if (NULL == pMBox)
	{
		return MessageBoxA(hWnd, "hi!", "how are you?", uType);
	}
	else
	{
		return pMBox(hWnd, "hi!", "how are you?", uType);
	}
	

}



LPDWORD FoundIAT(const char *pDllName,char* funcName)

{

	DWORD test = 0;



	LPVOID pMapping = GetModuleHandle(NULL);

	if (pMapping == NULL)

		exit(-1);



	PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)pMapping;



	if (DosHeader->e_magic != IMAGE_DOS_SIGNATURE)

		exit(-1);



	PIMAGE_NT_HEADERS NtHeaders = (PIMAGE_NT_HEADERS)((char*)DosHeader + DosHeader->e_lfanew);



	if (NtHeaders->Signature != IMAGE_NT_SIGNATURE)

		exit(-1);



	PIMAGE_DATA_DIRECTORY DataDirectory = &NtHeaders->OptionalHeader.DataDirectory[1]; // Import symbols

	PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((char*)DosHeader + DataDirectory->VirtualAddress);



	PIMAGE_THUNK_DATA32 OriginalFirstThunk = (PIMAGE_THUNK_DATA32)((char*)DosHeader + ImportDescriptor->OriginalFirstThunk);

	/*
	while (ImportDescriptor->Name)
	{
		DWORD pDllTestName = (DWORD)((char*)pMapping + ImportDescriptor->Name);
		printf("%s", pDllTestName);
		if (0 == _strcmpi(pDllTestName, pDllName))
		{
			break;
		}
		ImportDescriptor++;
	}
	
	if (!ImportDescriptor || !ImportDescriptor->Name)
	{
	exit(-1);
	}
	*/


	while (OriginalFirstThunk != 0)

	{

		while (ImportDescriptor->Name)
		{
			DWORD pDllTestName = (DWORD)((char*)pMapping + ImportDescriptor->Name);
			printf("%s", pDllTestName);
			if (0 == _strcmpi(pDllTestName, pDllName))
			{
				break;
			}
			ImportDescriptor++;
		}

		if (!ImportDescriptor || !ImportDescriptor->Name)
		{
			exit(-1);
		}
		
		//DWORD pDllTestName = (DWORD)((char*)pMapping + ImportDescriptor->Name);
		//printf("%s", pDllTestName);

		OriginalFirstThunk = (PIMAGE_THUNK_DATA32)((char*)DosHeader + ImportDescriptor->OriginalFirstThunk);

		PIMAGE_THUNK_DATA32 FirstThunk = (PIMAGE_THUNK_DATA32)((char*)DosHeader + ImportDescriptor->FirstThunk);



		//while (OriginalFirstThunk->u1.AddressOfData != 0)
		while (OriginalFirstThunk->u1.Function)
		{

			if (((DWORD)OriginalFirstThunk->u1.Ordinal & (DWORD)IMAGE_ORDINAL_FLAG32) != (DWORD)IMAGE_ORDINAL_FLAG32);
			{
			PIMAGE_IMPORT_BY_NAME NameImg = (PIMAGE_IMPORT_BY_NAME)((char*)DosHeader + (DWORD)OriginalFirstThunk->u1.AddressOfData);



			//test = (DWORD)OriginalFirstThunk->u1.Function & (DWORD)IMAGE_ORDINAL_FLAG32;

			//if (test == 0)

			//{
				printf("%s\n", (const char*)NameImg->Name);
				OutputDebugString((const char*)NameImg->Name);
				//OutputDebugString(' ');
				
				if (_strcmpi(funcName, (const char*)NameImg->Name) == 0)

				{

					return (LPDWORD)&(FirstThunk->u1.Function);

				}
				
				
			}



			OriginalFirstThunk++;

			FirstThunk++;

		}



		ImportDescriptor++;

	}



	return 0;

}