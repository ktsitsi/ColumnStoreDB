#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "column_store.h"


char* ReadLine (FILE* f);
void PrintMenu ();
Record RecordParse(char* buf, int* flag);

int main() {
	int fileDesc;
	char** fieldNames = new char*[6];
	char** customfieldNames = new char*[6];
	for (int i = 0; i < 6; i++)
		fieldNames[i] = new char[15];
	strcpy(fieldNames[0], "id");
	strcpy(fieldNames[1], "name");
	strcpy(fieldNames[2], "surname");
	strcpy(fieldNames[3],"status");
	strcpy(fieldNames[4], "dateOfBirth");
	strcpy(fieldNames[5], "salary");

	HeaderInfo header;

	char* pathname = NULL;
	char* input;
	char* fn;
	char* b, *line,*p;
	
	int j,num,fl;
	int openflag = 0;
	int pflag = 0;
	Record rec;
	/*Initialize block level*/
	CS_Init ();
	/*Control-loop for testing*/
	while (1) {
		/*Print menu*/
		PrintMenu();
		/*Get input, you only need the first character*/
		input = ReadLine(stdin);
		char c = input[0];
		free(input);
		switch (c) {
			case 'A':{
				printf("Type the pathname of the new file\n");
				/*String are dynamically allocated so free is needed*/
				if (pathname != NULL)
					free(pathname);
				pathname = ReadLine(stdin);
				break;
			}
			case 'B': {
				if(pathname != NULL){
					printf("%s\n", pathname);
				}
				else{
					perror("Error: No pathname was given - thus couldn't print!\n");
				}
				break;
			}
			case 'C':{
				printf("Creating file with pathname %s...\n", pathname);
				fl = CS_CreateFiles(fieldNames, pathname);
				if(fl == CSE_NULLARGUMENT){
					printf("Give a pathname and try again!\n");
				}
				else if (fl < 0){
					CS_PrintError("Error in CS_CreateFiles\n");
					exit(EXIT_FAILURE);
				}
				break;
			}
			case 'D':{
				printf("Opening file with pathname %s...\n", pathname);
				fileDesc = CS_OpenFile(&header, pathname);
				if (fileDesc == CSE_NULLARGUMENT){
					CS_PrintError("You haven't set a pathname and haven't created files.\n");
				}
				else if (fileDesc < 0){
					CS_PrintError("Error in CS_OpenFile\n");
					exit(EXIT_FAILURE);
				}
				else{
					openflag = 1;
				}
				break;
			}
			case 'E':{
				printf("Closing file with pathname %s...\n", pathname);
				if(pathname != NULL){
					fl = CS_CloseFile(&header);
					if (fl < 0){
					 	CS_PrintError("Error in CS_CloseFile\n");
						exit(EXIT_FAILURE);
					}
					openflag = 0;
				}
				else{
					perror("Error: You haven't set a pathname,create files and you want to close them? Really?\n");
				}
				break;
			}
			case 'F':{
				/*Parse the record*/
				if(openflag){
					b = ReadLine(stdin);    
					printf("\n");
					rec = RecordParse(b, &pflag);   // Parse the record from input.
					/*Insert record*/
					if (pflag != 1){
						if(pathname != NULL){
							if(CS_InsertEntry(&header, rec)<0) CS_PrintError("Error in CS_InsertEntry\n");
						}
					}
					else{
						perror("Error: You haven't set a pathname,create files,open files and you want to insert record!\n");
					}
					pflag = 0;
					free(b);
				}
				else{
					perror("Error: Files are not opened in order to insert an Entry!\n");
				}
				break;
			}
			case 'G':{
				/*Choose field and value for search*/
				if(openflag){
					printf("Choose field for search (id/name/surname/status/dateOfBirth/salary or enter to print every record) \n");
					char* field = ReadLine(stdin);  // Read field for comparisons.
					printf("Choose value for search (or enter to print every record)\n");
					char* val = ReadLine(stdin);    // Read value for comparisons.
					printf("\n");
					/*Only valid argument that starts with i is id so convert to number*/
					if (field == NULL || val == NULL) {
						perror("Error in reading field or value for search\n");
						break;
					}
					j=0;
					printf("\nChoose fields for printing - select ONE at a time and press NEWLINE after it to continue with the next one (or in case of just newline, end the selection).\n\n");
					printf("For example to include the field id in printing press id and then newline etc.\n\n\n");
					do{
						printf("\n\n\nChoose one field to print from the following: id/name/surname/status/dateOfBirth/salary or just enter to end the selection and proceed with the printing.\n\n");
						p = ReadLine(stdin);
						if ((*p)!='\0') {
							customfieldNames[j]=p;  // Store fields for printing.
							j++;
						}
					}while(j<6 && ((*p) != '\0'));  // At most 6.
					if (j<6) free(p);   // Less than 6 selected, p was not stored.
					num = j;    // Number of fields selected.
					for (;j<6;j++) {
						customfieldNames[j]=NULL; // Null for every other element.
					}
					if (val != NULL && (field[0] == 'i' || (field[0] == 's' && field[1] == 'a'))){/*Only valid argument that starts with i is id so convert to number*/
						int v = atoi(val);
						if (strlen(field)==0 && strlen(val)==0) {   // User chose to print every record.
							CS_GetAllEntries(&header, NULL, NULL, customfieldNames, num);
							if (CS_errno !=CSE_OK) CS_PrintError("Error in CS_GetAllEntries\n");
						} else if (strlen(field)>0 && strlen(val)>0){
							CS_GetAllEntries(&header, field, &v, customfieldNames, num);
							if (CS_errno != CSE_OK) CS_PrintError("Error in CS_GetAllEntries\n");
						} else {
							printf("To print every record press enter in both field and value!\n");
						}
					} else{     // String value
						if (strlen(field)==0 && strlen(val)==0) {   // User chose to print every record.
							CS_GetAllEntries(&header, NULL, NULL, customfieldNames, num);
							if (CS_errno !=CSE_OK) CS_PrintError("Error in CS_GetAllEntries\n");
						} else if (strlen(field)>0 && strlen(val)>0){	// User chose specific comparison field and value.
							CS_GetAllEntries(&header, field, val, customfieldNames, num);
							if (CS_errno != CSE_OK) CS_PrintError("Error in CS_GetAllEntries\n");
						} else {
							printf("To print every record press enter in both field and value!\n");
						}
					}
					for (j=0;j<6;j++)
						free (customfieldNames[j]);
					free(field);
					free(val);
					
				}
				else{
					perror("Error: You cannot search through files if they are closed!\n");
				}
				break;
			}
			case 'H':{
				if(openflag){
					printf("Enter name of file\n");
					fn = ReadLine(stdin);
					FILE* f = fopen(fn,"r");
					if (f == NULL) {
						perror("Failed to open file\n");
						return 1;
					}
					line = ReadLine(f);
					while (line != NULL) {
						rec = RecordParse(line, &pflag);
						if (pflag != 1){
							if (CS_InsertEntry(&header, rec)<0) CS_PrintError("Error in CS_InsertEntry\n");
						}
						pflag = 0;
						free (line);
						line = ReadLine(f);
					}
					fclose(f);
					free(fn);
					free(line);
				}
				else{
					perror("Search cannot operate on Closed Files !\n");
				}
				break;
			}
			default:
				printf("Exiting program\n");
				for (int i=0;i<6;i++)
					delete[] (fieldNames[i]);
				delete[] fieldNames;
				delete[] customfieldNames;
				free(pathname);
				return 0;
			}
		}
	for (int i=0;i<6;i++)
		delete[] (fieldNames[i]);
	delete[] fieldNames;
	delete[] customfieldNames;
	free(pathname);
	return 0;
}

#define BUF 80 

/*Reads a line from file and returns pointer to the string*/
char* ReadLine (FILE* f) {
	char* buf = (char*) malloc(BUF*sizeof(char));
	buf = fgets(buf, BUF,f);
	if (buf == NULL)
		return NULL;
	buf[strlen(buf)-1] = '\0';
	return buf;
}

/*Parse record from string, perform some checks, "" and , are removed*/
Record RecordParse(char* buf, int* flag) {
	Record rec;
	char* res;
	/*strtok returns NULL after it finds '0' so it checks if that is the case*/
	res = strtok(buf, " ,\"");
	if (res == NULL) {
		*flag = 1;
		printf("%s\n", buf);
		return rec;
	}
	rec.id = atoi(res);
	res = strtok(NULL, " ,\"");
	if (res == NULL) {
		*flag = 1;
		printf("%s\n", buf);
		return rec;
	}
	strncpy(rec.name, res,15);
	rec.name[14] = '\0';
	res = strtok(NULL, " ,\"");
	if (res == NULL) {
		*flag = 1;
		printf("%s\n", buf);
		return rec;
	}
	strncpy(rec.surname, res,20);
	rec.surname[19] = '\0';
	res = strtok(NULL, " ,\"");
	if (res == NULL) {
		*flag = 1;
		printf("%s\n", buf);
		return rec;
	}
	strncpy(rec.status, res,1);
	res = strtok(NULL, " ,\"");
	if (res == NULL) {
		*flag = 1;
		printf("%s\n", buf);
		return rec;
	}
	strncpy(rec.dateOfBirth, res,11);
	rec.dateOfBirth[10] = '\0';

	res = strtok(NULL, " ,\"");
	if (res == NULL) {
		*flag = 1;
		printf("%s\n", buf);
		return rec;
	}
	rec.salary = atoi(res);
	return rec;
}

void PrintMenu () {
	printf("\n\n\nChoose the next action:\n\n");
	printf("A.Set directory path\n");
	printf("B.Print directory path\n");
	printf("C.Create file\n");
	printf("D.Open file\n");
	printf("E.Close file\n");
	printf("F.Insert record in file\n");
	printf("G.Perform a search through records\n");
	printf("H.Perform a load from file\n");
	printf("Anything else to exit\n\n\n");
}
