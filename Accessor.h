#ifndef __ACCESSOR__
#define __ACCESSOR__


#include "rec.h"
#include "BF.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "error.h"

#define MAXNAME 20
#define MAXDIRNAME 50   // Sufficient for long absolute paths.
#define MAXFIELDS 10

typedef struct HeaderStruct{
	int fileDesc;
	char type;
	int size;
	int Open;
}HeaderStruct;

typedef struct HeaderInfo{
	HeaderStruct AccessorInfo[MAXFIELDS];
	char fieldNames[MAXFIELDS][MAXNAME];
	char dir[MAXDIRNAME];
	int fieldNumber;
}HeaderInfo;

struct AccessData {
	int recordNumber;
	int size;
	char verification;
};

int Header_Init(HeaderInfo* header_info,char** fieldNames, char *dbDirectory);

class Accessor;

/*Column Accessor - Initial Class*/
class ColumnAccessor {
	HeaderInfo* header;
	int fieldNumber;
	Accessor** accessor;
	int printNumber;
	int* printable;
	int comparable;

	int GetFileIndex (char* fileName);
public:
	ColumnAccessor (HeaderInfo* header_info);
	~ColumnAccessor ();

	int InitColumnFiles (); /*Use in CreateFiles to initialize first blocks of the files. 0 in success, -1 in fail*/
	static int OpenColumnFiles (HeaderInfo* header_info, int verify = 0); /*Use in OpenFiles to open the files. verify checks if it is our file, 0 when creating*/
	static int CloseColumnFiles (HeaderInfo* header_info); /*Use in CloseFiles to close the files*/
	int SetFieldCheck (char* fieldName); /*select the field used for comparisons, 0 in success, -1 in fail*/
	int SetPrint (char** fieldName, int n); /*select fields to be printed when using print, 0 in success, -1 in fail*/
	void IterationSet (); /*reset iteration to start of file*/
	int Next (); /*move iteration to next. 0 if ok, -1 if end is reached*/
	int CompareCurrent (void* value, bool *flag); /*compare current element with value. basically cmp (current, value). flag is true in case of error*/
	int PrintCurrent (); /*print current element*/
	int InsertRecord (Record record); /*0 in success, -1 in fail*/
	int GetBlockReadCounter (); /*see how many blocks have been read*/
};



#endif
