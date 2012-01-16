#include "list.h"



#define LIST_ITEM(item) ((struct _SAC_ListItem*)(item))

#define LIST_ITEM_WRAP(item) ((void*)(item))

#define LIST_ITEM_UNWRAP(item) LIST_ITEM(item)

struct _SAC_ListItem {
  void *obj;
  struct _SAC_ListItem *next;
};



static struct _SAC_ListItem* SAC_list_item_alloc(SAC_MPool mpool, void *obj) {
  struct _SAC_ListItem *item =
    (struct _SAC_ListItem*)SAC_mpool_alloc(mpool, sizeof(struct _SAC_ListItem));

  if (item == NULL) return NULL;

  item->obj = obj;
  item->next = NULL;
  return item;
}



static void SAC_list_item_free(struct _SAC_ListItem *item, SAC_MPool mpool) {
  if (item->next != NULL) SAC_list_item_free(item->next, mpool);
  SAC_mpool_free(mpool, item);
}



#define LIST(list) ((struct _SAC_List*)(list))

struct _SAC_List {
  struct _SAC_ListItem *head;
  struct _SAC_ListItem *tail;
  size_t size;
};



SAC_List SAC_list_open(SAC_MPool mpool) {
  struct _SAC_List *list =
    (struct _SAC_List*)SAC_mpool_alloc(mpool, sizeof(struct _SAC_List));

  if (list == NULL) return NULL;

  list->head = SAC_list_item_alloc(mpool, NULL);
  list->tail = list->head;
  list->size = 0;

  return list;
}



void SAC_list_close(SAC_List list, SAC_MPool mpool) {
  SAC_list_item_free(LIST(list)->head, mpool);
  SAC_mpool_free(mpool, list);
}



SAC_ListIter SAC_list_head(SAC_List list) {
  return LIST_ITEM_WRAP(LIST(list)->head->next);
}



SAC_ListIter SAC_list_next(SAC_ListIter iter) {
  return LIST_ITEM_WRAP(LIST_ITEM_UNWRAP(iter)->next);
}



void* SAC_list_push_front(SAC_List list, SAC_MPool mpool, void *obj) {
  struct _SAC_ListItem *item;

  item = SAC_list_item_alloc(mpool, obj);

  if (item == NULL) return NULL;

  item->next = LIST(list)->head->next;
  LIST(list)->head->next = item;
  ++LIST(list)->size;
  return obj;
}



void* SAC_list_push_back(SAC_List list, SAC_MPool mpool, void *obj) {
  struct _SAC_ListItem *item;

  item = SAC_list_item_alloc(mpool, obj);
  
  if (item == NULL) return NULL;

  LIST(list)->tail->next = item;
  LIST(list)->tail = item;
  ++LIST(list)->size;
  return obj;
}



size_t SAC_list_size(SAC_List list) {
  return LIST(list)->size;
}
