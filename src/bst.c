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

#include "bst_private.h"
#include <stdio.h>
#include <stdlib.h>


/**** EXERCISE #1 - BEGIN of FUNDAMENTAL OPERATIONS ****/


upo_bst_t upo_bst_create(upo_bst_comparator_t key_cmp) {
	upo_bst_t tree = malloc(sizeof(struct upo_bst_s));
	if (tree == NULL) {
		perror("Unable to create a binary search tree");
		abort();
	}

	tree->root = NULL;
	tree->key_cmp = key_cmp;

	return tree;
}

void upo_bst_destroy(upo_bst_t tree, int destroy_data) {
	if (tree != NULL) {
		upo_bst_clear(tree, destroy_data);
		free(tree);
	}
}

void upo_bst_clear_impl(upo_bst_node_t* node, int destroy_data) {
	if (node != NULL) {
		upo_bst_clear_impl(node->left, destroy_data);
		upo_bst_clear_impl(node->right, destroy_data);

		if (destroy_data) {
			free(node->key);
			free(node->value);
		}

		free(node);
	}
}

void upo_bst_clear(upo_bst_t tree, int destroy_data) {
	if (tree != NULL) {
		upo_bst_clear_impl(tree->root, destroy_data);
		tree->root = NULL;
	}
}

upo_bst_node_t* upo_bst_put_impl (upo_bst_node_t* node , void* key, void* value, void* oldv, upo_bst_comparator_t cmp) {
	oldv = NULL;
	if (node == NULL) {
		node = (upo_bst_node_t*) malloc ( sizeof(struct     upo_bst_node_s));
		node->key = key;
		node->value = value;
		node->left = NULL;
		node->right = NULL;
		return node;
	}
	else if (cmp (key ,node->key) < 0)
		node->left = upo_bst_put_impl (node->left, key, value, oldv, cmp);
	else if (cmp (key ,node->key) > 0)
		node->right = upo_bst_put_impl (node->right, key, value, oldv, cmp);
	else {
		oldv = node->value;
		node->value= value;
	}

	return node;
}
void* upo_bst_put(upo_bst_t tree, void* key, void* value) {
	if (tree != NULL) {
		void* oldv = NULL;

		upo_bst_comparator_t cmp = upo_bst_get_comparator(tree);

		tree->root = upo_bst_put_impl(tree->root, key, value, oldv, cmp);
		return oldv;
	}
	return NULL;
}

upo_bst_node_t* upo_bst_insert_impl (upo_bst_node_t* node,void* key, void* value, upo_bst_comparator_t cmp) {
	if (node == NULL) {
		node = (upo_bst_node_t*) malloc ( sizeof(struct upo_bst_node_s));
		node->key = key;
		node->value = value;
		node->left = NULL;
		node->right = NULL;
	}
	else if (cmp (key ,node->key) < 0)
		node->left = upo_bst_insert_impl (node->left, key, value, cmp);
	else if(cmp (key ,node->key) > 0)
		node->right = upo_bst_insert_impl (node->right, key, value, cmp);

	return node;
}

void upo_bst_insert(upo_bst_t tree, void* key, void* value) {
	if (tree != NULL) {
		upo_bst_comparator_t cmp;
		cmp = upo_bst_get_comparator(tree);

		tree->root = upo_bst_insert_impl (tree->root, key, value, cmp);
	}
}

upo_bst_node_t* upo_bst_get_impl (upo_bst_node_t* node, const void* key, upo_bst_comparator_t cmp) {
	if (node != NULL) {
		if (cmp (key ,node->key) < 0)
			return upo_bst_get_impl (node->left, key, cmp);
		else if(cmp (key ,node->key) > 0)
			return upo_bst_get_impl (node->right, key, cmp);

		return node;
	}
	return NULL;
}

void* upo_bst_get(const upo_bst_t tree, const void* key) {
	if (tree != NULL) {
		upo_bst_comparator_t cmp = upo_bst_get_comparator(tree);
		upo_bst_node_t* node = upo_bst_get_impl(tree->root, key, cmp);

		if (node != NULL)
			return node->value;
	}
	return NULL;
}

int upo_bst_contains(const upo_bst_t tree, const void* key) {
	if (tree != NULL) {
		upo_bst_comparator_t cmp;
		cmp = upo_bst_get_comparator(tree);
		if (upo_bst_get_impl (tree->root, key, cmp) != NULL)
			return 1;
	}
	return 0;
}

/* DAFAQ IS  THIS */
upo_bst_node_t* upo_bst_delete_impl (upo_bst_node_t* node, const void* key, int destroy_data, upo_bst_comparator_t cmp);

upo_bst_node_t* upo_bst_max_impl (upo_bst_node_t* node) {
	if (node != NULL) {
		if (node->right != NULL)
			return upo_bst_max_impl(node->right);
		return node;
	}
	return NULL;
}

upo_bst_node_t* upo_bst_delete_impl_two_c (upo_bst_node_t* node, const void* key, int destroy_data, upo_bst_comparator_t cmp) {
	upo_bst_node_t* m = upo_bst_max_impl (node->left);
	node->key = m->key;
	node->value = m->value;
	node->left = upo_bst_delete_impl (node->left, m->key, destroy_data, cmp);

	return node;
}

upo_bst_node_t* upo_bst_delete_impl_one_c (upo_bst_node_t* node, const void* key, int destroy_data, upo_bst_comparator_t cmp) {
	upo_bst_node_t* sign = node;
	if (node->left != NULL)
		 node = node->left;
	else node = node->right;

	if (destroy_data != 0) {
		free (sign->value);
		free(sign->key);
	}
	free(sign);
	return node;
}

upo_bst_node_t* upo_bst_delete_impl (upo_bst_node_t* node, const void* key, int destroy_data, upo_bst_comparator_t cmp) {
	if (node == NULL)
		return NULL;
	else if (cmp (key ,node->key) < 0)
		node->left = upo_bst_delete_impl (node->left, key, destroy_data, cmp);
	else if (cmp (key ,node->key) > 0)
		node->right = upo_bst_delete_impl (node->right, key, destroy_data, cmp);
	else if ((node->right != NULL) && (node->left != NULL))
		node = upo_bst_delete_impl_two_c (node, key, destroy_data, cmp);
	else
		node = upo_bst_delete_impl_one_c (node, key, destroy_data, cmp);

	return node;
}

void upo_bst_delete(upo_bst_t tree, const void* key, int destroy_data) {
	if (tree != NULL) {
		upo_bst_comparator_t cmp;
		cmp = upo_bst_get_comparator(tree);
		tree->root = upo_bst_delete_impl (tree->root, key, destroy_data, cmp);
	}
}

size_t upo_bst_size_impl (upo_bst_node_t* node) {
		if (node == NULL)
			return 0;
		else
			return 1 + upo_bst_size_impl(node->left) + upo_bst_size_impl(node->right);
}

size_t upo_bst_size(const upo_bst_t tree) {
	if (tree != NULL)
		return upo_bst_size_impl (tree->root);
	return 0;
}

size_t upo_bst_height_impl (upo_bst_node_t* node) {
	if (node == NULL || (node->left== NULL && node->right ==NULL))
		return 0;
	else if (upo_bst_height_impl(node->right) > upo_bst_height_impl(node->left))
		return 1+ upo_bst_height_impl(node->right);
	else
		return 1 + upo_bst_height_impl(node->left);
}

size_t upo_bst_height(const upo_bst_t tree) {
	if (tree!= NULL)
		return upo_bst_height_impl(tree->root);
	return 0;
}

void upo_bst_traverse_in_order_impl (upo_bst_node_t* node, upo_bst_visitor_t visit, void* visit_arg) {
	if (node != NULL) {
		upo_bst_traverse_in_order_impl (node->left, visit, visit_arg);
		visit (node->key, node->value, visit_arg);
		upo_bst_traverse_in_order_impl (node->right, visit, visit_arg);
	}
}

void upo_bst_traverse_in_order(const upo_bst_t tree, upo_bst_visitor_t visit, void* visit_arg) {
	if (tree != NULL)
		upo_bst_traverse_in_order_impl (tree->root, visit, visit_arg);
}

int upo_bst_is_empty(const upo_bst_t tree) {
	if ((tree == NULL) || (tree->root == NULL))
		return 1;
	return 0;
}


/**** EXERCISE #1 - END of FUNDAMENTAL OPERATIONS ****/

/**** EXERCISE #2 - BEGIN of EXTRA OPERATIONS ****/


void* upo_bst_min(const upo_bst_t tree) {
	if (tree != NULL) {
		upo_bst_node_t* node = tree->root;
		if(node!=NULL) {
			while (node->left != NULL)
				node= node->left;
			return node->key;
		}
	}
	return NULL;
}

void* upo_bst_max(const upo_bst_t tree) {
	if (tree != NULL) {
		upo_bst_node_t* node = tree->root;
		if (node!=NULL) {
			while (node->right != NULL)
				node= node->right;
			return node->key;
		}
	}
	return NULL;
}

void upo_bst_delete_min(upo_bst_t tree, int destroy_data) {
	if (tree != NULL) {
		upo_bst_node_t* prec = NULL;
		upo_bst_node_t* node=tree->root;
		if (node != NULL) {
			while(node->left != NULL) {
				prec=node;
				node=node->left;
			}
			if (prec==NULL)
				tree->root=node->right;
			else if ((node->right != NULL) && (node->left == NULL))
				prec->left=node->right;
			else
				prec->left=NULL;

			if (destroy_data) {
				free(node->key);
				free(node->value);
			}
			free (node);
		}
	}
}

void upo_bst_delete_max(upo_bst_t tree, int destroy_data) {
	if (tree != NULL) {
		upo_bst_node_t* prec = NULL;
		upo_bst_node_t* node=tree->root;

		if (node != NULL) {
			while(node->right != NULL) {
				prec=node;
				node=node->right;
			}
			if (prec==NULL)
				tree->root=node->left;
			else if (node->left != NULL)
				prec->right=node->left;
			else
				prec->right=NULL;

			if (destroy_data) {
				free(node->key);
				free(node->value);
			}
			free (node);
		}
	}
}

void* upo_bst_floor(const upo_bst_t tree, const void* key) {
	if (tree != NULL) {
		upo_bst_comparator_t cmp =upo_bst_get_comparator(tree);
		upo_bst_node_t* node= tree->root;
		if(node!= NULL) { /* se la radice esisite */
			upo_bst_node_t* prec= NULL;

			/* se la chiave è minore del minimo dell'albero*/
			if (cmp(key, upo_bst_min(tree)) < 0)
				return NULL;

			do{
				if(prec != NULL && cmp(key, node->key)<0 && cmp(key, prec->key)>0)
					return prec->key;

				prec = node;

				if(cmp(key, node->key)>0)
					node=node->right;
				else if(cmp(key, node->key)<0)
					node=node->left;
				else
					return node->key;

			} while(node != NULL);

			return prec->key;
		}
	}
	return NULL;
}

void* upo_bst_ceiling(const upo_bst_t tree, const void* key) {
 	if (tree != NULL) {
		upo_bst_comparator_t cmp= upo_bst_get_comparator(tree);
		upo_bst_node_t* node= tree->root;

		if(node!= NULL) { /* se la radice esisite */
			upo_bst_node_t* prec= NULL;

			/* se la chiave è maggiore del massimo dell'albero*/
			if ( cmp(key, upo_bst_max(tree)) >0)
				return NULL;

			do{
				if(prec != NULL && cmp(key, node->key)>0 && cmp(key, prec->key)<0)
					return prec->key;

				prec = node;

				if(cmp(key, node->key)>0)
					node=node->right;
				else if(cmp(key, node->key)<0)
					node=node->left;
				else
					return node->key;

			} while(node != NULL);

			return prec->key;
		}
	}
	return NULL;
}

upo_bst_key_list_t upo_bst_keys_range(const upo_bst_t tree, const void* low_key, const void* high_key) {
	if (tree != NULL)
		return upo_bst_keys_range_impl(tree->key_cmp, tree->root, low_key, high_key, NULL);
	return NULL;
}

upo_bst_key_list_t upo_bst_keys_range_impl(upo_bst_comparator_t cmp, upo_bst_node_t* node, const void* low, const void* high, upo_bst_key_list_t list) {
	if(node!=NULL) {
		list=upo_bst_keys_range_impl(cmp, node->left, low, high, list); /* right */

		if(cmp(node->key, low)>=0 && cmp(node->key, high)<=0) {
			upo_bst_key_list_t temp=malloc(sizeof( struct upo_bst_key_list_node_s));

			if(list!=NULL)
				temp->next=list;
			else
				temp->next=NULL;
			list=temp;
			temp->key=node->key;
		}

		return upo_bst_keys_range_impl(cmp, node->right, low, high, list); /* left */
	}
	return list;
}



upo_bst_key_list_t upo_bst_keys_impl (upo_bst_comparator_t cmp,upo_bst_node_t* node, upo_bst_key_list_t list) {
	if(node!=NULL) {
		upo_bst_key_list_t temp=malloc(sizeof( struct upo_bst_key_list_node_s));
		list=upo_bst_keys_impl(cmp, node->left, list); /* right */

		if(list!=NULL)
			temp->next=list;
		else
			temp->next=NULL;
		list=temp;
		temp->key=node->key;

		return upo_bst_keys_impl(cmp, node->right, list); /* left */
	}
	return list;
}

upo_bst_key_list_t upo_bst_keys(const upo_bst_t tree) {
	if (tree!=NULL)
		return upo_bst_keys_impl (tree->key_cmp,tree->root, NULL);
	return NULL;
}

int upo_bst_is_bst(const upo_bst_t tree, const void* min_key, const void* max_key) {
	if (tree!= NULL) {
		upo_bst_node_t* node=tree->root;
		if (node!= NULL)
			return upo_bst_is_bst_impl (node, min_key,max_key,tree->key_cmp);
	}
	return 1;
}

int upo_bst_is_bst_impl (upo_bst_node_t* node, const void* min, const void* max, upo_bst_comparator_t cmp) {
	int val1 =0;
	int val2 =0;
	if (node == NULL) return 1;
	if ( cmp (node->key, max)> 0 || cmp(node->key, min) <0 ) return 1;

	if(node->left != NULL) {
		if((cmp(node->left->key, node->key) <0))
			val1 = upo_bst_is_bst_impl (node->left, min, max, cmp);
	}
	else val1=1;

	if (node->right != NULL) {
		if (cmp(node->right->key, node->key) >0 )
			val2 = upo_bst_is_bst_impl (node->right, min, max, cmp);
	}
	else val2 = 1;

	if (!val1 || !val2) return 0;
	else return 1;
}


/*
    if (node == NULL) return 1;
   if (cmp(node->key, min) <0 || cmp (node->key, max) > 0 ) return 0;
    if ((cmp(node->left->key, node->key) <0 || node->left ==NULL) && (cmp(node->right->key, node->key) >0 || node->right ==NULL)){
        upo_bst_is_bst_impl (node->left, min, max, cmp);
        upo_bst_is_bst_impl (node->right, min, max, cmp);
        return 1;
    } else return 0;*/
/*
    if (node == NULL) return 1;
    if (node->left != NULL && cmp (node->left->key, node->key) > 0 ) return 0;
    if (node->right != NULL && cmp (node->right->key, node->key) > 0) return 0;
    if (!upo_bst_is_bst_impl (node->left, min, max, cmp) || !upo_bst_is_bst_impl (node->right, min, max, cmp)) return 0;
    return 1;*/


/**** EXERCISE #2 - END of EXTRA OPERATIONS ****/


upo_bst_comparator_t upo_bst_get_comparator(const upo_bst_t tree) {
	if (tree != NULL)
		return tree->key_cmp;
	return NULL;
}
