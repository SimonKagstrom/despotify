#define GRAIN_SIZE			0x1000		/* must be power of 2 */
#define ESBUF_CTX_HASH_SIZE		0x400000	/* must be power of 2 */

#define ESBUF_SERIALIZATION_MAGIC	0x23C0FFEE	/* Magic value of serialization marker. */
#define ESBUF_MAX_SERIALIZED_DATA_LEN	0x1700000	/* Max length of a serialized buffer/string. */

#define AKsize_t	size_t
#define AKssize_t	ssize_t

#define ESbuf_PANIC(f...) esbuf_panic(__FILE__, __LINE__, NULL, 0, ##f)
#define ESbuf_PANIC_FROM(f...) esbuf_panic(__FILE__, __LINE__, from_file, from_line, ##f)
#define ESbuf_ASSERT(cond) if (!(cond)) esbuf_panic(__FILE__, __LINE__, NULL, 0, "Assertion (" #cond ") failed")
#ifdef ESBUF_DEBUG
#define ESbuf_dASSERT(cond) ESbuf_ASSERT(cond)
#else
#define ESbuf_dASSERT(cond)
#endif

#define AKfree(ptr) if ((ptr) != NULL) { free(ptr); (ptr) = NULL; }

typedef unsigned int esbuf_ctxh;

#define ESbuf_INVALID_CTX ((esbuf_ctxh)-1)

typedef struct esbuf_s esbuf;
typedef struct esbuf_ctx_s esbuf_ctx;

struct esbuf_s
{
  unsigned int in_use;
  esbuf *prev;
  esbuf *next;
  unsigned int idx;
  AKsize_t size;
  unsigned char *head;
};

struct esbuf_ctx_s
{
  esbuf_ctxh hnd;
  esbuf *head;
  esbuf_ctx *prev;
  esbuf_ctx *next; 
};

typedef struct esbuf_table_s esbuf_table;
typedef struct esbuf_table_entry_s esbuf_table_entry;

struct esbuf_table_s
{
  esbuf_ctxh ctxh;
  unsigned int type;
  esbuf_table_entry *head;
  esbuf_table_entry *tail;
};

struct esbuf_table_entry_s
{
  esbuf *key;
  esbuf *data;
  esbuf_table_entry *prev;
  esbuf_table_entry *next;
};

enum { ESBUF_TABLE_NOCASE, ESBUF_TABLE_CASE, ESBUF_TABLE_BIN };

void esbuf_panic(unsigned char *, unsigned int, unsigned char *, unsigned int, unsigned char *, ...);
esbuf_ctxh esbuf_new_ctx(void);
void esbuf_free(esbuf_ctxh, esbuf *);
void _esbuf_free_ctx(esbuf_ctxh, unsigned char *, unsigned int);
void esbuf_consume(esbuf *, AKsize_t);
unsigned char esbuf_eat_byte(esbuf *);
int esbuf_chr(esbuf *, unsigned char);
void esbuf_asciiz(esbuf *);

esbuf *esbuf_init(esbuf_ctxh, AKsize_t);
void *esbuf_malloc(esbuf_ctxh, AKsize_t);
void esbuf_set_size(esbuf *, AKsize_t);
void esbuf_set_data(esbuf *, unsigned char *, AKsize_t);
void esbuf_append_data(esbuf *, unsigned char *, AKsize_t);
void esbuf_append_byte(esbuf *, unsigned char);
void esbuf_vsprintf(esbuf *, unsigned char *, va_list);
void esbuf_sprintf(esbuf *, unsigned char *, ...);
void esbuf_appendf(esbuf *, unsigned char *, ...);

esbuf_table *esbuf_table_init(esbuf_ctxh, unsigned int);
esbuf_table_entry *esbuf_table_entry_add(esbuf_ctxh, esbuf_table *, unsigned char *, esbuf *);
esbuf_table_entry *esbuf_table_entry_add_buf(esbuf_ctxh, esbuf_table *, esbuf *, esbuf *);
esbuf_table_entry *esbuf_table_entry_add_str(esbuf_ctxh, esbuf_table *, unsigned char *, unsigned char *);
esbuf_table_entry *esbuf_table_entry_find(esbuf_table *, unsigned char *);
esbuf *esbuf_table_entry_get(esbuf_table *, unsigned char *);
unsigned char *esbuf_table_entry_get_str(esbuf_table *, unsigned char *);

int esbuf_serialize(esbuf *, unsigned char *, ...);
int esbuf_unserialize(esbuf *, unsigned char *, ...);

int esbuf_atoi(esbuf *, unsigned int *);

#define esbuf_table_walk_start(tbl, ent) ((ent) = (tbl)->head) 
#define esbuf_table_walk_next(ent) ((ent) = (ent)->next)
#define esbuf_table_entry_key(ent) ((ent)->key)
#define esbuf_table_entry_data(ent) ((ent)->data)
#define esbuf_table_has_data(tbl) ((tbl)->head != NULL)
#define esbuf_table_is_last_entry(ent) ((ent)->next == NULL)

void esbuf_urlencode_data(unsigned char *, AKsize_t, esbuf *);
void esbuf_base64encode_data(unsigned char *, AKsize_t, esbuf *);

#define esbuf_free_ctx(ctx) _esbuf_free_ctx((ctx), __FILE__, __LINE__)
#define esbuf_idx(buf) ((buf)->idx)
#define esbuf_size(buf) ((buf)->size)
#define esbuf_data(buf) ((buf)->head)
#define esbuf_empty(buf) (esbuf_idx(buf) == 0)

#define esbuf_strcpy(buf, str) esbuf_set_data((buf), (str), strlen(str))
#define esbuf_append_str(buf, str) esbuf_append_data((buf), (str), strlen(str))
#define esbuf_clone(buf, src) esbuf_set_data((buf), esbuf_data(src), esbuf_idx(src))
#define esbuf_append_buf(buf, src) esbuf_append_data((buf), esbuf_data(src), esbuf_idx(src))

#define esbuf_peek_byte(buf) ((akbuf_idx(buf) > 0)? akbuf_data(buf)[0] : 0) /* Note that this function does NOT abort if the buffer is empty - it just returns 0. Fixme? */

#define esbuf_urlencode(buf, out) esbuf_urlencode(esbuf_data(buf), esbuf_idx(buf), (out))

#define esbuf_split(buf, dest, idx)\
	{\
	  esbuf_set_data((dest), esbuf_data((buf)), (idx));\
	  esbuf_consume((buf), (idx) + 1);\
	}

#define esbuf_set_byte(buf, idx, byte)\
	{\
	  ESbuf_ASSERT((unsigned int)(idx) < esbuf_idx(buf) && (unsigned int)(idx) < esbuf_size(buf));\
	  *(esbuf_data(buf) + (idx)) = (byte);\
	}
#define esbuf_consume_end(buf, len)\
	{\
	  ESbuf_ASSERT((unsigned int)(len) <= esbuf_idx(buf));\
	  esbuf_idx(buf) -= (len);\
	}
#define esbuf_set_idx(buf, idx)\
	{\
	  ESbuf_ASSERT((unsigned int)(idx) <= esbuf_size(buf));\
	  esbuf_idx(buf) = (idx);\
	}
#define esbuf_get_byte(buf, idx, byte)\
	{\
	  ESbuf_ASSERT((unsigned int)(idx) < esbuf_idx(buf));\
	  (byte) = *(esbuf_data(buf) + (idx));\
	}

#define esbuf_get_uint32(buf, idx, dest)\
	{\
	  ESbuf_ASSERT((unsigned int)((idx) + 3) < esbuf_idx(buf) && (unsigned int)((idx) + 3) > (unsigned int)(idx));\
	  (dest) = (*(esbuf_data(buf) + (idx)) & 0xff) << 24;\
	  (dest) |= (*(esbuf_data(buf) + (idx) + 1) & 0xff) << 16;\
	  (dest) |= (*(esbuf_data(buf) + (idx) + 2) & 0xff) << 8;\
	  (dest) |= (*(esbuf_data(buf) + (idx) + 3) & 0xff);\
	}
#define esbuf_append_uint32(buf, data)\
	{\
	  esbuf_append_byte(buf, ((data) >> 24) & 0xff);\
	  esbuf_append_byte(buf, ((data) >> 16) & 0xff);\
	  esbuf_append_byte(buf, ((data) >> 8) & 0xff);\
	  esbuf_append_byte(buf, ((data)     ) & 0xff);\
	}
#define esbuf_append_uint16(buf, data)\
	{\
	  esbuf_append_byte(buf, ((data) >> 8) & 0xff);\
	  esbuf_append_byte(buf, ((data)     ) & 0xff);\
	}
