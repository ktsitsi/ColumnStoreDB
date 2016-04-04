#include "error.h"

/* Prints messages for types of errors */
void CS_PrintError(const char *errString){
	if (errString!=NULL)
		fprintf(stderr,"%s\n",errString);
	const char *msg;
	switch (CS_errno) {	
		case CSE_OK:
		msg = "Everything went okay.\n";
		break;
		case CSE_CANNOTCREATEFILE :
		BF_PrintError("CS_Error: failed to BF_CreateFile!\n");
		msg = "Description:-Could not create file.\n";
		break;
		case CSE_CANNOTOPENFILE :
		BF_PrintError("CS_Error: failed to BF_OpenFile!\n");
		msg = "Description:-Could not open file.\n";
		break;
		case CSE_CANNOTCLOSEFILE :
		BF_PrintError("CS_Error: failed to BF_CloseFile!\n");
		msg = "Description:-Could not close file.\n";
		break;
		case CSE_CANNOTALLOCATEBLOCK :
		BF_PrintError("CS_Error: failed to BF_AllocateBlock!\n");
		msg = "Description:-Could not allocate block.\n";
		break;
		case CSE_INCOMPLETEREAD :
		BF_PrintError("CS_Error: failed to BF_ReadBlock!\n");
		msg = "Description:-Could not read block.\n";
		break;
		case CSE_INCOMPLETEWRITE :
		BF_PrintError("CS_Error: failed to BF_WriteBlock!\n");
		msg = "Description:-Could not write block.\n";
		break;
		case CSE_CANNOTCREATEDIR  :					//It's not a BF_Error but a CS
		msg = "Description:-Could not create directory.\n";
		break;
		case CSE_BLOCKALREADYEXISTS :
		BF_PrintError("CS_Error: Initial block exists. Cannot initialize!\n");
		msg = "Description:-Initial block exists. Cannot initialize.\n";
		break;
		case CSE_INTERNALERROR :					//It's not a BF_Error but a CS
		msg = "Description:-An unexpected internal situation occured.\n";
		break;
		case CSE_NOTCOLUMNFILE :					//It's not a BF_Error but a CS
		msg = "Description:-The file is not a valid column file";
		break;
		case CSE_UNKNOWNFIELD :						//It's not a BF_Error but a CS
		msg = "Description:-This field does not exist.\n";
		break;
		case CSE_PRINTABLE :						//It's not a BF_Error but a CS
		msg = "Description:-Error in choice of fields to be print.\n";
		break;
		case CSE_COMPARABLE :						//It's not a BF_Error but a CS
		msg = "Description:-Error in choosing field for comparison.\n";
		break;
		case CSE_NULLARGUMENT :						//It's a generic error
		msg = "Description:-Null argument given!\n";
		break;
		case CSE_DIRALREADYEXISTS :					//Error in Creation
		msg = "CS_Create Error: Directory already exists!\n";
		break;
		default:
		return;
	}
	fprintf(stderr,"%s\n",msg);
}

