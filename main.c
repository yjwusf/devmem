#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>

#define DEBUG 1
// #define DEBUG 0

#include "stdio.h"
#include "unistd.h"
#include "inttypes.h"

struct A {
	unsigned long a;
	unsigned long b;
	unsigned long c;
	unsigned long d;
};

// void sifive_l2_flush64_range(unsigned long start, unsigned long len);

// function to translate vaddr to physical
// https://stackoverflow.com/questions/2440385/how-to-find-the-physical-address-of-a-variable-from-user-space-in-linux
uintptr_t vtop(uintptr_t vaddr) {
    FILE *pagemap;
    intptr_t paddr = 0;
    int offset = (vaddr / sysconf(_SC_PAGESIZE)) * sizeof(uint64_t);
    uint64_t e;

    // https://www.kernel.org/doc/Documentation/vm/pagemap.txt
    if ((pagemap = fopen("/proc/self/pagemap", "r"))) {
        if (lseek(fileno(pagemap), offset, SEEK_SET) == offset) {
            if (fread(&e, sizeof(uint64_t), 1, pagemap)) {
                if (e & (1ULL << 63)) { // page present ?
                    paddr = e & ((1ULL << 54) - 1); // pfn mask
                    paddr = paddr * sysconf(_SC_PAGESIZE);
                    // add offset within page
                    paddr = paddr | (vaddr & (sysconf(_SC_PAGESIZE) - 1));
                }   
            }   
        }   
        fclose(pagemap);
    }   

    return paddr;
}

void *get_phys_from_ptr(int fd, int len, uint64_t offset) {
	uint64_t page_size = sysconf(_SC_PAGE_SIZE);
	uint64_t page_base = (offset / page_size) * page_size;
	uint64_t page_offset = offset - page_base;

	if (DEBUG)
		printf("Pagesize: %lu, Pagebase: %lx, Page offset: %x, Offset: %lx\n", page_size, page_base, page_offset, offset);

	// Does the actual mmap
	// void *ptr = mmap(NULL, page_offset + len, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, page_base);
	void *ptr = mmap(NULL, page_offset + len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, page_base);
	
	if (ptr == MAP_FAILED) {
		perror("Can't mmap memory!\n");
		exit(1);
	}
	if (DEBUG)
		printf("Final vaddr %lx %lx\n", ptr, page_offset);

	ptr += page_offset;	
	if (DEBUG)
		printf("Final vaddr %lx\n", ptr);

	return ptr;
}

int main(int argc, char *argv[]) {
	struct A *x_p = malloc(sizeof(struct A));
	if (DEBUG)
		printf("Malloced vaddr %lx\n", x_p);

	// get phys offset from the noncoherent part
	off_t offset = 0x440000000UL + vtop((uintptr_t)x_p) - 0x40000000UL;
	if (DEBUG)
		printf("Malloced paddr %lx, final offset %lx, from start %lx\n", vtop((uintptr_t)x_p), offset, vtop((uintptr_t)x_p) - 0x40000000UL);
	
	int fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0) {
		perror("Can't open file\n");
		return -1;
	}

	if (DEBUG)
		printf("Passed size %u, fd: %u\n", sizeof(struct A), fd);

	struct A *p = get_phys_from_ptr(fd, sizeof(struct A), offset);
	p->c = 4;	
	printf("c value in uncohere struct at vaddr %lx paddr %lx: %lx %lx %lx %lx\n", (uintptr_t)p, offset, p->a, p->b, p->c, p->d);
	p->c += 8;
	printf("c value in uncohere struct at vaddr %lx paddr %lx: %lx %lx %lx %lx\n", (uintptr_t)p, offset, p->a, p->b, p->c, p->d);
	// printf("c value in coherent struct at vaddr %lx paddr %lx: %lx %lx %lx %lx\n", (uintptr_t)x_p, vtop((uintptr_t)x_p), x_p->a, x_p->b, x_p->c, x_p->d);
	return 0;
}
