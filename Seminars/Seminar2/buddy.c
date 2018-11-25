
#include <sys/mman.h>
#include <assert.h>
#include <stdio.h>

#define MIN 5 // MIN is 2^5 = 32
#define LEVELS 8 // Makes max level 12 => 2^12 = 4096 = 4KB
#define PAGE 4096

enum flag {Free, Taken};

struct head {
  enum flag status;
  short int level;
  struct head *next;
  struct head *prev;
};

struct head* new() {
  struct head *new = (struct head*) mmap(NULL, PAGE,
                                        PROT_READ | PROT_WRITE,
                                        MAP_PRIVATE | MAP_ANONYMOUS,
                                        -1, 0);
  if(new == MAP_FAILED) {
    return NULL;
  }
  assert(((long int)new & 0xfff) == 0); // last 12 bits should be 0
  new->status = Free;
  new->level = LEVELS-1;
  return new;
}

struct head *buddy(struct head* block) {
  int index = block->level;
  long int mask = 0x1 << (index + MIN); // find relevant bit for buddy
  return (struct head*)((long int)block ^ mask); // toggle bit to get address of budy
}

/*
struct head *merge(struct head* block, struct head* sibling) {
  struct head *primary;
  if(sibling < block) // primary block is the one of lowest address
    primary = sibling;
  else
    primary = block;
  primary->level = primary->level+1; // coalesce, secondary header will be included in user's bytes
  return primary;
}
*/

// New merge, NOT INCREMENTING LEVEL THOUGH
struct head* primary(struct head* block) {
  int index = block->level;
  long int mask = 0xffffffffffffffff << (1 + index + MIN); // Set all up to and including the buddy bit to 0
  return (struct head*)((long int)block & mask);
}

// jump forward one header size so the header becomes hidden for the user
void *hide(struct head* block) {
  return (void*)(block + 1);
}

// jump back to header which was hidden for user
struct head* magic(void* memory) {
  return((struct head*)memory - 1);
}

int level(int size) {
  int req = size + sizeof(struct head); // size required is users size + header

  int i = 0;
  req = req >> MIN; // If required size < 32 => index = 0

  // Should this not be > 1??? think of the case where req = 32 = 2^5 )
  // the 6th bit is 1 and LEVEL will be 1
  while(req > 0) { // Double memory size until it is greater than required
    i++;
    req = req >> 1;
  }
  return i;
}

// Should we not set flags?
struct head *split(struct head *block) {
  int index = block->level - 1;
  int mask = 0x1 << (index + MIN); // find relevant bit for buddy
  return (struct head*)((long int)block | mask); // return secondary
}

void test() {
  printf("Running test!!!!!11!!1\n");
  struct head *newBlock  = new();
  printf("Allocated new block: %p, status should be 0(free): %d, level should be 7 (max): %d\n", newBlock, newBlock->status, newBlock->level);
  struct head *buddy = split(newBlock);
  printf("Split new block, primary vs secondary: \n%p\n%p\n", newBlock, buddy);
}
