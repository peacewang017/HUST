#include "user_lib.h"
#include "util/string.h"
#include "util/types.h"

int main(int argc, char *argv[])
{
	printu("Hart 1 running cycle, will continue running\n");
	while (1) {
		// ...
	}
	// 永远不会到达的地方
	exit(0);
	return 0;
}