#include <stdio.h>
#include <sqlite3.h>

void perror(const char *msg) {
	fprintf(stderr, "[fatal]: %s\n", msg);
}
int opendb_rw(sqlite3 *db, const char *db_file) {
	return sqlite3_open_v2(db_file, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
}
int closedb(sqlite3 *db) {
	return sqlite3_close_v2(db);
}

int main(void) {
	if (sqlite3_initialize() != SQLITE_OK) {
		perror("Could not initialize sqlite3");
		return 1;
	}

	const char *db_file = "my.db";
	sqlite3 *db = NULL;
	/* char *errmsg = 0; */

	if (opendb_rw(db, db_file) != SQLITE_OK) {
		perror("Could not open or create database");
		return 1;
	}

	if (closedb(db) != SQLITE_OK) {
		perror("Could not close database properly");
	}

	sqlite3_shutdown();
	return 0;
}

// vim:ft=c:ts=4:sw=4:nosi:noai:noet:cin
