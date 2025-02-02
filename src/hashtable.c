/* vim: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */

/*
 * Copyright 2015 University of Piemonte Orientale, Computer Science Institute
 *
 * This file is part of UPOalglib.
 *
 * UPOalglib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * UPOalglib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with UPOalglib.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <assert.h>
#include "hashtable_private.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <upo/error.h>
#include <upo/utility.h>

/* Le aggiunte che ho fatto sono sempre seguite da un commento con dei ++++++++++ */
/* Le modifiche che ho fatto sono sempre seguite da un commento contenente il codice precedente */

/*** EXERCISE #1 - BEGIN of HASH TABLE with SEPARATE CHAINING ***/

upo_ht_sepchain_t upo_ht_sepchain_create(size_t m, upo_ht_hasher_t key_hash, upo_ht_comparator_t key_cmp) {
	upo_ht_sepchain_t ht = NULL;
	size_t i = 0;

	/* preconditions */
	assert( key_hash != NULL );
	assert( key_cmp != NULL );

	/* Allocate memory for the hash table type */
	ht = (upo_ht_sepchain_t)malloc(sizeof(struct upo_ht_sepchain_s)); /* +++++++ Aggiunto il cast (upo_ht_sepchain_t) */
	if (ht == NULL) {
		perror("Unable to allocate memory for Hash Table with Separate Chaining");
		abort();
	}

	/* Allocate memory for the array of slots */
	ht->slots = malloc(m*sizeof(upo_ht_sepchain_slot_t));
	if (ht->slots == NULL) {
		perror("Unable to allocate memory for slots of the Hash Table with Separate Chaining");
		abort();
	}

	/* Initialize fields */
	for (i = 0; i < m; ++i)
		ht->slots[i].head = NULL;
	ht->capacity = m;
	ht->size = 0;
	ht->key_hash = key_hash;
	ht->key_cmp = key_cmp;

	return ht;
}

void upo_ht_sepchain_destroy(upo_ht_sepchain_t ht, int destroy_data) {
	if (ht != NULL) {
		upo_ht_sepchain_clear(ht, destroy_data);
		free(ht->slots);
		free(ht);
	}
}

void upo_ht_sepchain_clear(upo_ht_sepchain_t ht, int destroy_data) {
	if (ht != NULL && ht->slots != NULL) {
		size_t i = 0;

		/* For each slot, clear the associated list of collisions */
		for (i = 0; i < ht->capacity; ++i) {
			upo_ht_sepchain_list_node_t* list = NULL;
			list = ht->slots[i].head;

			while (list != NULL) {
				upo_ht_sepchain_list_node_t* node = list;
				list = list->next;

				if (destroy_data) {
					free(node->key);
					free(node->value);
				}

				free(node);
			}
			ht->slots[i].head = NULL;
		}
		ht->size = 0;
	}
}

/* Helping function */
upo_ht_sepchain_list_node_t* upo_ht_sepchain_list_node_create(void* k, void* v) {
	upo_ht_sepchain_list_node_t* node;
	node = malloc(sizeof(struct upo_ht_sepchain_list_node_s));

	node->key = k;
	node->value = v;

	return node;
}

void* upo_ht_sepchain_put(upo_ht_sepchain_t ht, void* key, void* value) {

	if(ht != NULL) {

		void* old_value = NULL;
		int key_hash = (int) ht->key_hash(key, ht->capacity);
		upo_ht_sepchain_list_node_t* n = ht->slots[key_hash].head;

		while ((n != NULL) && (ht->key_cmp(key, n->key) != 0))
				n = n->next;

		if (n == NULL) {
				n = upo_ht_sepchain_list_node_create(key, value);
				n->next = ht->slots[key_hash].head;
				ht->slots[key_hash].head = n;
			}

			else {
				old_value = n->value;
				n->value = value;
			}

		return old_value;
	}

	return NULL;
}

void upo_ht_sepchain_insert(upo_ht_sepchain_t ht, void* key, void* value) {

	if(ht != NULL) {
			int key_hash = (int) ht->key_hash(key, ht->capacity);
			upo_ht_sepchain_list_node_t* n = ht->slots[key_hash].head;

			while ((n != NULL) && (ht->key_cmp(key, n->key) != 0))
				n = n->next;

			if (n == NULL) {
				n = upo_ht_sepchain_list_node_create(key, value);
				n->next = ht->slots[key_hash].head;
				ht->slots[key_hash].head = n;
		}
	}
}

void* upo_ht_sepchain_get(const upo_ht_sepchain_t ht, const void* key) {

	if(ht != NULL) {
		int key_hash = (int) ht->key_hash(key, ht->capacity);
		upo_ht_sepchain_list_node_t* n = ht->slots[key_hash].head;

		while ((n != NULL) && (ht->key_cmp(key, n->key)!= 0))
			n = n->next;

		if (n != NULL)
			return n->value;
	}

	return NULL;
}

int upo_ht_sepchain_contains(const upo_ht_sepchain_t ht, const void* key) {

	if (upo_ht_sepchain_get(ht, key) != NULL)
		return 1;
	else
		return 0;
}

void upo_ht_sepchain_destroy_node(upo_ht_sepchain_list_node_t* x, int dd) {

	if(dd) {
		free(x->value);
		free(x->key);
	}

	free(x);
}

void upo_ht_sepchain_delete(upo_ht_sepchain_t ht, const void* key, int destroy_data) {

	if (ht != NULL) {
		int key_hash = (int) ht->key_hash(key, ht->capacity);
		upo_ht_sepchain_list_node_t* n = ht->slots[key_hash].head;
		upo_ht_sepchain_list_node_t* p = NULL;

		while ((n != NULL) && (ht->key_cmp(key, n->key)!=0)) {
			p = n;
			n = n ->next;
		}

		if (n != NULL) {
			if (p == NULL)
				ht->slots[key_hash].head = n->next;
			else
				p->next = n->next;

			upo_ht_sepchain_destroy_node(n, destroy_data);
		}
	}
}

size_t upo_ht_sepchain_size(const upo_ht_sepchain_t ht) {

	if (ht!=NULL) {
		int i=0;
		int c = (int) ht->capacity;
		size_t count_keys =0;
		upo_ht_sepchain_list_node_t* node;

		for (i=0; i<c ; i++) {
			node = ht->slots[i].head;

		while (node != NULL) {
			if (node->key != NULL)
				count_keys++;
			node= node->next;
			}

		}
		return count_keys;
	}

	return 0;
}

int upo_ht_sepchain_is_empty(const upo_ht_sepchain_t ht) {
	return upo_ht_sepchain_size(ht) == 0 ? 1 : 0;
}

size_t upo_ht_sepchain_capacity(const upo_ht_sepchain_t ht) {
	return (ht != NULL) ? ht->capacity : 0;
}

double upo_ht_sepchain_load_factor(const upo_ht_sepchain_t ht) {
	/* ++++++++++++ Attenzione alla divisione per 0 */
	return upo_ht_sepchain_size(ht) / (double) upo_ht_sepchain_capacity(ht);
}


/*** EXERCISE #1 - END of HASH TABLE with SEPARATE CHAINING ***/


/*** EXERCISE #2 - BEGIN of HASH TABLE with LINEAR PROBING ***/


void upo_ht_linprob_resize(upo_ht_linprob_t ht, size_t n) {
	/* preconditions */
	assert( n > 0 );

	if (ht != NULL) {
		/* The hash table must be rebuilt from scratch since the hash value of
		 * keys will be in general different (due to the change in the
		 * capacity). */

		size_t i = 0;
		upo_ht_linprob_t new_ht = NULL;

		/* Create a new temporary hash table */
		new_ht = upo_ht_linprob_create(n, ht->key_hash, ht->key_cmp);
		if (new_ht == NULL) {
			perror("Unable to allocate memory for slots of the Hash Table with Separate Chaining");
			abort();
		}

		/* Put in the temporary hash table the key-value pairs stored in the
		 * hash table to resize.
		 * Note: by calling function 'put' we are also rehashing the keys
		 * according to the new capacity. */
		for (i = 0; i < ht->capacity; ++i)
			if (ht->slots[i].key != NULL)
				upo_ht_linprob_put(new_ht, ht->slots[i].key, ht->slots[i].value);

		/* Copy the new slots in the old hash table.
		 * To do so we use a trick that avoids to loop for each key-value pair:
		 * swap the array of slots, the size and the capacity between new and
		 * old hash tables. */
		upo_swap(&ht->slots, &new_ht->slots, sizeof ht->slots);
		upo_swap(&ht->capacity, &new_ht->capacity, sizeof ht->capacity);
		upo_swap(&ht->size, &new_ht->size, sizeof ht->size);

		/* Destroy temporary hash table */
		upo_ht_linprob_destroy(new_ht, 0);
	}
}

upo_ht_linprob_t upo_ht_linprob_create(size_t m, upo_ht_hasher_t key_hash, upo_ht_comparator_t key_cmp) {
	upo_ht_linprob_t ht = NULL;
	size_t i = 0;

	/* preconditions */
	assert( key_hash != NULL );
	assert( key_cmp != NULL );

	/* Allocate memory for the hash table type */
	ht = malloc(sizeof(struct upo_ht_linprob_s));
	if (ht == NULL) {
		perror("Unable to allocate memory for Hash Table with Linear Probing");
		abort();
	}

	/* Allocate memory for the array of slots */
	if (m > 0) {
		ht->slots = malloc(m*sizeof(upo_ht_linprob_slot_t));
		if (ht->slots == NULL) {
			perror("Unable to allocate memory for slots of the Hash Table with Separate Chaining");
			abort();
		}

		/* Initialize the slots */
		for (i = 0; i < m; ++i) {
			ht->slots[i].key = NULL;
			ht->slots[i].value = NULL;
			ht->slots[i].tombstone = 0;
		}
	}

	ht->capacity = m;
	ht->size = 0;
	ht->key_hash = key_hash;
	ht->key_cmp = key_cmp;

	return ht;
}

void upo_ht_linprob_destroy(upo_ht_linprob_t ht, int destroy_data) {
	if (ht != NULL) {
		upo_ht_linprob_clear(ht, destroy_data);
		free(ht->slots);
		free(ht);
	}
}

void upo_ht_linprob_clear(upo_ht_linprob_t ht, int destroy_data) {
	if (ht != NULL && ht->slots != NULL) {
		size_t i = 0;

		/* For each slot, clear the associated list of collisions */
		for (i = 0; i < ht->capacity; ++i)
			if (ht->slots[i].key != NULL) {
				if (destroy_data) {
					free(ht->slots[i].key);
					free(ht->slots[i].value);
				}
				ht->slots[i].key = NULL;
				ht->slots[i].value = NULL;
				ht->slots[i].tombstone = 0;
			}
		ht->size = 0;
	}
}

void* upo_ht_linprob_put(upo_ht_linprob_t ht, void* key, void* value) {

	if (ht!=NULL) {
		if (ht->capacity != 0) {
			void* old_value = NULL;
			double load_factor = upo_ht_linprob_load_factor(ht);
			int key_hash = (int) ht->key_hash(key, ht->capacity);
			int found_tomb = 0;
			int key_tomb;

			if (load_factor >= 0.5)
				upo_ht_linprob_resize(ht, ht->capacity *2);

			while ((ht->slots[key_hash].key != NULL && key != ht->slots[key_hash].key) || ht->slots[key_hash].tombstone ) {
				if (ht->slots[key_hash].tombstone && !found_tomb) {
					found_tomb = 1;
					key_tomb= key_hash;
				}

				key_hash= (key_hash+1)% ht->capacity;
			}

			if (ht->slots[key_hash].key == NULL) {
				if (found_tomb)
					key_hash=key_tomb;
				ht->slots[key_hash].key = key;
				ht->slots[key_hash].value = value;
				ht->slots[key_hash].tombstone=0;
			}

			else {
				old_value= ht->slots[key_hash].value;
				ht->slots[key_hash].value = value;
			}

			return old_value;
		}
	}

	return NULL;
}

void upo_ht_linprob_insert(upo_ht_linprob_t ht, void* key, void* value) {

	if (ht!=NULL) {
		if (ht->capacity != 0) {
			double load_factor = upo_ht_linprob_load_factor(ht);
			int key_hash = (int) ht->key_hash(key, ht->capacity);
			int found_tomb = 0;
			int key_tomb;

			if (load_factor >= 0.5)
			upo_ht_linprob_resize(ht, ht->capacity *2);

			while ((ht->slots[key_hash].key != NULL && key != ht->slots[key_hash].key) || ht->slots[key_hash].tombstone ) {
				if (ht->slots[key_hash].tombstone && !found_tomb) {
					found_tomb = 1;
					key_tomb= key_hash;
				}
				key_hash= (key_hash+1)% ht->capacity;
			}

			if (ht->slots[key_hash].key == NULL) {
				if (found_tomb)
					key_hash=key_tomb;
				ht->slots[key_hash].key = key;
				ht->slots[key_hash].value = value;
				ht->slots[key_hash].tombstone=0;
			}
		}
	}
}

void* upo_ht_linprob_get(const upo_ht_linprob_t ht, const void* key) {

	if (ht != NULL) {
		int key_hash = (int) ht->key_hash(key, ht->capacity);
		while ((ht->slots[key_hash].key != NULL && key != ht->slots[key_hash].key) || ht->slots[key_hash].tombstone )
			key_hash = (key_hash+1) % ht->capacity;
		if (ht->slots[key_hash].key != NULL)
			return ht->slots[key_hash].value;
	}

	return NULL;
}

int upo_ht_linprob_contains(const upo_ht_linprob_t ht, const void* key) {

	if (ht != NULL) {
		if (upo_ht_linprob_get (ht, key))
			return 1;
	}

	return 0;
}

void upo_ht_linprob_delete(upo_ht_linprob_t ht, const void* key, int destroy_data) {

	if (ht!= NULL) {
		if(ht->capacity != 0) {
			int key_hash= (int)ht->key_hash(key, ht->capacity);

			while ((ht->slots[key_hash].key != NULL && key != ht->slots[key_hash].key) || ht->slots[key_hash].tombstone )
				key_hash = (key_hash+1) % ht->capacity;

			if (ht->slots[key_hash].key != NULL) {
				if(destroy_data) {
					free(ht->slots[key_hash].key);
					free(ht->slots[key_hash].value);
				}
				ht->slots[key_hash].key = NULL;
				ht->slots[key_hash].value = NULL;
				ht->slots[key_hash].tombstone= 1;
				if (upo_ht_linprob_load_factor(ht) <= 0.125)
					upo_ht_linprob_resize(ht, ht->capacity /2);
			}
		}
	}
}

size_t upo_ht_linprob_size(const upo_ht_linprob_t ht) {

	if (ht != NULL) {
		size_t count=0;
		size_t i=0;
		for (i=0; i<ht->capacity; i++)
			if (ht->slots[i].tombstone == 0 && ht->slots[i].key!= NULL)
				count ++;
		return count;
	}

	return 0;
}

int upo_ht_linprob_is_empty(const upo_ht_linprob_t ht) {
	return upo_ht_linprob_size(ht) == 0 ? 1 : 0;
}

size_t upo_ht_linprob_capacity(const upo_ht_linprob_t ht) {
	return (ht != NULL) ? ht->capacity : 0;
}

double upo_ht_linprob_load_factor(const upo_ht_linprob_t ht) {
	return upo_ht_linprob_size(ht) / (double) upo_ht_linprob_capacity(ht);
}


/*** EXERCISE #2 - END of HASH TABLE with LINEAR PROBING ***/


/*** EXERCISE #3 - BEGIN of HASH TABLE - EXTRA OPERATIONS ***/


upo_ht_key_list_t upo_ht_sepchain_keys(const upo_ht_sepchain_t ht){

	if(ht != NULL) {
		size_t i=0;
		upo_ht_key_list_t list=NULL, temp=NULL;
		upo_ht_sepchain_list_node_t* node=NULL;
		for (i=0; i< ht->capacity; i++)
			if (ht->slots[i].head != NULL) {
				node = ht->slots[i].head;
				while (node != NULL) {
					if (list==NULL) {
						list =malloc (sizeof(upo_ht_key_list_node_t));
						list->key= node->key;
						list->next=NULL;
						temp=list;
					}
					else {
						temp->next= malloc (sizeof(upo_ht_key_list_node_t));
						temp=temp->next;
						temp->key=node->key;
						temp->next=NULL;
					}
					node=node->next;
				}
			}
		return list;
	}
	return NULL;
}

void upo_ht_sepchain_traverse(const upo_ht_sepchain_t ht, upo_ht_visitor_t visit, void* visit_arg) {

	if (ht != NULL) {
		size_t i=0;
		upo_ht_sepchain_list_node_t* node = NULL;
		for (i=0; i< ht->capacity; i++)
			if (ht->slots[i].head != NULL) {
				node = ht->slots[i].head;
				while (node != NULL){
					visit (node->key, node->value, visit_arg);
					node=node->next;
				}
			}
	}
}

upo_ht_key_list_t upo_ht_linprob_keys(const upo_ht_linprob_t ht) {

	if (ht!=NULL) {
		upo_ht_key_list_t list=NULL, temp=NULL;
		size_t i=0;
		for (i=0; i<ht->capacity; i++)
			if (ht->slots[i].key != NULL && !ht->slots[i].tombstone) {
				if (list==NULL){
					list =malloc (sizeof(upo_ht_key_list_node_t));
					list->key= ht->slots[i].key;
					list->next=NULL;
					temp=list;
				}
				else {
					temp->next= malloc (sizeof(upo_ht_key_list_node_t));
					temp=temp->next;
					temp->key=ht->slots[i].key;
					temp->next=NULL;
				}
			}
		return list;
	}
	return NULL;
}

void upo_ht_linprob_traverse(const upo_ht_linprob_t ht, upo_ht_visitor_t visit, void* visit_arg) {
	if (ht != NULL) {
		size_t i=0;
		for (i=0; i< ht->capacity; i++)
			if (ht->slots[i].key != NULL && !ht->slots[i].tombstone)
				visit( ht->slots[i].key,ht->slots[i].value , visit_arg );
	}
}


/*** EXERCISE #3 - END of HASH TABLE - EXTRA OPERATIONS ***/


/*** BEGIN of HASH FUNCTIONS ***/


size_t upo_ht_hash_int_div(const void* x, size_t m) {
	/* preconditions */
	assert( x != NULL );
	assert( m > 0 );

	return *((int*) x) % m;
}

size_t upo_ht_hash_int_mult(const void* x, double a, size_t m) {
	/* preconditions */
	assert( x != NULL );
	assert( a > 0 && a < 1 );
	assert( m > 0 );

	return floor( m * fmod(a * *((int*) x), 1.0) );
}

size_t upo_ht_hash_int_mult_knuth(const void* x, size_t m) {
	return upo_ht_hash_int_mult(x, 0.5*(sqrt(5)-1), m);
}

size_t upo_ht_hash_str(const void* x, size_t h0, size_t a, size_t m) {
	const char* s = x;
	size_t h = h0;

	/* preconditions */
	assert( x != NULL );
	assert( m > 0 );
/*
    assert( a < m );
*/
	assert( h0 < m );

	for (; *s; ++s) 
	{
		h = (a*h + *s) % m;
	}
	return h;
}

size_t upo_ht_hash_str_djb2(const void* x, size_t m) {
	return upo_ht_hash_str(x, 5381U, 33U, m);
}

size_t upo_ht_hash_str_djb2a(const void* x, size_t m) {
	const char* s = x;
	size_t h = 5381U;

	/* preconditions */
	assert( x != NULL );
	assert( m > 0 );

	for (; *s; ++s)
		h = (33U*h ^ *s) % m;

	return h;
}

size_t upo_ht_hash_str_java(const void* x, size_t m) {
	return upo_ht_hash_str(x, 0U, 31U, m);
}

size_t upo_ht_hash_str_kr2e(const void* x, size_t m) {
	return upo_ht_hash_str(x, 0U, 31U, m);
}

size_t upo_ht_hash_str_sgistl(const void* x, size_t m) {
	return upo_ht_hash_str(x, 0U, 5U, m);
}

size_t upo_ht_hash_str_stlport(const void* x, size_t m) {
	return upo_ht_hash_str(x, 0U, 33U, m);
}

/*** END of HASH FUNCTIONS ***/
