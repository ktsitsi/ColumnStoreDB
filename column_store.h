#ifndef __CS__
#define __CS__

#include "Accessor.h"
#include "BF.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "error.h"

void CS_Init();
int CS_CreateFiles(char** fieldNames,char* dbDirectory);
int CS_OpenFile(HeaderInfo* header_info,char* dbDirectory);
int CS_CloseFile(HeaderInfo* header_info);
int CS_InsertEntry (HeaderInfo* header_info, Record record);
void CS_GetAllEntries (HeaderInfo* header_info, char* fieldName, void* value, char** fieldNames, int n);



#endif
