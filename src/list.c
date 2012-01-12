#include "list.h"



#define LIST_ITEM(item) ((struct _SAC_ListItem*)(item))

#define LIST_ITEM_WRAP(item) ((void*)(item))

#define LIST_ITEM_UNWRAP(item) LIST_ITEM(item)

struct _SAC_ListItem {
  void *obj;
  struct _SAC_ListItem *next;
};



static struct _SAC_ListItem* list_item_open(SAC_MPool mpool, void *obj) {
  struct _SAC_ListItem *ret =
    (struct _SAC_ListItem*)mpool_alloc(mpool, sizeof(struct _SAC_ListItem));

  ret->obj = obj;
  ret->next = NULL;
  return ret;
}



static void list_item_close(struct _SAC_ListItem *item, SAC_MPool mpool) {
  mpool_free(mpool, item);
}



static void list_item_close_all(struct _SAC_ListItem *item, SAC_MPool mpool) {
  if (item->next != NULL) list_item_close_all(item->next, mpool);
  list_item_close(item, mpool);
}



#define LIST(list) ((struct _SAC_List*)(list))

struct _SAC_List {
  struct _SAC_ListItem *head;
  struct _SAC_ListItem *tail;
  size_t size;
};



SAC_List list_open(SAC_MPool mpool) {
  struct _SAC_List *ret =
    (struct _SAC_List*)mpool_alloc(mpool, sizeof(struct _SAC_List));

  ret->head = list_item_open(mpool, NULL);
  ret->tail = ret->head;
  ret->size = 0;

  return ret;
}



void list_close(SAC_List list, SAC_MPool mpool) {
  list_item_close_all(LIST(list)->head, mpool);
  mpool_free(mpool, list);
}



list_iter_t list_head(SAC_List list) {
  return LIST_ITEM_WRAP(LIST(list)->head->next);
}



list_iter_t list_next(list_iter_t iter) {
  return LIST_ITEM_WRAP(LIST_ITEM_UNWRAP(iter)->next);
}



void list_push_front(SAC_List list, SAC_MPool mpool, void *obj) {
  struct _SAC_ListItem *item;

  item = list_item_open(mpool, obj);

  item->next = LIST(list)->head->next;
  LIST(list)->head->next = item;
  ++LIST(list)->size;
}



void list_push_back(SAC_List list, SAC_MPool mpool, void *obj) {
  struct _SAC_ListItem *item;

  item = list_item_open(mpool, obj);
  
  LIST(list)->tail->next = item;
  LIST(list)->tail = item;
  ++LIST(list)->size;
}



size_t list_size(SAC_List list) {
  return LIST(list)->size;
}
