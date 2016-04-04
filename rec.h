#ifndef __REC__
#define __REC__


typedef struct Record {
	int id;
	char name[15];
	char surname[20];
	char status[1];
	char dateOfBirth[11];
	int salary;
} Record;

#endif