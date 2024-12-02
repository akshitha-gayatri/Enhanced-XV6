// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run
{
  struct run *next;
};

struct
{
  struct spinlock lock;
  struct run *freelist;
} kmem;

int refcnt[PHYSTOP / PGSIZE];
void kinit()
{
  initlock(&kmem.lock, "kmem");
  memset(refcnt, 0, sizeof(refcnt));

  freerange(end, (void *)PHYSTOP);
}

void freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char *)PGROUNDUP((uint64)pa_start);
  for (; p + PGSIZE <= (char *)pa_end; p += PGSIZE)
  {
    refcnt[(uint64)p / PGSIZE] = 1;
    kfree(p);
  }
}

void incref(uint64 pa)
{
  // acquire the lock
  acquire(&kmem.lock);
  int pn = pa / PGSIZE;
  if (pa > PHYSTOP || refcnt[pn] < 1)
  {
    panic("increase ref cnt");
  }
  refcnt[pn]++;
  release(&kmem.lock);
}

// Decrement the reference count for a page.
// If reference count drops to zero, free the page.
void decref(uint64 pa) {
  int should_free = 0;
  
  acquire(&kmem.lock);
  if (--refcnt[pa / PGSIZE] == 0) {
    should_free = 1;  // Mark for freeing after releasing the lock
  }
  release(&kmem.lock);

  // Free the page if no references remain
  if (should_free) {
    kfree((void *)pa);
  }
}

void kfree(void *pa)
{
  struct run *r;
  r = (struct run *)pa;

  if (((uint64)pa % PGSIZE) != 0 || (char *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  acquire(&kmem.lock);
  int pn = (uint64)r / PGSIZE;

  // Check for valid reference count before decrementing
  if (refcnt[pn] < 1)
    panic("kfree panic: refcount below zero");

  // Decrement reference count
  refcnt[pn] -= 1;

  // Only free if reference count has reached zero
  if (refcnt[pn] == 0)
  {
    memset(pa, 1, PGSIZE); // Fill with junk to catch dangling refs
    r->next = kmem.freelist;
    kmem.freelist = r;
  }

  release(&kmem.lock);
}

void *kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;

  if (r)
  {
    int pn = (uint64)r / PGSIZE;

    // Ensure reference count is zero before allocation
    if (refcnt[pn] != 0)
      panic("kalloc: refcount not zero");

    // Set reference count to 1 for newly allocated page
    refcnt[pn] = 1;
    kmem.freelist = r->next;
  }

  release(&kmem.lock);

  if (r)
    memset((char *)r, 5, PGSIZE); // Fill with junk
  return (void *)r;
}
