#include "mpool.h"

#include <stdlib.h>
#include <string.h>

#define MALLIGN(size) (((size) + sizeof(size_t) - 1) & ~(sizeof(size_t) - 1))



#define MOBJ_DATASIZE sizeof(size_t) * 2

#define MOBJ_UNWRAP_SIZE(obj) (*(((size_t*)(obj)) - 1))

#define MOBJ_UNWRAP_MPAGE(obj) (*(struct mpage_s**)(((size_t*)(obj)) - 2))



struct mpage_s {
  struct mpage_s *prev;
  void *data;
  void *ptr;
  size_t free;
  size_t used;
};



struct mpage_s* mpage_open(struct mpage_s *prev, size_t size) {
  struct mpage_s *ret = malloc(sizeof(struct mpage_s));

  size = MALLIGN(size + MOBJ_DATASIZE);
  ret->prev = prev;
  ret->data = (char*)malloc(size);
  ret->ptr = ret->data;
  ret->free = size;
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



void* mpage_alloc(struct mpage_s *mpage, size_t size) {
  void *ret;

  size = MALLIGN(size + MOBJ_DATASIZE);

  if (mpage->free < size) return NULL;

  ret = (char*)mpage->ptr + MOBJ_DATASIZE;
  mpage->used += size;
  mpage->free -= size;
  mpage->ptr = (char*)mpage->ptr + size;

  MOBJ_UNWRAP_SIZE(ret) = size - MOBJ_DATASIZE;
  MOBJ_UNWRAP_MPAGE(ret) = mpage;

  return ret;
}



#define MPOOL(mpool) ((struct mpool_s*)(mpool))

struct mpool_s {
  size_t page_size;
  struct mpage_s *page;
};



mpool_t mpool_open(size_t page_size) {
  struct mpool_s *ret;

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
  void *ret;

  if (size == 0) return NULL;

  ret = mpage_alloc(MPOOL(mpool)->page, size);
  if (ret == NULL) {
    size_t page_size=
      size < MPOOL(mpool)->page_size ? MPOOL(mpool)->page_size : size;

    MPOOL(mpool)->page = mpage_open(MPOOL(mpool)->page, page_size);
    ret = mpage_alloc(MPOOL(mpool)->page, size);
  }

  return ret;
}



void* mpool_realloc(mpool_t mpool, void *ptr, size_t size) {
  if (ptr == NULL) return mpool_alloc(mpool, size);

  if (size == 0) {
    mpool_free(mpool, ptr);
    return NULL;
  }

  if (MOBJ_UNWRAP_SIZE(ptr) < size) {
    void *ret = mpool_alloc(mpool, size);

    memcpy(ret, ptr, MOBJ_UNWRAP_SIZE(ptr));
    mpool_free(mpool, ptr);
    return ret;
  }
  
  return ptr;
}



void mpool_free(mpool_t mpool, void *ptr) {
  struct mpage_s *page;

  if (ptr == NULL) return;
  
  page = MOBJ_UNWRAP_MPAGE(ptr);
  page->used -= MOBJ_UNWRAP_SIZE(ptr) + MOBJ_DATASIZE;

  if (page->used == 0 && !(page->prev == NULL && MPOOL(mpool)->page == page)) {
    struct mpage_s *cur;
    struct mpage_s *next;

    cur = MPOOL(mpool)->page;
    next = NULL;
    for (; cur != NULL; next = cur, cur = cur->prev) {
      if (cur != page) continue;
     
      if (next != NULL) {
        next->prev = cur->prev;
      } else {
        MPOOL(mpool)->page = cur->prev;
      }
      mpage_close(cur);
      break;
    }
  }
}



void mpool_stats(mpool_t mpool, size_t *pages) {
  size_t _pages;
  struct mpage_s *cur;
  
  for (cur = MPOOL(mpool)->page, _pages = 0; cur != NULL; cur = cur->prev)
    ++_pages;

  *pages = _pages;
}
