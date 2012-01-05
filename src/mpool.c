#include "mpool.h"

#include <stdlib.h>
#include <string.h>

#define MALLIGN(size) (((size) + sizeof(size_t) - 1) & ~(sizeof(size_t) - 1))



struct mpage_s {
  struct mpage_s *prev;
  char *data;
  char *ptr;
  size_t free;
  size_t used;
};



struct mpage_s* mpage_open(struct mpage_s *prev, size_t page_asize) {
  struct mpage_s *ret = malloc(sizeof(struct mpage_s));
  ret->prev = prev;
  ret->data = malloc(page_asize);
  ret->ptr = ret->data;
  ret->free = page_asize;
  ret->used = 0;
  return ret;
}



void mpage_close(struct mpage_s *mpage) {
  free(mpage->data);
  free(mpage);
}



void mpage_close_all(struct mpage_s *mpage) {
  if (mpage->prev != NULL) mpage_close_all(mpage->prev);
  mpage_close(mpage);
}



void* mpage_alloc(struct mpage_s *mpage, size_t asize) {
  void *ret;
  if (mpage->free < asize) return NULL;
  ret = mpage->ptr;
  mpage->used += asize;
  mpage->free -= asize;
  mpage->ptr += asize;
  return ret;
}



#define MPOOL(mpool) ((struct mpool_s*)(mpool))

struct mpool_s {
  size_t page_size;
  struct mpage_s *page;
};



mpool_t mpool_open(size_t page_size) {
  struct mpool_s *ret;

  page_size = MALLIGN(page_size);
  ret = (struct mpool_s*)malloc(sizeof(struct mpool_s));
  ret->page_size = page_size;
  ret->page = mpage_open(NULL, ret->page_size);
  return ret;
}



void mpool_close(mpool_t mpool) {
  mpage_close_all(MPOOL(mpool)->page);
  free(mpool);
}



void* mpool_alloc(mpool_t mpool, size_t size) {
  size_t page_size;
  void *ret;

  size = MALLIGN(size);
  ret = mpage_alloc(MPOOL(mpool)->page, size);
  if (ret == NULL) {
    page_size = size < MPOOL(mpool)->page_size ? MPOOL(mpool)->page_size : size;

    MPOOL(mpool)->page = mpage_open(MPOOL(mpool)->page, size);
    ret = mpage_alloc(MPOOL(mpool)->page, size);
  }
  return ret;
}



void* mpool_realloc(
  mpool_t mpool,
  void *ptr __attribute__((unused)),
  size_t size)
{
  return mpool_alloc(mpool, size);
}



void mpool_free(
  mpool_t mpool __attribute__((unused)),
  void *ptr __attribute__((unused)))
{}



void mpool_stats(mpool_t mpool, size_t *pages) {
  size_t _pages = 0;
  struct mpage_s *cur = MPOOL(mpool)->page;
  
  for (; cur != NULL; cur = cur->prev)
    ++_pages;

  *pages = _pages;
}
