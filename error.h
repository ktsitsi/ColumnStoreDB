#include <cstdio>
#include "BF.h"

/* Error codes used in PrintError */
#define CSE_OK 0
#define CSE_CANNOTCREATEFILE -1
#define CSE_CANNOTOPENFILE -2
#define CSE_CANNOTALLOCATEBLOCK -3
#define CSE_INCOMPLETEREAD -4
#define CSE_INCOMPLETEWRITE -5
#define CSE_CANNOTCLOSEFILE -6
#define CSE_CANNOTCREATEDIR -7

#define CSE_BLOCKALREADYEXISTS -8
#define CSE_INTERNALERROR -9
#define CSE_NOTCOLUMNFILE -10
#define CSE_UNKNOWNFIELD -11
#define CSE_COMPARABLE -13
#define CSE_PRINTABLE -14
#define CSE_NULLARGUMENT -15
#define CSE_DIRALREADYEXISTS -16



extern int CS_errno;	// Last error detected.

void CS_PrintError(const char *errString);  // Prints a custom message and another one, specific for the last error.
