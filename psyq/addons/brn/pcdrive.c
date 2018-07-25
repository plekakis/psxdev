void main()
{
char c[80];
struct DIRENTRY res;
strcpy(c, "pcdrv:*.*");
printf("\n** Loading Files...\n");
if (firstfile(c,&res)==0)
        {
        printf("Failed to read Directory Entry, does directory pcdata exist?\n");
        exit(1);
        }
processFile(rec.name)
while (nextfile(&res)!=0)
        {
        processFile(rec.name)
        }
}
void processfile(char *name)
{
int fd;
char fullName[80];
strcat(fullName, O_RDONLY);
if (fd==-1)
        {
        printf("Failed to open file %s\n",rec->filename);
        exit(1);
        }
numRead = read(fd, (char *)rec->ptr, rec->length);
if (numRead != rec->length)
        {
        close(fd);
        printf("Failed to read from file %s\n",rec->filename);
        exit(1);
        }
printf("OK.\n");
close(fd);
rec=rec->next;
}

