#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// static implementations
void
cat(const char* path)
{
    FILE *file;
    char chr;
    if((file = fopen(path, "r")) != 0)
    {
        while((chr = getc(file)) != EOF)
	{
            fprintf(stdout, "%c", chr);
	}
        fclose(file);
    }
}

void
ls(const char* path)
{
    struct dirent **namelist;
    int n;
    printf("dir: %s\n", path);
    n=scandir(path,&namelist,NULL,alphasort);
    while(n--)
    {
	    printf("%s\n", namelist[n]->d_name);
	    free(namelist[n]);
    }
    free(namelist);
    printf("---\n");
}
