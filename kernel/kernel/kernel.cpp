#include <stdio.h>

#include <kernel/Terminal.h>
 
int i = 100;

namespace kernel
{
	extern "C" void kernel_main(void) 
	{
		terminal::Init();

		printf("                          ___  ___        _____ _____ \n");
		printf("                          |  \\/  |       |  _  /  ___|\n");
		printf("                          | .  . |_   _  | | | \\ `--. \n");
		printf("                          | |\\/| | | | | | | | |`--. \\\n");
		printf("                          | |  | | |_| | \\ \\_/ /\\__/ /\n");
		printf("                          \\_|  |_/\\__, |  \\___/\\____/ \n");
		printf("                                   __/ |              \n");
		printf("                                  |___/               \n");

		printf("global variable 'i' as int:     [%i]\n", i);
		printf("global variable 'i' as hex:     [0x%x]\n", i);
		const char* str = "this is a const char*";
		printf("local variable 'str':           [%s]\n", str);
	}
}
