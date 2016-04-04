#include "column_store.h"

int CS_errno;


/*CS_Fucntions*/

/*This function initialiaze the Column Store Level*/
void CS_Init(){
	BF_Init ();
}

/*This function has to create a directory where the files should store
All the functions called by this function are obvious as BF library is used
The function initialize the header_info file but also the column files that are
created to their initial values*/
int CS_CreateFiles(char** fieldNames,char* dbDirectory){
	void* buffer;
	int headi_id;
	if(dbDirectory == NULL){											/*If directory is null then return error*/
		CS_errno = CSE_NULLARGUMENT;
		return CSE_NULLARGUMENT;
	}
	CS_Init();
	struct stat fileStat = {0};				// Structure used by stat call
	int status;							
	if (stat(dbDirectory,&fileStat)<0){		// If directory does not already exist.
		status = mkdir(dbDirectory,0777);	// Create the directory.
		if (status <0) {					// If creation failed, report.
			CS_errno = CSE_CANNOTCREATEDIR;
			return CSE_CANNOTCREATEDIR;
		}
	} else {								// Directory already exists.
		CS_errno = CSE_DIRALREADYEXISTS;
		return CSE_DIRALREADYEXISTS;
	}
	char const *name = "Header_Info";
	char path[strlen(name)+strlen(dbDirectory)+2]; 
	sprintf(path,"%s/%s",dbDirectory,name);		// Make path for Header_Info

	if(BF_CreateFile(path)<0){
		CS_errno = CSE_CANNOTCREATEFILE;
		return CSE_CANNOTCREATEFILE;
	}
	if((headi_id = BF_OpenFile(path))<0){
		CS_errno = CSE_CANNOTOPENFILE;
		return CSE_CANNOTOPENFILE;
	}
	if(BF_AllocateBlock(headi_id)<0){
		CS_errno = CSE_CANNOTALLOCATEBLOCK;
		return CSE_CANNOTALLOCATEBLOCK;
	}
	if(BF_ReadBlock(headi_id,0,&buffer)<0){
		CS_errno = CSE_INCOMPLETEREAD;
		return CSE_INCOMPLETEREAD;
	}
	HeaderInfo* headi = new HeaderInfo;
	if(Header_Init(headi,fieldNames,dbDirectory)<0) return CSE_CANNOTCREATEFILE;
	memcpy(buffer,headi,sizeof(HeaderInfo));
	if(BF_WriteBlock(headi_id,0)<0){
		CS_errno = CSE_INCOMPLETEWRITE;
		return CSE_INCOMPLETEWRITE;
	}
	if(BF_CloseFile(headi_id)<0){
		CS_errno = CSE_CANNOTCLOSEFILE;
		return CSE_CANNOTCLOSEFILE;
	}
	if(ColumnAccessor::OpenColumnFiles (headi) < 0){
		CS_errno = CSE_CANNOTOPENFILE;
		return CSE_CANNOTOPENFILE;
	}
	ColumnAccessor accessor = ColumnAccessor (headi);
	accessor.InitColumnFiles ();
	if(ColumnAccessor::CloseColumnFiles (headi) < 0){
		CS_errno = CSE_CANNOTCLOSEFILE;
		return CSE_CANNOTCLOSEFILE;
	}
	CS_errno = CSE_OK;
	return CSE_OK;
}

/*This Function is responsible for opening the files that are stored in 
the particualr directory that has been given. The functionality of the inner function that
are used is obvious as the BF level is used.*/

int CS_OpenFile(HeaderInfo* header_info,char* dbDirectory){
	int headi_id;
	void* buffer;
	if(dbDirectory == NULL){    // If string for directory is null.
		CS_errno = CSE_NULLARGUMENT;
		return CSE_NULLARGUMENT;
	}
	const char* name = "Header_Info";
	char path[strlen(name)+strlen(dbDirectory)+2];
	sprintf(path,"%s/%s",dbDirectory,name);			// Path for the Header_Info file.
	if((headi_id = BF_OpenFile(path)) < 0){
		CS_errno = CSE_CANNOTOPENFILE;
		return CSE_CANNOTOPENFILE;
	}
	if(BF_ReadBlock(headi_id,0,&buffer)<0){
		CS_errno = CSE_INCOMPLETEREAD;
		return CSE_INCOMPLETEREAD;
	}
	memcpy(header_info,buffer,sizeof(HeaderInfo));
	if(ColumnAccessor::OpenColumnFiles (header_info, 1) < 0){
		CS_errno = CSE_CANNOTOPENFILE;
		return CSE_CANNOTOPENFILE;
	}
	CS_errno = CSE_OK;
	return CSE_OK;
}

/*This Function is responsible for Closing all the files that the HeaderInfo file 
has as "members" -> the column files*/

int CS_CloseFile(HeaderInfo* header_info){
	if(header_info == NULL){
		CS_errno = CSE_NULLARGUMENT;
		return CSE_NULLARGUMENT;
	}
	if(ColumnAccessor::CloseColumnFiles (header_info) < 0){
		CS_errno = CSE_CANNOTOPENFILE;
		return CSE_CANNOTOPENFILE;
	}
	CS_errno = CSE_OK;
	return CSE_OK;
}

/* Inserts record in column store files. Returns 0 in case of success / -1 in case of error. */
int CS_InsertEntry (HeaderInfo* header_info, Record record) {
	ColumnAccessor accessor = ColumnAccessor (header_info);		// Make column accessor object and initialize with header_info.
	return accessor.InsertRecord (record);						// Insert record and return result.
}

/* Prints every record of field fieldName with a certain value. If both of them are null, every record is printed.
   Fields printed are specified in fieldNames (number n)
   Prints number of record blocks we read during search */
void CS_GetAllEntries (HeaderInfo* header_info, char* fieldName, void* value, char** fieldNames, int n) {
	ColumnAccessor accessor = ColumnAccessor (header_info);		// Make and initialize column accessor.
	if (accessor.SetFieldCheck (fieldName) <0) return;			// Set field for comparison.    
	if (accessor.SetPrint (fieldNames, n) <0) return;			// Set fields to be printed.
	accessor.IterationSet ();									// Initialize iteration to the start of file.
	while (accessor.Next () == 0) {								// While there are still records to check (did not reach end of files).
		bool flag = false;										// Flag for errors in CompareCurrent.
		if (accessor.CompareCurrent (value,&flag) == 0){		// If we found result, print it.
			if (accessor.PrintCurrent() <0) return;
		} else {
			if (flag == true) return;							// Error occurred in CompareCurent.
		} 
	}
	printf ("Blocks read: %d\n", accessor.GetBlockReadCounter ());  // Print number of record blocks read during search.
	CS_errno = CSE_OK;		// Everything went okay.
}


