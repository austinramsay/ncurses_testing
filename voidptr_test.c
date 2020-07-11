#include <curses.h>
#include <stdlib.h>
#include <sys/random.h>

int main(int argc, char **argv) {
	void *buffer = malloc(32);

	ssize_t byte_count = getrandom(buffer, 32, 0);

	printf("The byte count is %ld\n", byte_count);
	(buffer + 32) = 0x22;

	for (int i = 0; i < 39; i++) {
		printf("%02x", ((unsigned char *)buffer)[i]);
	}
	printf("\n");

	return 0;
}
