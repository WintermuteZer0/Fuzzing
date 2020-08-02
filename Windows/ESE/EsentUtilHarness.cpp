// EsentHarness.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <windows.h> 
#include <stdio.h>
#include <esent.h>


typedef int(__stdcall* GetFileInfoA)(const char* szDatabaseName, void* pvResult, unsigned long cbMax, unsigned long InfoLevel);

extern "C" __declspec(dllexport)int main(int argc, char** argv);
/**void fuzz(int argc, char* argv[])
{
    GetFileInfoA ProcAddGetInfo;
    void* result_buff;
    result_buff = (void*)malloc(200); // This is hacky and should be fixed appropriatley
    ProcAddGetInfo = (GetFileInfoA)GetProcAddress(hinstLib, "JetGetDatabaseFileInfoA");
    if (NULL != ProcAddGetInfo)
    {
        //result = (ProcAddGetInfo)(argv[1], &result_buff, 4, 0x11);
        int result = (ProcAddGetInfo)(argv[1], &result_buff, 4, 0x11);
        std::cout << "Harness function call complete...!\n";
        printf("Database GetInfoW result: %ld\n", result);
    }
}**/


int main(int argc, char** argv)
{

    if (argc < 2) {
        printf("Usage: %s <ESE DB file>\n", argv[0]);
        return 0;
    }

    // DLL Handle
    HINSTANCE hinstLib;
    JET_ERR result;
    hinstLib = LoadLibrary(TEXT("esent.dll"));
    //fuzz(argc, argv);  
    
    GetFileInfoA ProcAddGetInfo;
    void* result_buff;
    result_buff = (void*)malloc(200); // This is hacky and should be fixed appropriatley
    ProcAddGetInfo = (GetFileInfoA)GetProcAddress(hinstLib, "JetGetDatabaseFileInfoA");
    if (NULL != ProcAddGetInfo)
    {
        //result = (ProcAddGetInfo)(argv[1], &result_buff, 4, 0x11);
        int result = (ProcAddGetInfo)(argv[1], &result_buff, 4, 0x11);
        std::cout << "Harness function call complete...!\n";
        printf("Database GetInfoW result: %ld\n", result);
    }

    // Free the DLL module.
    //free(result_buff);
    FreeLibrary(hinstLib); 

}
