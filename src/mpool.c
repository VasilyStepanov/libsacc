#include "mpool.h"

#include <sacc.h>

#include <stdlib.h>
#include <string.h>

#define MALLIGN(size) (((size) + sizeof(size_t) - 1) & ~(sizeof(size_t) - 1))



#define MOBJ_WRAP(obj) ((void*)((char*)(obj) + sizeof(struct _SAC_MObj)))
#define MOBJ_UNWRAP(obj) \
  ((struct _SAC_MObj*)((char*)(obj) - sizeof(struct _SAC_MObj)))

#define MOBJ_UNWRAP_SIZE(obj) MOBJ_UNWRAP(obj)->size

#define MOBJ_UNWRAP_MPAGE(obj) MOBJ_UNWRAP(obj)->mpage




struct _SAC_MPage {
  struct _SAC_MPage *prev;
  void *data;
  void *ptr;
  size_t free;
  size_t used;
};



struct _SAC_MObj {
  struct _SAC_MPage *mpage;
  size_t size;
};



static struct _SAC_MPage* mpage_open(struct _SAC_MPage *prev, size_t size) {
  struct _SAC_MPage *page = malloc(sizeof(struct _SAC_MPage));

  if (page == NULL) return NULL;

  size = MALLIGN(size + sizeof(struct _SAC_MObj));
  page->prev = prev;
  page->data = (char*)malloc(size);
  page->ptr = page->data;
  page->free = size;
  page->used = 0;
  return page;
}



static void mpage_close(struct _SAC_MPage *mpage) {
  free(mpage->data);
  free(mpage);
}



static void mpage_close_all(struct _SAC_MPage *mpage) {
  if (mpage->prev != NULL) mpage_close_all(mpage->prev);
  mpage_close(mpage);
}



static void* mpage_alloc(struct _SAC_MPage *mpage, size_t size) {
  void *ptr;

  size = MALLIGN(size + sizeof(struct _SAC_MObj));

  if (mpage->free < size) return NULL;

  ptr = MOBJ_WRAP(mpage->ptr);
  mpage->used += size;
  mpage->free -= size;
  mpage->ptr = (char*)mpage->ptr + size;

  MOBJ_UNWRAP_SIZE(ptr) = size - sizeof(struct _SAC_MObj);
  MOBJ_UNWRAP_MPAGE(ptr) = mpage;

  return ptr;
}



#define MPOOL(mpool) ((struct _SAC_MPool*)(mpool))

struct _SAC_MPool {
  size_t page_size;
  struct _SAC_MPage *page;
};



SAC_MPool SAC_mpool_open(size_t page_size) {
  struct _SAC_MPool *mpool;

  mpool = (struct _SAC_MPool*)malloc(sizeof(struct _SAC_MPool));

  if (mpool == NULL) return NULL;

  mpool->page_size = page_size;
  mpool->page = mpage_open(NULL, mpool->page_size);
  return mpool;
}



void SAC_mpool_close(SAC_MPool mpool) {
  if (mpool == NULL) return;
  
  mpage_close_all(MPOOL(mpool)->page);
  free(mpool);
}



void* SAC_mpool_alloc(SAC_MPool mpool, size_t size) {
  void *ptr;

  if (size == 0) return NULL;

  ptr = mpage_alloc(MPOOL(mpool)->page, size);
  if (ptr == NULL) {
    struct _SAC_MPage *page;
    size_t page_size=
      size < MPOOL(mpool)->page_size ? MPOOL(mpool)->page_size : size;

    page = mpage_open(MPOOL(mpool)->page, page_size);

    if (page == NULL) return NULL;

    MPOOL(mpool)->page = page;
    ptr = mpage_alloc(MPOOL(mpool)->page, size);
  }

  return ptr;
}



void* SAC_mpool_realloc(SAC_MPool mpool, void *ptr, size_t size) {
  if (ptr == NULL) return SAC_mpool_alloc(mpool, size);

  if (size == 0) {
    SAC_mpool_free(mpool, ptr);
    return NULL;
  }

  if (MOBJ_UNWRAP_SIZE(ptr) < size) {
    void *dest = SAC_mpool_alloc(mpool, size);

    if (dest == NULL) return NULL;

    memcpy(dest, ptr, MOBJ_UNWRAP_SIZE(ptr));
    SAC_mpool_free(mpool, ptr);
    return dest;
  }
  
  return ptr;
}



void SAC_mpool_free(SAC_MPool mpool, void *ptr) {
  struct _SAC_MPage *page;

  if (ptr == NULL) return;
  
  page = MOBJ_UNWRAP_MPAGE(ptr);
  page->used -= MOBJ_UNWRAP_SIZE(ptr) + sizeof(struct _SAC_MObj);

  if (page->used == 0 && !(page->prev == NULL && MPOOL(mpool)->page == page)) {
    struct _SAC_MPage *cur;
    struct _SAC_MPage *next;

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



void SAC_mpool_stats(SAC_MPool mpool, size_t *pages) {
  size_t _pages;
  struct _SAC_MPage *cur;
  
  for (cur = MPOOL(mpool)->page, _pages = 0; cur != NULL; cur = cur->prev)
    ++_pages;

  *pages = _pages;
}
