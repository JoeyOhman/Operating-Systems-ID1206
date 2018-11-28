
#include <sys/mman.h>
#include <assert.h>
#include <stdio.h>

#define MIN 5 // MIN is 2^5 = 32
#define LEVELS 8 // Makes max level 12 => 2^12 = 4096 = 4KB
#define PAGE 4096

void printFlist();

int pageCounter = 0;
int memGiven = 0;
int memUsed = 0; // Actually used, measures internal fragmentation.

enum flag {Free, Taken};

struct head {
  enum flag status;
  short int level;
  struct head *next;
  struct head *prev;
};
void removeBlockFromFlists(struct head* block);

struct head* flists[LEVELS] = {NULL};

void printEval(int bufferSize) {
  // printf("%d\t%d\t%f\n", pageCounter*PAGE, memGiven, memGiven / (double)(pageCounter*PAGE));
  printf("%d\t%f\t%f\t%f\n", bufferSize, (double)pageCounter*PAGE/1000, (double)memGiven/1000, (double)memUsed/1000);
  //printFlist();
}

void returnPages() {
  pageCounter = 0;
  memGiven = 0;
  memUsed = 0;
  struct head* page = flists[LEVELS-1];
  while(page != NULL) {
    removeBlockFromFlists(page);
    munmap(page, PAGE);
    page = flists[LEVELS-1];
  }
}

struct head* new() {
  pageCounter++;
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

// New merge, NOT INCREMENTING LEVEL THOUGH, nor changing free
struct head* primary(struct head* block) {
  int index = block->level;
  long int mask = 0xffffffffffffffff << (1 + index + MIN); // Set all up to and including the buddy bit to 0
  return (struct head*)((long int)block & mask);
}

// Merges with buddy and returns the head (primary of buddies with flags changed)
struct head* merge(struct head* block) {
  struct head* prim = primary(block);
  prim->level++;
  prim->status = Free;
  return prim;
}

// jump forward one header size so the header becomes hidden for the user
void *hide(struct head* block) {
  return (void*)(block + 1);
}

// jump back to header which was hidden for user
struct head* magic(void* memory) {
  return((struct head*)memory - 1);
}

int level(int req) {
  int total = req + sizeof(struct head); // size required is users size + header

  int i = 0;
  int size = 1 << MIN;
  while(total > size) { // double size until we have enough
    size <<= 1;
    i++;
  }
  return i;
}

// Should we not set flags?
// Splits block and returns secondary, primary is the argument now
struct head* split(struct head *block) {
  int index = block->level - 1;
  int mask = 0x1 << (index + MIN); // find relevant bit for buddy
  struct head* secondary = (struct head*)((long int)block | mask);
  block->level = index;
  secondary->level = index;
  block->status = Free;
  secondary->status = Free;
  return secondary;
}

void addBlockToFlists(struct head* block) {
  struct head* first = flists[block->level];
  /*
  if(block->level == 5) {
    printf("Block lvl5 added to flist: %p\n", block);
  }
  assert(block != first);
  */
  if(first != NULL) {
    first->prev = block;
    block->next = first;
  }
  flists[block->level] = block;
}

void removeBlockFromFlists(struct head* block) {
  if(flists[block->level] == block) { // First
    flists[block->level] = block->next;
    /*
    printf("Removed first!\n");
    if(flists[block->level] != NULL) {
      printf("Removed block{%p}, new first{%p}\n", block, block->next);
      printf("New first prev={%p}, next={%p}\n", flists[block->level]->prev, flists[block->level]->next);
    }
    */
  } else { // Not first
    //printFlist();
    // printf("\nprev: %p\n", block->prev);
    // printf("block status=%d, block level=%d\n", block->status, block->level);
    block->prev->next = block->next; // There is a block prev
  }

  if(block->next != NULL) { // Not last
    // Unlink block
    block->next->prev = block->prev;
  }

  block->next = NULL;
  block->prev = NULL;
}

struct head* splitUntilMatchingLevel(struct head* block, int level) {

  while(block->level > level) {
    struct head* buddy = split(block);
    addBlockToFlists(buddy);
  }
  block->status = Taken;
  return block;
}

struct head* find(int index) {
  int foundIndex = index;
  while(flists[foundIndex] == NULL) {
    if(foundIndex == LEVELS-1) {
      // Highest level without finding free block, allocate new!
      return splitUntilMatchingLevel(new(), index);
    }
    foundIndex++;
  }
  struct head* foundHead = flists[foundIndex];
  removeBlockFromFlists(foundHead);
  return splitUntilMatchingLevel(foundHead, index);
}

// Find buddy, merge if free, repeat, then add to free list
void insert(struct head* block) {
  block->status = Free;
  struct head* buddyBlock = buddy(block);
  //printf("Block being freed: {%p}, its buddy: {%p}\n", block, buddyBlock);
  //printf("Block level: %d\n", block->level);
  //printf("Buddy level: %d\n", buddyBlock->level);
  //printf("Block/buddy status: %d/%d\n", block->status, buddyBlock->status);

  // Make sure to check block level first, as checking buddyprops on highest level results in segfault
  // Merge while buddy is free and not on max level
  while(block->level < LEVELS-1 && buddyBlock->status == Free
    && buddyBlock->level == block->level) {

    //printf("Merging, from level=%d\n", block->level);
    removeBlockFromFlists(buddyBlock);
    block = merge(block);
    buddyBlock = buddy(block);
  }
  // Merged as much as possible, add resulting block to flist
  addBlockToFlists(block);
}

void* balloc(size_t size) {
  if(size == 0) {
    return NULL;
  }
  memUsed += size;
  int index = level(size);
  memGiven += 1 << (index + MIN);
  struct head* taken = find(index);
  return hide(taken);
}

void bfree(void* memory) {
  if(memory != NULL) {
    struct head* block = magic(memory);
    memGiven -= 1 << (block->level + MIN);
    insert(block);
  }
  return;
}
void bfreeTrackIntFrag(void* memory, size_t size) {
  memUsed -= size;
  bfree(memory);
}

void printFlist() {
  printf("Flists:\n");
  for(int i = LEVELS-1; i >= 0; i--) {
    int counter = 0;
    struct head* block = flists[i];
    while(block != NULL) {
      counter++;
      if(block == block->next) {
        printf("BLOCK POINTING TO ITSELF IN FLIST!\n");
        break;
      }
      block = block->next;
    }
    printf("Level %d: #freeBlocks=%d\n", i, counter);
  }
}

void testBallocBfree() {
  assert(balloc(0) == NULL);
  printf("\n");
  printFlist();

  printf("Allocating level 0 block...\n");
  void* level0Block = balloc(1);
  assert(magic(level0Block)->level == 0);
  printFlist();

  printf("Allocating level 1 block...\n");
  void* level1Block = balloc(33 - sizeof(struct head));
  assert(magic(level1Block)->level == 1);
  printFlist();

  printf("Allocating level 5 block...\n");
  void* level5Block = balloc(1024 - sizeof(struct head));
  assert(magic(level5Block)->level == 5);
  printFlist();
  printf("Allocating level 5 block...\n");
  void* level5Block2 = balloc(1024 - sizeof(struct head));
  assert(magic(level5Block2)->level == 5);
  printFlist();

  printf("Allocating level 7 block...\n");
  void* level7Block = balloc(4096 - sizeof(struct head));
  assert(magic(level7Block)->level == 7);
  printFlist();

  printf("Allocating level 0 block...\n");
  void* level0Block2 = balloc(1);
  assert(magic(level0Block2)->level == 0);
  printFlist();

  printf("Freeing level 0 block...\n");
  bfree(level0Block);
  printFlist();

  printf("Freeing level 0 block...\n");
  bfree(level0Block2);
  printFlist();

  printf("Freeing level 5 block..., buddy should be taken even though a 5 is free...\n");
  bfree(level5Block);
  printFlist();

  printf("Freeing another level 5 block, should see a merge..\n");
  bfree(level5Block2);
  printFlist();

  printf("Freeing all blocks...\n");
  bfree(level7Block);
  printf("Freed 7block!\n");
  bfree(level1Block);
  printf("Freed 1block! (last)\n");
  printFlist();

  printf("Allocating 10 blocks..\n");
  void* mem0 = balloc(23);
  void* mem1 = balloc(23);
  void* mem2 = balloc(4000);
  void* mem3 = balloc(2000);
  void* mem4 = balloc(1234);
  void* mem5 = balloc(88);
  void* mem6 = balloc(1233);
  void* mem7 = balloc(2343);
  void* mem8 = balloc(4000);
  void* mem9 = balloc(1);
  printFlist();

  printf("Freeing the 10 blocks...\n");
  bfree(mem0);
  bfree(mem1);
  bfree(mem2);
  bfree(mem3);
  bfree(mem4);
  bfree(mem5);
  bfree(mem6);
  bfree(mem7);
  bfree(mem8);
  bfree(mem9);
  printFlist();

}

void test() {
  printf("Running test!!!!!11!!1\n");
  struct head *newBlock  = new();
  printf("Allocated new block: %p, status should be 0(free): %d, level should be 7 (max): %d\n", newBlock, newBlock->status, newBlock->level);
  struct head *buddyBlock = split(newBlock);
  printf("Split new block, primary vs secondary: \n%p\n%p\n", newBlock, buddyBlock);
  assert(buddy(buddyBlock) == newBlock); // secondarys buddy is primary
  struct head* buddyOfBuddyBlock = split(buddyBlock);
  assert(buddy(buddyBlock) == buddyOfBuddyBlock); //  primarys buddy is secondary
  printf("Header after magic and memWrite: level=%d, status=%d\n", buddyOfBuddyBlock->level, buddyOfBuddyBlock->status);
  int* mem = (int*)hide(buddyOfBuddyBlock);
  *mem = 3;
  struct head* imBack = magic((void*)mem);
  assert(imBack == buddyOfBuddyBlock);
  printf("Header after magic and memWrite: level=%d, status=%d\n", imBack->level, imBack->status);
  struct head* primaryOfBuddyOfBuddyBlock = merge(buddyOfBuddyBlock);
  assert(primaryOfBuddyOfBuddyBlock == buddyBlock);
  assert(buddy(buddyBlock) == newBlock);
  assert(primary(buddyBlock) == newBlock);
  assert(primary(newBlock) == newBlock);
  assert(merge(buddyBlock) == newBlock);
  printf("Header of newBlock: level=%d, status=%d\n", newBlock->level, newBlock->status);

  assert(level(32 - sizeof(struct head)) == 0);
  assert(level(33 - sizeof(struct head)) == 1);
  assert(level(64 - sizeof(struct head)) == 1);
  assert(level(65 - sizeof(struct head)) == 2);
  assert(level(PAGE - sizeof(struct head)) == LEVELS-1);

  // Test balloc and bfree
  testBallocBfree();

  printf("\nAll assertions passed! If prints are correct, program should be working!\n");
}
