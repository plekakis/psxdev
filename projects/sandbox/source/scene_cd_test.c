#include <base/core/core.h>

///////////////////////////////////////////////////
void start()
{
	const StringId filename = ID("ROOT\\ROOT0.TXT");
	void* contents;

	Stream_BeginRead(filename, &contents);
	Stream_ReadFileBlocking();

	REPORT(contents);

	Stream_EndRead();
}

///////////////////////////////////////////////////
void update()
{

}

///////////////////////////////////////////////////
void render()
{

}