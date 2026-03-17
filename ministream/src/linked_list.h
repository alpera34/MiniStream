#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "ministream.h"

/* Linked list başına şarkı ekle */
void ll_ekle(Sarki** bas, Sarki* sarki);

/* ID'ye göre ara — O(n) */
Sarki* ll_ara(Sarki* bas, int id);

/* Tüm listeyi dolaş ve yazdır */
void ll_yazdir(Sarki* bas);

/* Düğüm sayısını döndür */
int ll_boyut(Sarki* bas);

#endif
