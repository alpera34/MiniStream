#ifndef BELLEK_IZCI_H
#define BELLEK_IZCI_H

#include <stdlib.h>

typedef struct {
    size_t toplam_ayrildi;
    size_t toplam_serbest;
    int    malloc_sayisi;
    int    free_sayisi;
} BellekIzci;

void*  izlenen_malloc(size_t boyut);
void   izlened_free(void* ptr, size_t boyut);

/* realloc sonrası izleyiciyi güncelle */
void   izci_realloc_guncelle(size_t eski_boyut, size_t yeni_boyut);

int    izci_malloc_sayisi(void);
int    izci_free_sayisi(void);
size_t izci_aktif_byte(void);
size_t izci_sizinti(void);

void   bellek_raporu_yazdir(void);

#endif
