/****************************************************************************
    Copyright (C) 1987-2015 by Jeffery P. Hansen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
****************************************************************************/
/*
    Data structure and initilization for the hash table.  The hash table is
    used to look up circuit elements by name.  This is used durring the download
    phase and when setting and deleting scope probes.
*/

#ifndef __hash_h
#define __hash_h

#include "config.h"

#if HAVE_STDINT_H
#include <stdint.h>
#endif

#include "misc.h"

#define INITIAL_HASHSIZE	32
#define HASH_MAXLOAD		4

__BEGIN_DECLS

union hash_key {
  char *s;
  intptr_t d;
  void *p;
};

typedef struct hash_elem_str {
  union			hash_key key;
  uintptr_t		hashcode;
  void			*value;
  struct		hash_elem_str *next;
} HashElem;

typedef struct hash_str {
  void		*vtable;	/* Memory allocator function table */
  unsigned	size;		/* Number of hash buckets (must be power of 2) */
  uintptr_t	mask;		/* Mask for hash addressed (size-1) */
  unsigned	num;		/* Number of elements in hash */
  int		loop_ok;	/* OK to use Hash_next() */
  HashElem	**elems;	/* Elements buffer */
} Hash;

/* String hash */
typedef Hash SHash;
/* Number hash */
typedef Hash NHash;
/* Pointer hash */
typedef Hash PHash;

typedef void HashElemDelFunc(HashElem *, Hash *);

uintptr_t computestrhash(const char *);

void SHashElem_uninit(HashElem *, Hash *);

/*******************************************************************************
 * Create new Hash
 *
 * param int use_ob Usage of the undoable objects flag
 *
 ******************************************************************************/
Hash *new_Hash(int);

void Hash_init(Hash *, int);

void delete_Hash(Hash *, HashElemDelFunc *);

void Hash_uninit(Hash *, HashElemDelFunc *);

HashElem *Hash_first(Hash *);

HashElem *Hash_next(Hash *, HashElem *);

void Hash_flush(Hash * H, HashElemDelFunc * hdel);

void Hash_resize(Hash * H, int reqSize);

void *SHash_find(Hash *, const char *);

#define HashElem_obj(E)		(E)->value
#define SHashElem_key(E)	(E)->key.s
#define NHashElem_key(E)	(E)->key.d
#define PHashElem_key(E)	(E)->key.p
#define Hash_numElems(H)	(H)->num

#define new_SHash()	((SHash*)new_Hash(1))
#define new_SHash_noob()	((SHash*)new_Hash(0))
#define delete_SHash(H)	delete_Hash(H,SHashElem_uninit)
#define SHash_init(H)	Hash_init(H,1)
#define SHash_uninit(H)	Hash_uninit(H,SHashElem_uninit)
int SHash_insert(Hash*,const char*,void*);
int SHash_replace(Hash*,const char*,void*);
int SHash_remove(Hash*,const char*);
#define SHash_flush(H)	Hash_flush(H,SHashElem_uninit)
#define SHash_resize(H, reqSize) Hash_resize(H,reqSize)

#define new_NHash()	((NHash*)new_Hash(1))
#define new_NHash_noob()	((NHash*)new_Hash(0))
#define delete_NHash(H)	delete_Hash(H,0)
#define NHash_init(H)	Hash_init(H,1)
#define NHash_uninit(H)	Hash_uninit(H,0)
void *NHash_find(Hash*,intptr_t);
int NHash_insert(Hash*,intptr_t,void*);
int NHash_replace(Hash*,intptr_t,void*);
int NHash_remove(Hash*,intptr_t);
#define NHash_flush(H)	Hash_flush(H,0)
#define NHash_resize(H, reqSize) Hash_resize(H,reqSize)

#define new_PHash()	((PHash*)new_Hash(1))
#define new_PHash_noob() ((PHash*)new_Hash(0))
#define delete_PHash(H)	delete_Hash(H,0)
#define PHash_init(H)	Hash_init(H,1)
#define PHash_init_noob(H)	Hash_init(H,0)
#define PHash_uninit(H)	Hash_uninit(H,0)
#define PHash_find(H,P) NHash_find(H,(intptr_t)(P))
#define PHash_insert(H,P,O) NHash_insert(H,(intptr_t)(P),O)
#define PHash_remove(H,P) NHash_remove(H,(intptr_t)(P))
#define PHash_flush(H)	Hash_flush(H,0)
#define PHash_resize(H, reqSize) Hash_resize(H,reqSize)

__END_DECLS

#endif // __hash_h
