#include "list.h"



#define LIST_ITEM(item) ((struct list_item_s*)(item))

#define LIST_ITEM_WRAP(item) ((void*)(item))

#define LIST_ITEM_UNWRAP(item) LIST_ITEM(item)

struct list_item_s {
  void *obj;
  struct list_item_s *next;
};



struct list_item_s* list_item_open(mpool_t mpool, void *obj) {
  struct list_item_s *ret =
    (struct list_item_s*)mpool_alloc(mpool, sizeof(struct list_item_s));

  ret->obj = obj;
  ret->next = NULL;
  return ret;
}



void list_item_close(struct list_item_s *item, mpool_t mpool) {
  mpool_free(mpool, item);
}



void list_item_close_all(struct list_item_s *item, mpool_t mpool) {
  if (item->next != NULL) list_item_close_all(item->next, mpool);
  mpool_free(mpool, item);
}



#define LIST(list) ((struct list_s*)(list))

struct list_s {
  struct list_item_s *head;
  struct list_item_s *tail;
};



list_t list_open(mpool_t mpool) {
  struct list_s *ret =
    (struct list_s*)mpool_alloc(mpool, sizeof(struct list_s));

  ret->head = list_item_open(mpool, NULL);
  ret->tail = ret->head;

  return ret;
}



void list_close(list_t list, mpool_t mpool) {
  list_item_close_all(LIST(list)->head, mpool);
  mpool_free(mpool, list);
}



list_iter_t list_head(list_t list) {
  return LIST_ITEM_WRAP(LIST(list)->head->next);
}



list_iter_t list_next(list_iter_t iter) {
  return LIST_ITEM_WRAP(LIST_ITEM_UNWRAP(iter)->next);
}



void list_push_front(list_t list, mpool_t mpool, void *obj) {
  struct list_item_s *item;

  item = list_item_open(mpool, obj);

  item->next = LIST(list)->head->next;
  LIST(list)->head->next = item;
}



void list_push_back(list_t list, mpool_t mpool, void *obj) {
  struct list_item_s *item;

  item = list_item_open(mpool, obj);
  
  LIST(list)->tail->next = item;
  LIST(list)->tail = item;
}
