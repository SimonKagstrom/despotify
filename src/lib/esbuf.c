/*
 * Estoy buffer library.
 * Copyright (c) Estoy Ltd. Seychelles IBC
 * This is free to use in any product, commercial or non-commercial, closed or open source, as long as 
 * the following conditions are met:
 * a)
 *   You are NOT affiliated with the RIAA, MPAA, MPA, IFPI, or any similar organization in any way.
 * b)
 *   Credit is given to "Estoy Ltd. Seychelles IBC <http://www.estoykh.com>"
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>

#include "esbuf.h"

esbuf_ctx *ctx_hash[ESBUF_CTX_HASH_SIZE];
unsigned int ctx_hash_initialized = 0;
unsigned int prng_initialized = 0;
unsigned int cur_line = 0;

/* Specify which function to use to generate new contexts.
 * vanilla_rand uses normal rand()
 * randfunc_inc just increases the context number
 * This might provide some additional security/increase of difficulty of exploitation
 * in case of certain programming errors.
 */
#define RANDFUNC vanilla_rand
//#define RANDFUNC randfunc_inc

//#define ESBUF_DEBUG

#define ES_REALSIZE(size)	(((size) <= GRAIN_SIZE && (size) != 0)? (size) : (((size) | (GRAIN_SIZE - 1)) + 1))
#define CTX_HASH_FN(hnd)	((hnd) & (ESBUF_CTX_HASH_SIZE - 1))

#ifdef ESBUF_TEST
#define ESBUF_DEBUG
#endif

#ifdef ESBUF_DEBUG
void dump_all_ctxs(void)
{
#if 0
  esbuf_ctx *ctx;
  
  printf("dumping ctxs head %.8x:\n", (unsigned int)ctx_head);
  ctx = ctx_head;
  while (ctx != NULL)
  {
    printf("  ctx @ %.8x; handle %.8x bufs %.8x\n", (unsigned int)ctx, (unsigned int)ctx->hnd, (unsigned int)ctx->head);
    ctx = ctx->next;
  }
  printf("end of dump\n");
#endif
}
#endif
unsigned int vanilla_rand(void)
{
  if (prng_initialized == 0)
  {
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    srand(tv.tv_sec ^ tv.tv_usec ^ (getpid() << 16));
    prng_initialized = 1;
  }
  return rand();
}
static unsigned int cur_ctx;
unsigned int randfunc_inc(void)
{
  if (prng_initialized == 0)
  {
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    cur_ctx = tv.tv_usec;
    prng_initialized = 1;
  }
  return cur_ctx ++; 
}
void esbuf_panic(unsigned char *file, unsigned int line, unsigned char *from_file, unsigned int from_line, unsigned char *format, ...)
{
  unsigned char msg[2048];
  
  va_list va;
  if (from_file != NULL)
  {
    snprintf(msg, sizeof(msg) - 1, "ESbuf PANIC @ %s:%u (from %s:%u): ", file, line, from_file, from_line); msg[sizeof(msg) - 1] = 0;
  } else
  {
    snprintf(msg, sizeof(msg) - 1, "ESbuf PANIC @ %s:%u: ", file, line); msg[sizeof(msg) - 1] = 0;
  }
  write(2, msg, strlen(msg));
  va_start(va, format);
  vsnprintf(msg, sizeof(msg) - 1, format, va); msg[sizeof(msg) - 1] = 0;
  va_end(va);
  write(2, msg, strlen(msg));
  write(2, "\n", 1);
  _exit(1);
}
esbuf_ctx *esbuf_find_ctx(esbuf_ctxh hnd)
{
  esbuf_ctx *cur;
  
  if (ctx_hash_initialized == 0)
  {
    unsigned int i;
    
    for (i = 0; i < ESBUF_CTX_HASH_SIZE; i ++) ctx_hash[i] = NULL;
    ctx_hash_initialized = 1;
  }
  cur = ctx_hash[CTX_HASH_FN(hnd)];
  while (cur != NULL) { if (cur->hnd == hnd) return cur; cur = cur->next; }
  return NULL;
}
esbuf_ctxh esbuf_new_ctx(void)
{
  esbuf_ctx *new;
  unsigned int idx;
  
  new = malloc(sizeof(*new));
  ESbuf_ASSERT(new != NULL);
  do
  {
    new->hnd = RANDFUNC();
  } while (esbuf_find_ctx(new->hnd) != NULL || new->hnd == ESbuf_INVALID_CTX);
  new->head = NULL;
  new->prev = NULL;
  new->next = ctx_hash[idx = CTX_HASH_FN(new->hnd)];
  if (new->next != NULL) new->next->prev = new;
  ctx_hash[idx] = new;
  return new->hnd;
}
esbuf *esbuf_init(esbuf_ctxh ctxh, AKsize_t _size)
{
  AKsize_t size;
  esbuf *new;
  esbuf_ctx *ctx;
  
  if ((ctx = esbuf_find_ctx(ctxh)) == NULL) ESbuf_PANIC("esbuf_init(): Invalid ctx %.8x", ctxh);
  size = ES_REALSIZE(_size); 
#ifdef ESBUF_DEBUG
  printf("DEBUG: req size %.8x, final size %.8x\n", _size, size);
#endif
  ESbuf_ASSERT(size >= _size);
  new = malloc(sizeof(esbuf));
  ESbuf_ASSERT(new != NULL);
  new->in_use = 1;
  new->size = size;
  new->idx = 0;
  new->prev = NULL;
  new->next = ctx->head;
  new->head = malloc(size);
  ESbuf_ASSERT(new->head != NULL);
  if (ctx->head != NULL) ctx->head->prev = new;
  ctx->head = new;
#ifdef ESBUF_DEBUG
  memset(new->head, 'A', new->size);
#endif
  return new;
}
void *esbuf_malloc(esbuf_ctxh ctxh, AKsize_t size)
{
  esbuf *buf;
  
  buf = esbuf_init(ctxh, size);
  ESbuf_ASSERT(buf != NULL);
  return esbuf_data(buf);
}

void esbuf_set_size(esbuf *buf, AKsize_t _size)
{
  AKsize_t size;
  
  ESbuf_dASSERT(buf != NULL);
  size = ES_REALSIZE(_size);
  ESbuf_ASSERT(size >= _size);
  if (size == buf->size) return;
  buf->head = realloc(buf->head, size);
  ESbuf_ASSERT(buf->head != NULL);
  buf->size = size;
  if (buf->idx > size) buf->idx = size;
  return;
}
void esbuf_consume(esbuf *buf, AKsize_t len)
{
  ESbuf_dASSERT(buf != NULL);
  if (len == 0) return;
  ESbuf_ASSERT(buf->idx >= len && buf->idx <= buf->size);
  memcpy(buf->head, buf->head + len, buf->idx - len);
  buf->idx -= len;
  if (len >= GRAIN_SIZE) esbuf_set_size(buf, buf->idx);
  return;
}
unsigned char esbuf_eat_byte(esbuf *buf)
{
  unsigned char ret;
  
  ESbuf_dASSERT(buf != NULL);
  ESbuf_ASSERT(buf->idx > 0 && buf->idx <= buf->size);
  ret = *buf->head;
  memcpy(buf->head, buf->head + 1, buf->idx - 1);
  buf->idx --;
  return ret;
}
void esbuf_set_data(esbuf *buf, unsigned char *data, AKsize_t len)
{
  ESbuf_dASSERT(buf != NULL && data != NULL);
  if (len > buf->size) esbuf_set_size(buf, len);
  memcpy(buf->head, data, len);
  buf->idx = len;
  return;
}
void esbuf_vsprintf(esbuf *buf, unsigned char *format, va_list va)
{
  AKssize_t n;
    
  ESbuf_dASSERT(buf != NULL && format != NULL);
  if ((n = vsnprintf(buf->head, buf->size, format, va)) >= buf->size)
  {
    esbuf_set_size(buf, n);
    n = vsnprintf(buf->head, n, format, va);
  }
  ESbuf_ASSERT(n >= 0);
  buf->idx = n;
  return;
}
void esbuf_sprintf(esbuf *buf, unsigned char *format, ...)
{
  va_list va;
  
  va_start(va, format);
  esbuf_vsprintf(buf, format, va);
  va_end(va);
  return;
}
void esbuf_appendf(esbuf *buf, unsigned char *format, ...)
{
  esbuf_ctxh ctx;
  esbuf *appendbuf;
  va_list va;
  
  ctx = esbuf_new_ctx();
  appendbuf = esbuf_init(ctx, 0);
  va_start(va, format);
  esbuf_vsprintf(appendbuf, format, va);
  va_end(va);
  esbuf_append_buf(buf, appendbuf); 
  esbuf_free_ctx(ctx);
}
void esbuf_append_data(esbuf *buf, unsigned char *data, AKsize_t len)
{
  ESbuf_dASSERT(buf != NULL && data != NULL);
  ESbuf_ASSERT(buf->idx + len >= buf->idx && buf->idx <= buf->size);
  if (buf->idx + len >= buf->size) esbuf_set_size(buf, buf->idx + len);
  memcpy(buf->head + buf->idx, data, len); buf->idx += len;
  return;
}
void esbuf_append_byte(esbuf *buf, unsigned char b)
{
  ESbuf_dASSERT(buf != NULL);
  ESbuf_ASSERT(buf->idx + 1 > buf->idx && buf->idx <= buf->size);
  if (buf->idx + 1 >= buf->size) esbuf_set_size(buf, buf->idx + 1);
  buf->head[buf->idx ++] = b;
  return;
}
int esbuf_chr(esbuf *buf, unsigned char b)
{
  unsigned char *p;
  
  ESbuf_dASSERT(buf != NULL);
  if ((p = memchr(buf->head, b, buf->idx)) == NULL) return -1;
  return (int)(p - buf->head);
}
void esbuf_asciiz(esbuf *buf)
{
  ESbuf_dASSERT(buf != NULL);
  ESbuf_ASSERT(buf->idx + 1 > buf->idx && buf->idx <= buf->size);
  if (buf->idx == buf->size) esbuf_set_size(buf, buf->idx + 1);
  buf->head[buf->idx] = 0;
  return;
}
void esbuf_free(esbuf_ctxh ctxh, esbuf *buf)
{
  esbuf_ctx *ctx;
  esbuf *cur;
  
  ESbuf_dASSERT(buf != NULL);
  if ((ctx = esbuf_find_ctx(ctxh)) == NULL) ESbuf_PANIC("esbuf_free(): Invalid ctx %.8x", ctxh);
  cur = ctx->head;
  while (cur != buf && cur != NULL) cur = cur->next;
  if (cur == NULL) ESbuf_PANIC("esbuf_free(): Unknown esbuf %.8x", (unsigned int)buf);
#ifdef ESBUF_DEBUG
  printf("DEBUG: freeing buf %.8x\n", (unsigned int)cur);
#endif
  ESbuf_ASSERT(cur->in_use == 1 && cur->head != NULL);
  cur->in_use = 0;
  if (cur->prev != NULL) cur->prev->next = cur->next;
  if (cur->next != NULL) cur->next->prev = cur->prev;
  if (cur == ctx->head) ctx->head = cur->next;
  cur->prev = cur->next = NULL;
  cur->idx = (unsigned int)-1;
#ifdef ESBUF_DEBUG
  memset(cur->head, 'F', cur->size);
#else
  //memset(cur->head, 'F', (cur->size > GRAIN_SIZE)? GRAIN_SIZE : cur->size);
#endif  
  cur->size = 0;
  AKfree(cur->head); cur->head = NULL;
  free(cur);
}
void _esbuf_free_ctx(esbuf_ctxh ctxh, unsigned char *from_file, unsigned int from_line)
{
  esbuf_ctx *ctx;
  esbuf *cur, *prev;
  unsigned int idx;
  
  if (ctxh == ESbuf_INVALID_CTX) return;
  if ((ctx = esbuf_find_ctx(ctxh)) == NULL) ESbuf_PANIC_FROM("esbuf_free_ctx(): Invalid ctx %.8x", ctxh);
  cur = ctx->head;
  while (cur != NULL)
  {
    prev = cur;
    cur = cur->next;
    ESbuf_ASSERT(prev->in_use == 1 && prev->head != NULL);
    prev->in_use = 0;
    prev->prev = prev->next = NULL;
    prev->idx = (unsigned int)-1;
#ifdef ESBUF_DEBUG
    printf("DEBUG: prev %.8x prev->size %.8x\n", (unsigned int)prev, prev->size);
    memset(prev->head, 'G', prev->size);
#endif    
    //memset(prev->head, 'F', (prev->size > GRAIN_SIZE)? GRAIN_SIZE : prev->size);
    prev->size = 0;
    AKfree(prev->head); prev->head = NULL;
    free(prev);
  }
  ctx->head = NULL;
  if (ctx->prev != NULL) ctx->prev->next = ctx->next;
  if (ctx->next != NULL) ctx->next->prev = ctx->prev;
  if (ctx == ctx_hash[idx = CTX_HASH_FN(ctxh)]) ctx_hash[idx] = ctx->next;
#ifdef ESBUF_DEBUG
  memset(ctx, 'C', sizeof(esbuf_ctx));
#endif
  free(ctx);
}
esbuf_table *esbuf_table_init(esbuf_ctxh ctxh, unsigned int type)
{
  esbuf *tblbuf;
  esbuf_table *ret;
  
  tblbuf = esbuf_init(ctxh, sizeof(esbuf_table));
  ESbuf_dASSERT(tblbuf != NULL);
  esbuf_set_idx(tblbuf, sizeof(esbuf_table));
  ret = (esbuf_table *)esbuf_data(tblbuf);
  ret->head = ret->tail = NULL;
  ret->type = type;
  ret->ctxh = ctxh;
  return ret;
}
esbuf_table_entry *esbuf_table_entry_add(esbuf_ctxh ctxh, esbuf_table *tbl, unsigned char *key, esbuf *data)
{
  esbuf *entbuf;
  esbuf_table_entry *new;
  
  ESbuf_dASSERT(tbl != NULL && key != NULL && data != NULL);
  ESbuf_ASSERT(ctxh == tbl->ctxh);
  if (esbuf_find_ctx(ctxh) == NULL) ESbuf_PANIC("esbuf_table_entry_add(): Invalid ctx %.08x", ctxh);
  if ((new = esbuf_table_entry_find(tbl, key)) != NULL)
  {
    esbuf_clone(new->data, data);
    return new;
  }
  entbuf = esbuf_init(ctxh, sizeof(esbuf_table_entry));
  ESbuf_dASSERT(entbuf != NULL);
  esbuf_set_idx(entbuf, sizeof(esbuf_table_entry));
  new = (esbuf_table_entry *)esbuf_data(entbuf);
  new->key = esbuf_init(ctxh, 0);
  esbuf_strcpy(new->key, key);
  esbuf_append_byte(new->key, 0);
  new->data = esbuf_init(ctxh, esbuf_idx(data));
  esbuf_clone(new->data, data);
  new->prev = tbl->tail;
  new->next = NULL;
  if (tbl->head == NULL) 
  {
    tbl->head = new;
  } else
  {
    tbl->tail->next = new;
  }
  tbl->tail = new;
  return new;
}
esbuf_table_entry *esbuf_table_entry_add_buf(esbuf_ctxh ctxh, esbuf_table *tbl, esbuf *key, esbuf *data)
{
  esbuf *entbuf;
  esbuf_table_entry *new;

  ESbuf_dASSERT(tbl != NULL && key != NULL && data != NULL);
  ESbuf_ASSERT(tbl->type == ESBUF_TABLE_BIN);
  ESbuf_ASSERT(ctxh == tbl->ctxh);
  if (esbuf_find_ctx(ctxh) == NULL) ESbuf_PANIC("esbuf_table_entry_add(): Invalid ctx %.08x", ctxh);
  entbuf = esbuf_init(ctxh, sizeof(esbuf_table_entry));
  ESbuf_dASSERT(entbuf != NULL);
  esbuf_set_idx(entbuf, sizeof(esbuf_table_entry));
  new = (esbuf_table_entry *)esbuf_data(entbuf);
  new->key = esbuf_init(ctxh, esbuf_idx(key));
  esbuf_clone(new->key, key);
  esbuf_append_byte(new->key, 0);
  new->data = esbuf_init(ctxh, esbuf_idx(data));
  esbuf_clone(new->data, data);
  new->prev = tbl->tail;
  new->next = NULL;
  if (tbl->head == NULL) 
  {
    tbl->head = new;
  } else
  {
    tbl->tail->next = new;
  }
  tbl->tail = new;
  return new;
}
esbuf_table_entry *esbuf_table_entry_add_str(esbuf_ctxh ctxh, esbuf_table *tbl, unsigned char *key, unsigned char *data)
{
  esbuf *buf;
  esbuf_ctxh ctx;
  esbuf_table_entry *ret;
  
  ctx = esbuf_new_ctx();
  buf = esbuf_init(ctx, 0);
  esbuf_strcpy(buf, data); /* esbuf_append_byte(buf, 0); varfor finns den har ??? */
  ret = esbuf_table_entry_add(ctxh, tbl, key, buf);
  esbuf_free_ctx(ctx);
  return ret;
}
esbuf_table_entry *esbuf_table_entry_find(esbuf_table *tbl, unsigned char *key)
{
  int (*cmpfn)();
  esbuf_table_entry *ent;
  
  ESbuf_dASSERT(tbl != NULL && key != NULL);
  ESbuf_ASSERT(tbl->type == ESBUF_TABLE_NOCASE || tbl->type == ESBUF_TABLE_CASE || tbl->type == ESBUF_TABLE_BIN);
  cmpfn = (tbl->type == ESBUF_TABLE_CASE || tbl->type == ESBUF_TABLE_BIN)? strcmp : strcasecmp;
  ent = tbl->head;
  while (ent != NULL)
  {
    if (cmpfn(esbuf_data(ent->key), key) == 0) return ent;
    ent = ent->next;
  }
  return NULL;
}
esbuf *esbuf_table_entry_get(esbuf_table *tbl, unsigned char *key)
{
  esbuf_table_entry *ent;
  
  if ((ent = esbuf_table_entry_find(tbl, key)) == NULL) return NULL;
  return ent->data;
}
unsigned char *esbuf_table_entry_get_str(esbuf_table *tbl, unsigned char *key)
{
  esbuf *buf;
  
  if ((buf = esbuf_table_entry_get(tbl, key)) == NULL) return NULL;
  esbuf_asciiz(buf);
  return esbuf_data(buf); 
}

void esbuf_urlencode_data(unsigned char *data, AKsize_t len, esbuf *outbuf)
{
  unsigned int i;
  unsigned int c;

  for (i = 0; i < len; i ++)
  {
    unsigned char hexchars[] = "0123456789ABCDEF";

    c = data[i];
    if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= '<'))
    {
      esbuf_append_byte(outbuf, c);
    } else
    {
      esbuf_append_byte(outbuf, '%');
      esbuf_append_byte(outbuf, hexchars[(c >> 4) & 0xf]);
      esbuf_append_byte(outbuf, hexchars[c & 0xf]);
    }
  }
}
void esbuf_base64encode_data(unsigned char *data, AKsize_t len, esbuf *outbuf)
{
  AKsize_t rem;
  unsigned char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  rem = len;
  while (rem > 2)
  {
    esbuf_append_byte(outbuf, b64chars[data[0] >> 2]);
    esbuf_append_byte(outbuf, b64chars[((data[0] & 0x03) << 4) + (data[1] >> 4)]);
    esbuf_append_byte(outbuf, b64chars[((data[1] & 0x0f) << 2) + (data[2] >> 6)]);
    esbuf_append_byte(outbuf, b64chars[data[2] & 0x3f]);
    data += 3;
    rem -= 3;
  }
  if (rem != 0)
  {
    esbuf_append_byte(outbuf, b64chars[data[0] >> 2]);
    if (rem > 1)
    {
      esbuf_append_byte(outbuf, b64chars[((data[0] & 0x03) << 4) + (data[1] >> 4)]);
      esbuf_append_byte(outbuf, b64chars[((data[1] & 0x0f) << 2)]);
      esbuf_append_byte(outbuf, '=');
    } else
    {
      esbuf_append_byte(outbuf, b64chars[((data[0] & 0x03) << 4)]);
      esbuf_append_byte(outbuf, '=');
      esbuf_append_byte(outbuf, '=');
    }
  }
}

/*
 * Serialization of data to/from a buffer.
 * All values are stored in NETWORK byte order (big endian).
 * Serialized data types:
 *   4		32-bit value (int)
 *   2		16-bit value (short)
 *   1		8-bit value (char)
 *   b          esbuf (32-bit len + data)
 *   s		ASCIIZ string (32-bit len + data)
 *   f		Current serialization format (32-bit len + data of format)
 *   m		Serialization marker (32 bit value ESBUF_SERIALIZATION_MAGIC)
 */
 
#define SERIALIZE_BYTE(v) esbuf_append_byte(outbuf, (v))
#define SERIALIZE_UINT32(v)\
  do{\
    SERIALIZE_BYTE(((v) >> 24) & 0xff);\
    SERIALIZE_BYTE(((v) >> 16) & 0xff);\
    SERIALIZE_BYTE(((v) >> 8) & 0xff);\
    SERIALIZE_BYTE((v) & 0xff);\
  } while (0)
#define SERIALIZE_UINT16(v)\
  do{\
    SERIALIZE_BYTE(((v) >> 8) & 0xff);\
    SERIALIZE_BYTE((v) & 0xff);\
  } while (0)
#define SERIALIZE_DATA(d, n)\
  do{\
    SERIALIZE_UINT32(n);\
    esbuf_append_data(outbuf, (d), (n));\
  } while (0)

#define UNSERIALIZE_BYTE(v)\
  do{\
    if (in_idx >= esbuf_idx(inbuf)) { cur_line = __LINE__; return -ret; }\
    esbuf_get_byte(inbuf, in_idx, v);\
    in_idx ++;\
  } while (0)
#define UNSERIALIZE_UINT32(v)\
  do{\
    unsigned char c[4];\
    UNSERIALIZE_BYTE(c[0]);\
    UNSERIALIZE_BYTE(c[1]);\
    UNSERIALIZE_BYTE(c[2]);\
    UNSERIALIZE_BYTE(c[3]);\
    (v) = (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];\
  } while (0)
#define UNSERIALIZE_UINT16(v)\
  do{\
    unsigned char c[2];\
    UNSERIALIZE_BYTE(c[0]);\
    UNSERIALIZE_BYTE(c[1]);\
    (v) = (c[0] << 8) | c[1];\
  } while (0)
#define UNSERIALIZE_DATA_TO_BUF(b)\
  do{\
    unsigned int data_len;\
    UNSERIALIZE_UINT32(data_len);\
    printf("DEBUG: ser data len %u\n", data_len);\
    if (data_len > ESBUF_MAX_SERIALIZED_DATA_LEN || esbuf_idx(inbuf) - in_idx < data_len) { cur_line = __LINE__; return -ret; }\
    esbuf_set_data((b), esbuf_data(inbuf) + in_idx, data_len);\
    in_idx += data_len;\
  } while (0)      
#define UNESRIALIZE_DATA_TO_STRING(b, size)\
  do{\
    unsigned int data_len;\
    UNSERIALIZE_UINT32(inbuf, data_len);\
    if (data_len > ESBUF_MAX_SERIALIZED_DATA_LEN || esbuf_idx(inbuf) - in_idx < data_len || data_len >= size) { cur_line = __LINE__; return -ret; }\
    memcpy((b), esbuf_idx(inbuf) + in_idx, data_len);\
    (b)[data_len] = 0;\
  } while (0)
    
int esbuf_serialize(esbuf *outbuf, unsigned char *format, ...)
{
  va_list va;
  unsigned char *curformat;
  unsigned int arg_ui;
  unsigned char *arg_uc;
  esbuf *arg_buf;
  int ret;
    
  va_start(va, format);
  curformat = format;
  ret = 0;
  while (*curformat != 0)
  {
    switch (*curformat ++)
    {
      case '4': /* 32-bit value */
      {
        arg_ui = va_arg(va, unsigned int);
        SERIALIZE_UINT32(arg_ui);
        ret ++;
        break;
      }
      case '2': /* 16-bit value */
      {
        arg_ui = va_arg(va, unsigned int);
        SERIALIZE_UINT16(arg_ui);
        ret ++;
        break;
      }
      case '1': /* 8-bit value */
      {
        arg_ui = va_arg(va, unsigned int);
        SERIALIZE_BYTE(arg_ui & 0xff);
        ret ++;
        break;
      }
      case 'b': /* esbuf */
      {
        arg_buf = va_arg(va, esbuf *);
        SERIALIZE_DATA(esbuf_data(arg_buf), esbuf_idx(arg_buf));
        ret ++;
        break;
      }
      case 's': /* ASCIIZ string */
      {
        size_t len;
        
        arg_uc = va_arg(va, unsigned char *);
        len = strlen(arg_uc);
        SERIALIZE_DATA(arg_uc, len);
        ret ++;
        break;
      }
      case 'f': /* serialization format */
      {
        size_t len;
        
        len = strlen(format);
        SERIALIZE_DATA(format, len);
        ret ++;
        break;
      }
      case 'm': /* serialization marker */
      {
        SERIALIZE_UINT32(ESBUF_SERIALIZATION_MAGIC);
        ret ++;
        break;
      }
    }
  }
  va_end(va);
  return ret;
}

int esbuf_unserialize(esbuf *inbuf, unsigned char *format, ...)
{
  va_list va;
  unsigned char *curformat;
  unsigned int *arg_ui;
  esbuf *arg_buf;
  unsigned int in_idx;
  int ret;
    
  va_start(va, format);
  curformat = format;
  ret = 0;
  in_idx = 0;

  while (*curformat != 0)
  {
    switch (*curformat ++)
    {
      case '4': /* 32-bit value */
      {
        arg_ui = va_arg(va, unsigned int *);
        UNSERIALIZE_UINT32(*arg_ui);
        ret ++;
        break;
      }
      case '2': /* 16-bit value */
      {
        arg_ui = va_arg(va, unsigned int *);
        UNSERIALIZE_UINT16(*arg_ui);
        ret ++;
        break;
      }
      case '1': /* 8-bit value */
      {
        arg_ui = va_arg(va, unsigned int *);
        UNSERIALIZE_BYTE(*arg_ui);
        ret ++;
        break;
      }
      case 'b': /* esbuf */
      {
        arg_buf = va_arg(va, esbuf *);
        UNSERIALIZE_DATA_TO_BUF(arg_buf);
        ret ++;
        break;
      }
      case 'm': /* serialization marker */
      {
        unsigned int tmp;
        
        UNSERIALIZE_UINT32(tmp);
        if (tmp != ESBUF_SERIALIZATION_MAGIC) return -ret;
        ret ++;
        break;
      }
    }
  }
  va_end(va);
  return in_idx;
}

int esbuf_atoi(esbuf *buf, unsigned int *dest)
{
  unsigned int i, prev;
  
  *dest = 0;
  for (i = 0; i < esbuf_idx(buf); i ++)
  {
    if (esbuf_data(buf)[i] < '0' || esbuf_data(buf)[i] > '9') return -i;
    prev = *dest;
    *dest *= 10;
    if (*dest < prev) return -i;
    *dest += esbuf_data(buf)[i] - '0';
  }
  return i;
}

#ifdef ESBUF_TEST
int main(int argc, char *argv[])
{
  esbuf_ctxh ctx;
  esbuf *buf, *buf2, *buf3;
  int i, j;
  esbuf_table *tbl;
  esbuf_table_entry *ent;
  
  printf("esbuf_new_ctx()\n");
  ctx = esbuf_new_ctx();
  printf(" = %.8x\n", (unsigned int)ctx);
  printf("esbuf_new_ctx() second = %.8x\n", (unsigned int)esbuf_new_ctx());
  dump_all_ctxs();
  
  buf = esbuf_init(ctx, 0);
  esbuf_strcpy(buf, "blutti");
  printf("esbuf_table_init() =\n");
  tbl = esbuf_table_init(ctx, 0);
  printf("  %.08x\n", (unsigned int)tbl);
  printf("esbuf_table_entry_add() =\n");
  ent = esbuf_table_entry_add(ctx, tbl, "foobar", buf);
  printf("  %.08x\n", (unsigned int)ent);
  printf("[%s]\n", esbuf_data(ent->key));
  esbuf_strcpy(buf, "fnutti");
  ent = esbuf_table_entry_add(ctx, tbl, "foobar2", buf);
  printf("  next = %.08x prev = %.08x head = %.08x tail = %.08x\n", (unsigned int)ent->next, (unsigned int)ent->prev, (unsigned int)tbl->head, (unsigned int)tbl->tail);
  printf("esbuf_table_entry_get(..., \"foobar\")\n");
  buf2 = esbuf_table_entry_get(tbl, "foobar");
  if (buf2 != NULL)
  {
    esbuf_asciiz(buf2);
    printf("  [%s]\n", esbuf_data(buf2)); 
  }
  printf("esbuf_table_entry_get(..., \"foobar2\")\n");
  buf2 = esbuf_table_entry_get(tbl, "foobar2");
  if (buf2 != NULL)
  {
    esbuf_asciiz(buf2);
    printf("  [%s]\n", esbuf_data(buf2)); 
  }
  
#if 1
  printf("esbuf_init(0x242)\n");
  buf = esbuf_init(ctx, 0x242);
  printf(" = %.8x\n", (unsigned int)buf);
  //printf("esbuf_init(0xffffffff)\n");
  //esbuf_init(ctx, 0xffffffff);
  esbuf_strcpy(buf, "foobar blutti");
  esbuf_append_str(buf, " fnutti");
  esbuf_asciiz(buf);
  printf("[%s]\n", esbuf_data(buf));
  buf2 = esbuf_init(ctx, 0);
  esbuf_strcpy(buf2, "1 2 3");
  esbuf_append_str(buf2," 4 5 6");
  esbuf_consume(buf2, 4);
  esbuf_asciiz(buf2);
  printf("[%s]\n", esbuf_data(buf2));
  buf = esbuf_init(ctx, 0);
  esbuf_sprintf(buf, "foobar %u [%s]", 0x242, "foo");
  esbuf_asciiz(buf);
  printf("fmt [%s]\n", esbuf_data(buf));
  printf("freeing\n");
  esbuf_free_ctx(ctx);
  dump_all_ctxs();
  ctx = esbuf_new_ctx();
  buf = esbuf_init(ctx, 0x666);
  esbuf_strcpy(buf, "foo\nbar\nblutti\nfnutti");
  buf2 = esbuf_init(ctx, 0);
  while ((i = esbuf_chr(buf, '\n')) != -1)
  {
    esbuf_split(buf, buf2, i);
    esbuf_asciiz(buf2);
    printf("split [%s]\n", esbuf_data(buf2));
  }
  esbuf_asciiz(buf);
  printf("rem [%s]\n", esbuf_data(buf));
  
  /* test serialization */
  esbuf_set_idx(buf, 0);
  esbuf_strcpy(buf2, "TESTbuf");
  if ((i = esbuf_serialize(buf, "4sbm", 0x23, "TESTstr", buf2)) < 0) { printf("esbuf_serialize() returned %d\n", i); exit(1); }
  printf("serialized buf:");
  for (i = 0; i < esbuf_idx(buf); i ++) printf(" %.2x", esbuf_data(buf)[i]);
  printf("\n");
  
  j = -1;
  esbuf_set_idx(buf2, 0);
  buf3 = esbuf_init(ctx, 0);
  if ((i = esbuf_unserialize(buf, "4bbm", &j, buf2, buf3)) < 0) { printf("esbuf_unserialize() returned %d line %u\n", i, cur_line); exit(1); }
  printf("serialization inbuf idx %d\n", i);
  printf("serializion uint32 0x23 0x%x\n", j);
  esbuf_asciiz(buf2);
  printf("serialization buf TESTstr '%s'\n", esbuf_data(buf2));
  esbuf_asciiz(buf3);
  printf("serialization buf TESTbuf '%s'\n", esbuf_data(buf3));
  
  dump_all_ctxs();
  esbuf_free_ctx(ctx);
#endif
#ifdef TEST_DOUBLE_FREE
  esbuf_free_ctx(ctx);
#endif
  return 0;
}
#endif
