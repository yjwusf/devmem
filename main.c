#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

#define DEBUG true

void get_phys_from_ptr(FILE *f, int len) {
	int len = 4;
	int *x_p = malloc(len * sizeof(int)); // get phys address

	off_t offset = 0x400000000UL + (off_t)x_p;
	size_t page_size = sysconf(_SC_PAGE_SIZE);
	off_t page_base = (offset / page_size) * page_size;
	off_t page_offset = offset - page_base;

	if (DEBUG)
		printf("Pagesize: %lu, Pagebase: %lu, Offset: %lu", page_size, page_base, offset);

	// Does the actual mmap
	unsigned char *ptr = mmap(NULL, page_offset + len, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, page_base);
	
	if (ptr == MAP_FAILED) {
		perror("Can't map memory!\n");
		return -1;
	}
}

int main(int argc, char *argv[]) {
	FILE *fd = open("/dev/mem", O_SYNC);
	if (ptr == MAP_FAILED) {
		perror("Can't map memory.\n");
		return -1;
	}

	return 0;
}
