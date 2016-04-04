#include "Accessor.h"


/*Accessor Class Mother Class for other Classes*/
class Accessor {
	int blocksRead;
	int fileDesc;
	int recordNumber;
protected:
	int fieldSize;
	int iterator;
	int currBlock;
	void* currMem;

	int IndexToBlock (int n);
	int IndexToOffset (int n);
	void* AddOffset (void* mem, int offset);
	void* GetPointerToCurrent ();
public:
	Accessor (int fD, int fSize);
	
	int InitFile ();
	void IteratorSet ();
	int Next ();
	virtual int CompareCurrent (void* value) = 0;
	virtual int PrintCurrent () = 0;
	int Insert (void* value);
	int GetBlockReadCounter () {return blocksRead;}
};

/*Int Accessor Class Derives from the Mother Class Accessor*/
class IntAccessor : public Accessor {
public:
	IntAccessor (int fD) : Accessor (fD, sizeof(int)) {}
	
	int CompareCurrent (void* value);
	int PrintCurrent ();
};

/*String Acccessor Class - Derives from the Mother Class Accessor*/
class StrAccessor : public Accessor {
public:
	StrAccessor (int fD, int fSize) : Accessor (fD, fSize) {}
	

	int CompareCurrent (void* value);
	int PrintCurrent ();
};


int Header_Init(HeaderInfo* header_info,char** fieldNames, char *dbDirectory){
	header_info->fieldNumber = 6;
	strcpy (header_info->dir, dbDirectory);
	/*No problem because the fieldnames argument will be global*/
	for(int i=0; i<header_info->fieldNumber; i++){
		strcpy (header_info->fieldNames[i], fieldNames[i]);

		char *name = header_info->fieldNames[i];
		char path[strlen(name)+strlen(dbDirectory)+2];
		sprintf(path,"%s/%s",dbDirectory,name);
		printf ("Creating %s\n", path);
		if((header_info->AccessorInfo[i].fileDesc = BF_CreateFile(path))<0){
		    CS_errno = CSE_CANNOTCREATEFILE;
			return CSE_CANNOTCREATEFILE;
		}
		switch (i) {
			case 0:
			header_info->AccessorInfo[i].type = 'i';
			header_info->AccessorInfo[i].size = sizeof(int);
			break;
			case 1:
			header_info->AccessorInfo[i].type = 'c';
			header_info->AccessorInfo[i].size = 15;
			break;
			case 2:
			header_info->AccessorInfo[i].type = 'c';
			header_info->AccessorInfo[i].size = 20;
			break;
			case 3:
			header_info->AccessorInfo[i].type = 'c';
			header_info->AccessorInfo[i].size = 1;
			break;
			case 4:
			header_info->AccessorInfo[i].type = 'c';
			header_info->AccessorInfo[i].size = 11;
			break;
			case 5:
			header_info->AccessorInfo[i].type = 'i';
			header_info->AccessorInfo[i].size = sizeof(int);
			break;
			default:
			break;
		}
	}
	return CSE_OK;
}


/*Accessor Class Functions*/
Accessor::Accessor (int fD, int fSize) : fileDesc(fD), fieldSize (fSize), blocksRead(0), currBlock (-1), currMem (NULL) {
	if (BF_GetBlockCounter (fileDesc) > 0) {
		//printf ("Opening an initialized file\n");
		void* buffer;
		if (BF_ReadBlock (fileDesc, 0, &buffer) < 0) {
			CS_errno = CSE_INCOMPLETEREAD;
		}
		AccessData accessData;
		memcpy (&accessData, buffer, sizeof(AccessData));
		recordNumber = accessData.recordNumber;
	} else {
		recordNumber = -1;
	}
}

/*Initialize a file with header block*/
int Accessor::InitFile () {
	if (BF_GetBlockCounter (fileDesc) != 0) {
		CS_errno = CSE_BLOCKALREADYEXISTS;
		return CSE_BLOCKALREADYEXISTS;
	}
	//printf ("Allocating first block of file\n");
	if (BF_AllocateBlock (fileDesc) < 0) {
		CS_errno = CSE_CANNOTALLOCATEBLOCK;
		return CSE_CANNOTALLOCATEBLOCK;
	}
	AccessData accessData;
	accessData.recordNumber = 0;
	accessData.size = fieldSize;
	accessData.verification = 'Y';
	void* buffer;
	//printf ("Writing header metadata\n");
	if (BF_ReadBlock (fileDesc, 0, &buffer) < 0) {
		CS_errno = CSE_INCOMPLETEREAD;
		return CSE_INCOMPLETEREAD;
	}
	memcpy (buffer, &accessData, sizeof(AccessData));
	if (BF_WriteBlock (fileDesc, 0) < 0) {
		CS_errno = CSE_INCOMPLETEWRITE;
		return CSE_INCOMPLETEWRITE;
	}
	return 0;
}
/*set iterator before start*/
void Accessor::IteratorSet () {
	iterator = -1;
}
/*iterator to next record*/
int Accessor::Next () {
	if ((iterator+1) >= recordNumber) {
		return -1;
	}
	iterator++;
	return 0;
}
/*insert element at end*/
int Accessor::Insert (void* value) {
	int newindex = recordNumber;
	int block = IndexToBlock (recordNumber);
	int offset = IndexToOffset (recordNumber);
	/*open the block to insert to*/
	if (block == BF_GetBlockCounter(fileDesc)) {
		if (BF_AllocateBlock (fileDesc) < 0) {
			CS_errno = CSE_CANNOTALLOCATEBLOCK;
			return CSE_CANNOTALLOCATEBLOCK;
		}
	}

	if (block != currBlock) {
		//printf ("Moving to a new block: from %d to %d\n", currBlock, block);
		currBlock = block;
		if (BF_ReadBlock(fileDesc, block, &currMem) < 0) {
			CS_errno = CSE_INCOMPLETEREAD;
			return CSE_INCOMPLETEREAD;
		}
	}
	/*actual insertion here*/
	memcpy (AddOffset (currMem, offset), value, fieldSize);
	memcpy (AddOffset (currMem, offset+fieldSize), &newindex, sizeof(int));
	if (BF_WriteBlock (fileDesc, block) < 0) {
		CS_errno = CSE_INCOMPLETEWRITE;
		return CSE_INCOMPLETEWRITE;
	}

	recordNumber++;
	/*header is updated*/
	AccessData accessData;
	void* buffer;
	if (BF_ReadBlock (fileDesc, 0, &buffer) < 0) {
		CS_errno = CSE_INCOMPLETEREAD;
		return CSE_INCOMPLETEREAD;
	}
	memcpy (&accessData, buffer, sizeof(AccessData));
	accessData.recordNumber++;
	memcpy (buffer, &accessData, sizeof(AccessData));
	if (BF_WriteBlock (fileDesc, 0) < 0) {
		CS_errno = CSE_INCOMPLETEWRITE;
		return CSE_INCOMPLETEWRITE;
	}

	return 0;
}
/*functions for accesing blocks*/


int Accessor::IndexToBlock (int n) {
	int pairLength = fieldSize+sizeof(int);
	int recPerBlock = BLOCK_SIZE/pairLength;
	return n/recPerBlock+1;
}
int Accessor::IndexToOffset (int n) {
	int pairLength = fieldSize+sizeof(int);
	int recPerBlock = BLOCK_SIZE/pairLength;
	return (n%recPerBlock)*pairLength;
}
void* Accessor::AddOffset (void* mem, int offset) {
	char* copyPtr = (char*) mem;
	char* shiftedPtr = copyPtr+offset;
	return (void*) shiftedPtr;
}
void* Accessor::GetPointerToCurrent () {
	if (iterator == -1 || iterator == recordNumber) {
		CS_errno = CSE_INTERNALERROR;
		return NULL;
	}

	int block = IndexToBlock (iterator);
	int offset = IndexToOffset (iterator);

	if (block != currBlock) {
		blocksRead++;
		currBlock = block;
		if (BF_ReadBlock(fileDesc, block, &currMem) < 0) {
			CS_errno = CSE_INCOMPLETEREAD;
			return NULL; 
		}
	}

	return AddOffset (currMem, offset);
}

/*Int Accessors - Derives from Accessor Class*/
int IntAccessor::CompareCurrent (void* value) {
	int val = *((int*) value);
	int* curr = (int*) GetPointerToCurrent ();
	return *curr-val;
}
int IntAccessor::PrintCurrent () {
	int* curr = (int*) GetPointerToCurrent ();
	printf ("%d", *curr);
	return 0;
}

/*String Accessor - Child of th Accessor Class*/
int StrAccessor::CompareCurrent (void* value) {
	char* val = (char*) value;
	char* curr = (char*) GetPointerToCurrent ();
	if (fieldSize == 1)
		return curr[0]-val[0];
	else
		return strcmp (curr, val);
}
int StrAccessor::PrintCurrent () {
	char* curr = (char*) GetPointerToCurrent ();
	if (fieldSize == 1)
		putchar (*curr);
	else
		printf ("\"%s\"", curr);
	return 0;
}


/*Column Accessor - Initial Class*/
ColumnAccessor::ColumnAccessor (HeaderInfo* header_info) {
	
	header = header_info;
	fieldNumber = header_info->fieldNumber;
	accessor = new Accessor* [fieldNumber];
	
	for (int i = 0; i < fieldNumber; i++)
		if (header_info->AccessorInfo[i].type == 'i')
			accessor[i] = new IntAccessor (header_info->AccessorInfo[i].fileDesc);
		else if (header_info->AccessorInfo[i].type == 'c')
			accessor[i] = new StrAccessor (header_info->AccessorInfo[i].fileDesc, header_info->AccessorInfo[i].size);

	printable = NULL;
	comparable = -1;
}

ColumnAccessor::~ColumnAccessor () {
	for (int i = 0; i < fieldNumber; i++)
		delete accessor[i];
	delete[] accessor;

	if (printable != NULL)
		delete[] printable;
}

/*initialize files*/
int ColumnAccessor::InitColumnFiles () {
	for (int i = 0; i < fieldNumber; i++) {
		if (accessor[i]->InitFile () < 0)
			return -1;
	}
	return 0; 
}
/*open the files pointed to by the header info*/
int ColumnAccessor::OpenColumnFiles (HeaderInfo* header_info, int verify) {
	for (int i = 0; i < header_info->fieldNumber; i++) {
		char *name = header_info->fieldNames[i];
		char path[strlen(name)+strlen(header_info->dir)+2];
		sprintf(path,"%s/%s",header_info->dir,name);
		int fl = BF_OpenFile (path);
		if (fl < 0) {
			CS_errno = CSE_CANNOTOPENFILE;
			return CSE_CANNOTOPENFILE;
		}
		header_info->AccessorInfo[i].fileDesc = fl;
		header_info->AccessorInfo[i].Open = 1;
		/*verify if this is actually a column file*/
		if (verify) {
			void* buffer;
			fl = BF_ReadBlock (header_info->AccessorInfo[i].fileDesc, 0, &buffer);
			if (fl < 0) {
				CS_errno = CSE_INCOMPLETEREAD;
				return CSE_INCOMPLETEREAD;
			}
			AccessData accessData;
			memcpy (&accessData, buffer, sizeof(AccessData));
			if (accessData.verification != 'Y')  {
				CS_errno = CSE_NOTCOLUMNFILE;
				return CSE_NOTCOLUMNFILE;
			}
		}
	}
	return 0;
}
/*close all column files*/
int ColumnAccessor::CloseColumnFiles (HeaderInfo* header_info) {
	for (int i = 0; i < header_info->fieldNumber; i++) {
		printf ("Closinging column file: %s\n", header_info->fieldNames[i]);
		int fl = BF_CloseFile (header_info->AccessorInfo[i].fileDesc);
		if (fl < 0) {
			CS_errno = CSE_CANNOTCLOSEFILE;
			return CSE_CANNOTCLOSEFILE;
		}
		header_info->AccessorInfo[i].Open = 0;
	}
	return 0;
}

#define NOCOMP -1
#define PRINTALL -2 

/*set field for comparison*/
int ColumnAccessor::SetFieldCheck (char* fieldName) {
	if (fieldName == NULL) {
		comparable = PRINTALL;
		return 0;
	}
	comparable = NOCOMP;
	for (int i = 0; i < fieldNumber; i++)
		if (strcmp (header->fieldNames[i], fieldName) == 0) {
			comparable = i;
			return 0;
		}
	CS_errno = CSE_UNKNOWNFIELD;
	return CSE_UNKNOWNFIELD;
}
/*set column to be printed*/
int ColumnAccessor::SetPrint (char** fieldName, int n) {
	printable = new int [n];
	printNumber = n;
	for (int i = 0; i < printNumber; i++) {
		int found = 0;
		for (int j = 0; j < fieldNumber; j++)
			if (strcmp (header->fieldNames[j], fieldName[i]) == 0) {
				found = 1;
				printable[i] = j;
				break;
			}
		if (found == 0) {
			CS_errno = CSE_UNKNOWNFIELD;
			delete[] printable;
			return CSE_UNKNOWNFIELD;
		}
	}
	return 0;
}
/*set record iteration*/
void ColumnAccessor::IterationSet () {
	for (int i = 0; i < fieldNumber; i++)
		accessor[i]->IteratorSet ();
}
/*iterator to next record*/
int ColumnAccessor::Next () {
	int end = 0;
	for (int i = 0; i < fieldNumber; i++)
		end = end + accessor[i]->Next ();
	if (end) {
		return -1;
	} else
		return 0;
}
/*compare value with set up record*/
int ColumnAccessor::CompareCurrent (void* value,bool *flag) {
	if (comparable == NOCOMP) {
		CS_errno = CSE_COMPARABLE;
		*flag = true;
		return CSE_COMPARABLE;
	} else if (comparable == PRINTALL) {
		if (value != NULL) {
			CS_errno = CSE_COMPARABLE;
			*flag = true;
			return CSE_COMPARABLE;
		}
		return 0;
	}
	return accessor[comparable]->CompareCurrent(value);
}
/*print record*/
int ColumnAccessor::PrintCurrent () {
	if (printable == NULL) {
		CS_errno = CSE_PRINTABLE;
		return CSE_PRINTABLE;
	}
	for (int i = 0; i < printNumber; i++) {
		accessor[printable[i]]->PrintCurrent();
		if (i != printNumber-1)
			printf (",");
	}
	printf("\n");
	return 0;
}

/*insert record to column files*/
int ColumnAccessor::InsertRecord (Record record) {
	//printf ("Inserting record\n");
	int fail = 0;
	fail += accessor[0]->Insert (&(record.id));
	fail += accessor[1]->Insert (record.name);
	fail += accessor[2]->Insert (record.surname);
	fail += accessor[3]->Insert (record.status);
	fail += accessor[4]->Insert (record.dateOfBirth);
	fail += accessor[5]->Insert (&(record.salary));
	if (fail) {
		printf ("Insertion failure\n");
		return -1;
	} else
		return 0;
}
/*get number of blocks read*/
int ColumnAccessor::GetBlockReadCounter () {
	int s = 0;
	for (int i = 0; i < fieldNumber; i++)
		s += accessor[i]->GetBlockReadCounter ();
	return s;
}
