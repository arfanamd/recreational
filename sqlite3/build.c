#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"
#include <unistd.h>
#include <stdlib.h>

Cmd cmd = {0};

void cc(const char *build_dir) {
	cmd_append(
		&cmd,
		"cc",
		"-std=c17",
		"-Wall",
		"-Wconversion",
		"-Wshadow",
		"-Wpedantic",
		temp_sprintf("-o%s/build/main", build_dir),
		"main.c"
	);
}

void sqlite(void) {
	const char *home_path = getenv("HOME");
	cmd_append(
		&cmd,
		temp_sprintf("-I%s/.include/", home_path),
		temp_sprintf("-L%s/.lib/", home_path),
		temp_sprintf("-Wl,-rpath=%s/.lib/", home_path),
		"-ldl",
		"-lpthread",
		"-lm",
		"-lsqlite3"
	);
};

int main(int argc, char **argv) {
	NOB_GO_REBUILD_URSELF(argc, argv);
	char *build_dir = (char *)malloc(256 * sizeof(char));
	getcwd(build_dir, 256);
	if (nob_mkdir_if_not_exists(temp_sprintf("%s/build/", build_dir))) {
		cc(build_dir);
		sqlite();
		cmd_run(&cmd);
	}
	free(build_dir);
	return 0;
}

// vim:ft=c:ts=4:sw=4:nosi:noai:noet:cin
