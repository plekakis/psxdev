#include <core/core.h>

///////////////////////////////////////////////////
void start()
{
	const StringId filename = ID("ROOT\\ROOT0.TXT");
	const uint32 size = Stream_GetFileSize(filename);
	char* contents = (char*)Core_PushStack(CORE_STACKALLOC, size, 4);

	Stream_ReadFileBlocking(filename, (void*)contents);

	REPORT(contents);

	Core_PopStack(CORE_STACKALLOC);
}

///////////////////////////////////////////////////
void update()
{

}

///////////////////////////////////////////////////
void render()
{

}