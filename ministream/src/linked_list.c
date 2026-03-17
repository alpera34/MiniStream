#include <stdio.h>
#include "linked_list.h"
#include "bellek_izci.h"

void ll_ekle(Sarki** bas, Sarki* sarki) {
    if (!bas || !sarki) return;
    /* Yeni şarkıyı listenin başına bağla */
    sarki->sonraki = *bas;
    *bas = sarki;
}

Sarki* ll_ara(Sarki* bas, int id) {
    Sarki* curr = bas;
    while (curr != NULL) {
        if (curr->id == id) return curr;
        curr = curr->sonraki;
    }
    return NULL;
}

void ll_yazdir(Sarki* bas) {
    Sarki* curr = bas;
    int i = 1;
    while (curr != NULL) {
        printf("%d. [%d] %s - %s (%d)\n",
               i++, curr->id, curr->baslik,
               curr->sanatci, curr->yil);
        curr = curr->sonraki;
    }
}

int ll_boyut(Sarki* bas) {
    int sayac = 0;
    Sarki* curr = bas;
    while (curr != NULL) {
        sayac++;
        curr = curr->sonraki;
    }
    return sayac;
}
