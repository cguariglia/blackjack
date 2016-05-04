#include <stdio.h>
#include <stdlib.h>

// Wrapper function around malloc, to check for memory allocation failure
void * allocate(size_t size) {
	void *ptr;
	
	ptr = malloc(size);
	
	if(ptr == NULL) {
		printf("Error allocating memory!");
		exit(EXIT_FAILURE);
	}
	return ptr;
}
