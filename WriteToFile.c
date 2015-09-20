#include "WriteToFile.h"

int WriteToFile(char *path, char *filename,  double result)
{
	int lenght;
	char savefile[100] ="";;
	//char *finalfilename;
	FILE *fp;
	lenght = strlen(path);

	if(lenght < 100) {
		strcat(savefile, path);
		strcat(savefile, filename);
	}
	else {
		perror("File path name too long");
		exit(EXIT_FAILURE);
	}

	//finalfilename = strdup(savefile);

	fp = fopen(savefile, "w+");
	if(fp == NULL)
	{
		perror("Write to file failed");
		exit(EXIT_FAILURE);
	}

	fprintf(fp, "%.1f", result);
	fclose(fp);

	//free(finalfilename);

	return(EXIT_SUCCESS);
}
