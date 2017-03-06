#include <windows.h>
#include <stdio.h>
#include <stdlib.h>


#pragma comment(linker,"/include:__tls_used") // This will cause the linker to create the TLS directory
#pragma section(".CRT$XLB",read) // Create a new section

DWORD CALLBACK CheckBP(LPVOID args)
{
	HANDLE thread = (HANDLE)args;
	CONTEXT ctx = { 0 };
	printf("Inside thread\n");
	while (1)
	{
		SecureZeroMemory(&ctx, sizeof(CONTEXT));

		ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
		if (GetThreadContext(thread, &ctx) == 0)
		{
			printf("Failed to get Thread Context\n");
			return 0;
		}

		if (ctx.Dr0 != 0 || ctx.Dr1 != 0 || ctx.Dr2 != 0 || ctx.Dr3 != 0)
		{
			printf("Debug Registers not empty\n");
			CloseHandle(thread);
			return 0;
			//ExitProcess(0);
		}
		Sleep(10);
	}
}

void CALLBACK handle_hwbp(PVOID hHandle, DWORD dwReason, PVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DWORD hThreadID;
		HANDLE hThread, hDupThread;
		printf("duplicating handle\n");
		DuplicateHandle(GetCurrentProcess(),
			GetCurrentThread(),
			GetCurrentProcess(),
			&hDupThread,
			0,
			FALSE,
			DUPLICATE_SAME_ACCESS
		);
		printf("Starting thread..\n");
		hThread = CreateThread(NULL, 0, CheckBP, (LPVOID)hDupThread, 0, &hThreadID);
		//WaitForSingleObject(hThread, INFINITE);
		//CloseHandle(hThread);
	}
}
__declspec(allocate(".CRT$XLB")) PIMAGE_TLS_CALLBACK CallbackAddress[] = { handle_hwbp, NULL };
/*
#ifdef _WIN64
#pragma comment (linker, "/INCLUDE:_tls_used")  // See p. 1 below
#pragma comment (linker, "/INCLUDE:tls_callback_func")  // See p. 3 below
#else
#pragma comment (linker, "/INCLUDE:__tls_used")  // See p. 1 below
#pragma comment (linker, "/INCLUDE:_tls_callback_func")  // See p. 3 below
#endif

// Explained in p. 3 below
#ifdef _WIN64
#pragma const_seg(".CRT$XLF")
EXTERN_C const
#else
#pragma data_seg(".CRT$XLF")
EXTERN_C
#endif
PIMAGE_TLS_CALLBACK tls_callback_func = handle_hwbp;
#ifdef _WIN64
#pragma const_seg()
#else
#pragma data_seg()
#endif //_WIN64
*/

int main(int argc, char** argv)
{
	printf("waiting..");
	getchar();
	return 0;
}