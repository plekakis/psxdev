
#define CD_LOAD_RETRY (4u)

///////////////////////////////////////////////////
void start()
{
	{
		int32	i, cnt, found=0;
		CdlFILE	file;
		char	filename[64];

		sprintf(filename, "\\%s;1", "ROOT\\TEST\\LEVEL1\\L1.TXT");

		for (i = 0; i < CD_LOAD_RETRY; i++)
		{
			if (CdSearchFile(&file, filename) != 0)
			{
				found = 1;
				break;
			}
		}

		VERIFY_ASSERT(found, "Filename %s not found on CD!", filename);
		REPORT("Size: %i bytes", file.size);
	}
}

///////////////////////////////////////////////////
void update()
{

}

///////////////////////////////////////////////////
void render()
{

}