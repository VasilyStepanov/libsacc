#include "mpool.h"

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
  struct _SAC_MPage *ret = malloc(sizeof(struct _SAC_MPage));

  size = MALLIGN(size + sizeof(struct _SAC_MObj));
  ret->prev = prev;
  ret->data = (char*)malloc(size);
  ret->ptr = ret->data;
  ret->free = size;
  ret->used = 0;
  return ret;
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
  void *ret;

  size = MALLIGN(size + sizeof(struct _SAC_MObj));

  if (mpage->free < size) return NULL;

  ret = MOBJ_WRAP(mpage->ptr);
  mpage->used += size;
  mpage->free -= size;
  mpage->ptr = (char*)mpage->ptr + size;

  MOBJ_UNWRAP_SIZE(ret) = size - sizeof(struct _SAC_MObj);
  MOBJ_UNWRAP_MPAGE(ret) = mpage;

  return ret;
}



#define MPOOL(mpool) ((struct _SAC_MPool*)(mpool))

struct _SAC_MPool {
  size_t page_size;
  struct _SAC_MPage *page;
};



SAC_MPool SAC_mpool_open(size_t page_size) {
  struct _SAC_MPool *ret;

  ret = (struct _SAC_MPool*)malloc(sizeof(struct _SAC_MPool));
  ret->page_size = page_size;
  ret->page = mpage_open(NULL, ret->page_size);
  return ret;
}



void SAC_mpool_close(SAC_MPool mpool) {
  if (mpool != NULL) mpage_close_all(MPOOL(mpool)->page);
  free(mpool);
}



void* SAC_mpool_alloc(SAC_MPool mpool, size_t size) {
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



void* SAC_mpool_realloc(SAC_MPool mpool, void *ptr, size_t size) {
  if (ptr == NULL) return SAC_mpool_alloc(mpool, size);

  if (size == 0) {
    SAC_mpool_free(mpool, ptr);
    return NULL;
  }

  if (MOBJ_UNWRAP_SIZE(ptr) < size) {
    void *ret = SAC_mpool_alloc(mpool, size);

    memcpy(ret, ptr, MOBJ_UNWRAP_SIZE(ptr));
    SAC_mpool_free(mpool, ptr);
    return ret;
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
