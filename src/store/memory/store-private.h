#include "uthash.h"
typedef struct nhpm_channel_head_s nhpm_channel_head_t;
typedef struct nhpm_channel_head_cleanup_s nhpm_channel_head_cleanup_t;
typedef struct nhpm_subscriber_cleanup_s nhpm_subscriber_cleanup_t;
typedef struct nhpm_subscriber_s nhpm_subscriber_t;
typedef struct nhpm_message_s nhpm_message_t;

struct nhpm_subscriber_cleanup_s {
  nhpm_channel_head_cleanup_t  *shared;
  nhpm_subscriber_t            *sub;
}; //nhpm_subscriber_cleanup_t

struct nhpm_subscriber_s {
  ngx_uint_t                  id;
  subscriber_t                *subscriber;
  ngx_event_t                 ev;
  ngx_pool_t                 *pool;
  struct nhpm_subscriber_s   *prev;
  struct nhpm_subscriber_s   *next;
  ngx_http_cleanup_t         *r_cln;
  nhpm_subscriber_cleanup_t   clndata;
};

typedef enum {INACTIVE, NOTREADY, READY} chanhead_pubsub_status_t;

struct nhpm_message_s {
  ngx_http_push_msg_t      *msg;
  nhpm_message_t           *prev;
  nhpm_message_t           *next;
}; //nhpm_message_t

struct nhpm_channel_head_s {
  ngx_str_t                      id; //channel id
  ngx_http_push_channel_t        channel;
  ngx_atomic_t                   generation; //subscriber pool generation.
  ngx_pool_t                    *pool;
  chanhead_pubsub_status_t       status;
  ngx_atomic_t                   sub_count;
  ngx_uint_t                     min_messages;
  ngx_uint_t                     max_messages;
  nhpm_message_t                *msg_first;
  nhpm_message_t                *msg_last;
  nhpm_channel_head_cleanup_t   *shared_cleanup;
  nhpm_subscriber_t             *sub;
  ngx_http_push_msg_id_t         last_msgid;
  void                          *ipc_sub; //points to NULL or inacceessible memory.
  nhpm_llist_timed_t             cleanlink;
  UT_hash_handle                 hh;
};

struct nhpm_channel_head_cleanup_s {
  nhpm_channel_head_t        *head;
  ngx_str_t                   id; //channel id
  ngx_uint_t                  sub_count;
  ngx_pool_t                 *pool;
};

typedef struct {
  nhpm_channel_head_t *subhash;
  ngx_event_t         *chanhead_gc_timer;
  nhpm_llist_timed_t  *chanhead_gc_head;
  nhpm_llist_timed_t  *chanhead_gc_tail;
} shm_data_t;

nhpm_channel_head_t * ngx_http_push_store_find_chanhead(ngx_str_t *channel_id);
nhpm_message_t *chanhead_find_next_message(nhpm_channel_head_t *ch, ngx_http_push_msg_id_t *msgid, ngx_int_t *status);
shmem_t *ngx_http_push_memstore_get_shm(void);
ipc_t *ngx_http_push_memstore_get_ipc(void);
ngx_int_t ngx_http_push_memstore_handle_get_message_reply(ngx_http_push_msg_t *msg, ngx_int_t findmsg_status, void *d);


