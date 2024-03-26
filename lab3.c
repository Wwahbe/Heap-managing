#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct Block
{
	int block_size;
	struct Block *next_block;
	void *block_data;
};

const OVERHEAD_SIZE =
	sizeof(struct Block); // refers to size & pointer of a block
const POINTER_SIZE = sizeof(void *);
struct Block *free_head;

// YOUR CODE
void my_initialize_heap(int size)
{
	// Create a new block of memory with the requested size
	struct Block *new_block = (struct Block *)malloc(size);

	// Calculate the usable size of the block
	int usable_size = size - OVERHEAD_SIZE;

	// Initialize the new block
	new_block->block_size = usable_size;
	new_block->next_block = NULL;
	new_block->block_data = (void *)((char *)new_block + OVERHEAD_SIZE);

	// Set the new block as the head of the free list
	free_head = new_block;
}

void *my_alloc(int size)
{
	if (size <= 0)
	{
		printf("Size must be greater than 0");
		return 0;
	}

	// Adjust the size to be a multiple of POINTER_SIZE
	int adjusted_size = (int)ceil((double)size / POINTER_SIZE) * POINTER_SIZE;

	struct Block *curr = free_head;
	struct Block *prev = 0;
	bool found = false;

	while (curr != NULL)
	{
		if (curr->block_size >= adjusted_size)
		{
			found = true;

			// Determine if the current block can be split
			int remaining_size = curr->block_size - adjusted_size;
			if (remaining_size >= OVERHEAD_SIZE + POINTER_SIZE)
			{
				// Splittable
				struct Block *new_block =
					(struct Block *)((char *)curr + OVERHEAD_SIZE + adjusted_size);
				new_block->block_size = remaining_size;
				new_block->block_data = (char *)new_block + OVERHEAD_SIZE;

				// Adjust the double linked list
				if (curr == free_head)
				{
					free_head = new_block;
				}
				else
				{
					new_block->next_block = curr->next_block;
					prev->next_block = new_block;
				}

				curr->block_size = adjusted_size;
			}
			else
			{
				// Not splittable
				if (curr == free_head)
				{
					free_head = curr->next_block;
				}
				else
				{
					prev->next_block = curr->next_block;
				}
				curr->next_block = NULL;
			}

			// Block has been found!
			break;
		}
		else
		{
			// No available space :(
			prev = curr;
			curr = curr->next_block;
		}
	}

	// Return a pointer to the allocated data, if possible!!!
	if (found)
	{
		curr->block_data = (char *)curr + OVERHEAD_SIZE;
		return curr->block_data;
	}
	else
	{
		return NULL;
	}
}

void my_free(void *data)
{
	struct Block *freed_block = (struct Block *)((char *)data - OVERHEAD_SIZE);
	freed_block->next_block = free_head;
	free_head = freed_block;
}

void menuOptionOne()
{
	int *numOne = my_alloc(sizeof(int));
	printf("Address of int A: %p\n", numOne);
	my_free(numOne);

	int *numTwo = my_alloc(sizeof(int));
	printf("Address of int B: %p\n", numTwo);
};

// Allocate two ints and print their addresses; they should be exactly the size
// of your overhead plus the larger of (the size of an integer; the minimum
// block size) apart.
void menuOptionTwo()
{
	int *numOne = my_alloc(sizeof(int));
	printf("Address of int A: %p\n", numOne);
	int *numTwo = my_alloc(sizeof(int));
	printf("Address of int B: %p\n", numTwo);
	printf("Verifying Results...\n");
	int overheadPlusLarger = OVERHEAD_SIZE + sizeof(void *);
	printf("Size of overhead + larger of (the size of an integer; the minimum "
		   "block size): %d bytes\n",
		   overheadPlusLarger);
	printf("Address B - Address A: %d bytes \n", (int)numTwo - (int)numOne);
};

// Allocate three ints and print their addresses, then free the second of the
// three. Allocate an array of 2 double values and print its address (to
// allocate an array in C, allocate (2 * sizeof(double)); verify that the
// address is correct. Allocate another int and print its address; verify that
// the address is the same as the int that you freed.
void menuOptionThree()
{
	int *numOne = my_alloc(sizeof(int));
	printf("Address of int A: %p\n", numOne);
	int *numTwo = my_alloc(sizeof(int));
	printf("Address of int B: %p\n", numTwo);
	int *numThree = my_alloc(sizeof(int));
	printf("Address of int C: %p\n", numThree);
	my_free(numTwo);

	printf("After freeing int B...\n");
	double *arr = my_alloc(2 * sizeof(double));
	printf("Address of array of 2 double values: %p\n", arr);

	int *numFour = my_alloc(sizeof(int));
	printf("Address of int D (should be the int B): %p\n", numFour);
};

// Allocate one char, then allocate one int, and print their addresses. They
// should be exactly the same distance apart as in test #2.
void menuOptionFour()
{
	int check = 0;

	char *charOne = my_alloc(sizeof(char));
	printf("Address of char A: %p\n", charOne);
	int *numTwo = my_alloc(sizeof(int));
	printf("Address of int B: %p\n", numTwo);

	int overheadPlusLarger = OVERHEAD_SIZE + sizeof(void *);
	printf("Size of overhead + larger of (the size of an integer; the minimum "
		   "block size): %d\n",
		   overheadPlusLarger);
};

// Allocate space for a 80-element int array, then for one more int value.
// Verify that the address of the int value is 80 * sizeof(int) + the size of
// your header after the array's address. Free the array. Verify that the int's
// address and value has not changed.
void menuOptionFive()
{
	int *arr = my_alloc(80 * sizeof(int));
	int *numOne = my_alloc(sizeof(int));
	printf("Address of array: %p\n", arr);
	printf("Address of int A: %p\n", numOne);
	printf("Address of int value: %p\n", arr + 80 + sizeof(int));
	printf("Value of int A: %d\n", *numOne);

	printf("Difference between array and int A: %d\n", (int)numOne - (int)arr);

	my_free(arr);

	printf("After freeing array...\n");
	printf("Address of int value: %p\n", numOne);
	printf("Value of int A: %d\n", *numOne);
}

int main()
{
	int menuChoice = 0;
	int runAgain = 1;
	while (runAgain == 1)
	{
		printf("\n1. Allocate an int \n2. Allocate two ints \n3. Allocate three "
			   "ints \n4. Allocate one char \n5. Allocate space for an 80-element "
			   "int array \n6. Quit \nChoose a menu option: ");
		scanf("%d", &menuChoice);
		printf("\n---Test Case %d---\n", menuChoice);
		my_initialize_heap(1000);

		if (menuChoice == 1)
		{
			menuOptionOne();
		}

		else if (menuChoice == 2)
		{
			menuOptionTwo();
		}

		else if (menuChoice == 3)
		{
			menuOptionThree();
		}

		else if (menuChoice == 4)
		{
			menuOptionFour();
		}

		else if (menuChoice == 5)
		{
			menuOptionFive();
		}

		else if (menuChoice == 6)
		{
			printf("Done!");
			runAgain = 0;
		}
	}
	return 0;
}