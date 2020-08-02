// EsentUtilHarness.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h> 
#include <stdio.h>
#include <esent.h>


typedef int(__stdcall* CreateInstance)(JET_INSTANCE* pinstance, const char* szInstanceName);
typedef int(__stdcall* SetSystemParameter)(JET_INSTANCE* pinstance, JET_SESID sesid, unsigned long paramid, JET_API_PTR lParam, JET_PCSTR szParam);
typedef int(__stdcall* Init)(JET_INSTANCE* pinstance);
typedef int(__stdcall* BeginSession)(JET_INSTANCE pinstance,JET_SESID * sesid, JET_PCSTR szUserName, JET_PCSTR *zPassword);
typedef int(__stdcall* CreateDatabase)(JET_SESID sesid, JET_PCSTR szFilename, JET_PCSTR szConnect, JET_DBID *pdbid, JET_GRBIT grbit);
typedef int(__stdcall* CreateTableIndex)(JET_SESID sesID, JET_DBID dbid, JET_TABLECREATE* ptablecreate);


//Harness needs to work through the following to create a sample db 
// JetCreateInstance
// JetSetSystemParameter
// JetInit
// JetBeginSession
// JetCreateDatabase 


// We only actually want to fuzz JetGetDatabaseFileInforW at this point
// This preamble to is to create a valid test file!


int main()
{

    // DLL Handle
    HINSTANCE hinstLib;

    // Various required function pointer handles
    CreateInstance ProcCreateInstance;
    //SetSystemParameter ProcSetSystemParameter;
    Init ProcInit;
    BeginSession ProcBeginSession;
    CreateDatabase ProcAddCreate;

    CreateTableIndex ProcCreateTable;
    // Util variables
    JET_ERR result;
    //BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;
    void* result_buff;


    //runtime DLL Load for function search
    hinstLib = LoadLibrary(TEXT("esent.dll"));
    if (hinstLib != NULL)
    {
        //JetCreateInstance
        JET_INSTANCE instance = JET_instanceNil;// (JET_INSTANCE*)malloc(sizeof(JET_INSTANCE));
        const char* instanceName = "TestDummyDBFuzzUNIT";
        // Locate and utilise JetCreateDatabase function at runtime
        ProcCreateInstance = (CreateInstance)GetProcAddress(hinstLib, "JetCreateInstance");
        result_buff = (void*)malloc(500);
        if (NULL != ProcCreateInstance)
        {
            std::cout << "Found CreateInstance Function!\n";
            result = ProcCreateInstance(&instance, instanceName);
            printf("JetCreateInstance result: %ld\n", result);
        }
        free(result_buff);

        //JetCreateSystemParameter
        // Locate and utilise JetSetSystemParameter function at runtime
        /**ProcSetSystemParameter = (SetSystemParameter)GetProcAddress(hinstLib, "JetSetSystemParameter");
        result_buff = (void*)malloc(500);

        JET_SESID sesID = 0L;
        if (NULL != ProcSetSystemParameter)
        {
            std::cout << "Found JetSetSystemParameter Function!\n";
            result = ProcSetSystemParameter(instance, NULL,,,);
            printf("JetSetSystemParameter result: %ld\n", result);
        }
        free(result_buff);**/

        //Jetinit
        ProcInit = (Init)GetProcAddress(hinstLib, "JetInit");
        result = ProcInit(&instance);
        printf("Jetinit result: %ld\n", result);


        //JetBeginSession
        JET_SESID sesID = JET_sesidNil;
        ProcBeginSession = (BeginSession)GetProcAddress(hinstLib, "JetBeginSession");
        result = ProcBeginSession(instance, &sesID, 0, 0);
        printf("JetBeginSession result: %ld\n", result);

        //JetCreateDatabase
        JET_DBID dbID = JET_dbidNil;
        ProcAddCreate = (CreateDatabase)GetProcAddress(hinstLib, "JetCreateDatabase");
        ProcAddCreate(sesID, "C:\\Users\\IEUser\\Desktop\\TEST_CORPUS.db", 0, &dbID, 0);
        printf("JetCreateDatabase result: %ld\n", result);

        /** Create Table and columns with Index
        /JET_COLUMNCREATE columnCreate[2] = { 0 };

        const char* name = "PK";
        columnCreate[0].cbStruct = sizeof(JET_COLUMNCREATE);
        strcpy_s(columnCreate[0].szColumnName,3, "PK");
        //columnCreate[0].szColumnName = static_cast<const char*>("PK");
        columnCreate[0].coltyp = JET_coltypLong;
        columnCreate[0].grbit = JET_bitColumnAutoincrement;
        columnCreate[0].err = JET_errSuccess;

        columnCreate[1].cbStruct = sizeof(JET_COLUMNCREATE);
        strcpy_s(columnCreate[1].szColumnName,6, "Value");
        //columnCreate[1].szColumnName = "Value";
        columnCreate[1].coltyp = JET_coltypLongText;
        columnCreate[1].cbMax = 1024;
        columnCreate[1].grbit = JET_bitColumnTagged;
        columnCreate[1].cp = 1200;
        columnCreate[1].err = JET_errSuccess;

        JET_INDEXCREATE indexCreate[2] = { 0 };
        indexCreate[0].cbStruct = sizeof(JET_INDEXCREATE);
        strcpy_s(indexCreate[0].szIndexName,9,"PK_Index");
        strcpy_s(indexCreate[0].szKey,4,"+PK\0");
        //indexCreate[0].szIndexName = "PK_index";
        //indexCreate[0].szKey = "+PK\0";
        indexCreate[0].cbKey =
            static_cast<unsigned long>(::strlen(indexCreate[0].szKey) + 2);
        indexCreate[0].grbit = JET_bitIndexPrimary;
        indexCreate[0].err = JET_errSuccess;

        indexCreate[1].cbStruct = sizeof(JET_INDEXCREATE);
        strcpy_s(indexCreate[1].szIndexName,12,"Value_index");
        strcpy_s(indexCreate[1].szKey,7, "+Value\0");
        //indexCreate[1].szIndexName = "Value_index";
        //indexCreate[1].szKey = "+Value\0";
        indexCreate[1].cbKey =
            static_cast<unsigned long>(::strlen(indexCreate[1].szKey) + 2);
        indexCreate[1].grbit = JET_bitIndexUnique;
        indexCreate[1].err = JET_errSuccess;

        JET_TABLECREATE tableCreate = { 0 };
        tableCreate.cbStruct = sizeof(tableCreate);
        strcpy_s(tableCreate.szTableName, 10,"TestTable");
        //tableCreate.szTableName = "TestTable";
        tableCreate.rgcolumncreate = columnCreate;
        tableCreate.cColumns =
            sizeof(columnCreate) / sizeof(columnCreate[0]);
        tableCreate.rgindexcreate = indexCreate;
        tableCreate.cIndexes =
            sizeof(indexCreate) / sizeof(indexCreate[0]);
        tableCreate.tableid = JET_tableidNil;

        ProcCreateTable = (CreateTableIndex)GetProcAddress(hinstLib, "JetCreateTableColumnIndex");
        result = ProcCreateTable(sesID, dbID, &tableCreate);
        
        /** Locate and utilise JetGetDatabaseInfoW function at runtime
        ProcAddGetInfo = (GetFileInfow) GetProcAddress(hinstLib, "JetGetDatabaseFileInfoA");

        // If the function address is valid, call the function.
        result_buff = (void*)malloc(500);

        if (NULL != ProcAddGetInfo)
        {
            std::cout << "Found DLL Function!\n";
            result = (ProcAddGetInfo)("C:\\Users\\IEUser\\Desktop\\Biblio.mdb",&result_buff,4,0x11);
            std::cout << "Harness function call complete...!\n";
            //std::cout << "Database GetInfoW result: %lu\n" << result;
            printf("Database GetInfoW result: %ld\n",result);
        }
        // Free the DLL module.
        free(result_buff);
        fFreeResult = FreeLibrary(hinstLib);**/
    }
    //exit(1);
}

