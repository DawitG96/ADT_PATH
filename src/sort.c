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
#include "sort_private.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <../include/upo/utility.h>

void upo_bubble_sort(void* base, size_t n, size_t size, upo_sort_comparator_t cmp)
{
    /* Variabili */
    size_t i;
    size_t j;
    char* a = (char*) base;
    size_t cont=1;
	int k=0;

	while(cont>1 || k%2==1)
	{
		if(k%2==0)
			cont=0;
		//scansione dispari/pari
		for(j = k%2; j < n-2; j+=2)
		{
			if(cmp(&a[j*size],&a[(j+1)*size]) > 0)
			{
				cont++;
				swap(&a[j*size],&a[(j+1)*size],size);
			}
		}
		k++;
		//scansione pari dispari
		/*
		for(j = 0; j < n-2; j+=2)
		{
			if(cmp(&a[j*size],&a[(j+1)*size]) > 0)
			{
				cont=1;
				swap(&a[j*size],&a[(j+1)*size],size);
			}
		}
		*/
	}     
}

void upo_insertion_sort(void* base, size_t n, size_t size, upo_sort_comparator_t cmp)
{
    /* Variabili */
    size_t i;
    size_t j;
    char* a = (char*) base;

    /* Codice pag 14 Lezione 06 - Sort */
    for(i = 1; i < n; i++)
    {
        j = i;
        while( j > 0 && cmp( &a[j*size], &a[(j-1)*size] ) < 0)
        {
            upo_swap( &a[j*size], &a[(j-1)*size], size);
            j--;
        }
    }

}

void upo_merge_sort(void* base, size_t n, size_t size, upo_sort_comparator_t cmp)
{
	upo_merge_sort_rec((char*) base, 0, n-1, size, cmp);
}

static void upo_merge_sort_rec(char* a, size_t lo, size_t hi, size_t size, upo_sort_comparator_t cmp)
{
	/* Variabili */
	size_t mid = 0;

	/* Codice pag 16 Lezione 06 - Sort */
	if(lo >= hi)
		return;

	mid = (lo+hi)/2;

	upo_merge_sort_rec(a, lo, mid, size, cmp);
	upo_merge_sort_rec(a, mid+1, hi, size, cmp);
	upo_merge(a, lo, mid, hi, size, cmp);
}

static void upo_merge(char* a, size_t lo, size_t mid, size_t hi, size_t size, upo_sort_comparator_t cmp)
{
	/* Variabili */
	hi -= lo+1;
	mid -= lo+1;

	size_t i = 0;
	size_t j = mid;
	size_t k = 0;
	char* aux = (char*)malloc((hi)*sizeof(char));

	/* Codice pag 16 Lezione 06 - Sort */
	memcpy(aux, &a[lo], size*(hi));

	for(k=lo; i<mid && j<hi; k++) {
		if(cmp(&aux[i*size], &aux[j*size]) < 0) {
			memcpy(&a[k*size], &aux[i*size], size);
			i++;
		}
		else {
			memcpy(&a[k*size], &aux[j*size], size);
			j++;
		}
	}

	while(i<mid) {
		memcpy(&a[k*size], &aux[i*size], size);
		i++;
		k++;
	}
	while(j<hi) {
		memcpy(&a[k*size], &aux[j*size], size);
		j++;
		k++;
	}

	free(aux);
}

void upo_quick_sort(void* base, size_t n, size_t size, upo_sort_comparator_t cmp)
{
    upo_quick_sort_rec((char*) base, 0, n-1, size, cmp);
}

static void upo_quick_sort_rec(char* a, size_t lo, size_t hi, size_t size, upo_sort_comparator_t cmp)
{
	size_t j;
    if( lo >= hi )
        return;

    j = upo_partition(a, lo, hi, size, cmp);

    if( j > 0 )
        upo_quick_sort_rec(a, lo, j-1, size, cmp);
    upo_quick_sort_rec(a, j+1, hi, size, cmp);

}

static size_t upo_partition(char* a, size_t lo, size_t hi, size_t size, upo_sort_comparator_t cmp)
{
    size_t p = lo;
    size_t i = lo;
    size_t j = hi+1;

    while(1)
    {

        do{ i++; }while(!( i >= hi || cmp(&a[i*size], &a[p*size]) >= 0 ));
        do{ j--; }while(!( j <= lo || cmp(&a[j*size], &a[p*size]) <= 0 ));
        if( i >= j )
            break;
        upo_swap(&a[i*size], &a[j*size], size);
    }

    upo_swap(&a[p*size], &a[j*size], size);
    return j;
}
