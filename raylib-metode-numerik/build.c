#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"
#include <unistd.h>
#include <stdlib.h>

Cmd cmd = {0};

void compileprog(void) {
	const char *ofname = "output";
	const char *ray_home = "raylib-6.0_linux_amd64";
	cmd_append(
		&cmd,
		"cc",
		"-std=c17",
		"-Wall",
		/* "-Wconversion", */
		/* "-Wshadow", */
		/* "-Wpedantic", */
		"main.c",
		temp_sprintf("-o%s", ofname),
		temp_sprintf("-I%s/include/", ray_home),
		temp_sprintf("-L%s/lib/", ray_home),
		temp_sprintf("-Wl,-rpath,%s/lib/", ray_home),
		"-lraylib"
	);
}

int main(int argc, char **argv) {
	NOB_GO_REBUILD_URSELF(argc, argv);
	compileprog();
	if (!cmd_run(&cmd)) return 1;
	return 0;
}

// vim:ft=c:ts=4:sw=4:nosi:noai:noet:cin
