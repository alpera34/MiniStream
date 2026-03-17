#include <stdio.h>
#include "bellek_izci.h"

static BellekIzci izci = {0};

void* izlenen_malloc(size_t boyut) {
    void* ptr = malloc(boyut);
    if (ptr) {
        izci.toplam_ayrildi += boyut;
        izci.malloc_sayisi++;
    }
    return ptr;
}

void izlened_free(void* ptr, size_t boyut) {
    if (ptr) {
        free(ptr);
        izci.toplam_serbest += boyut;
        izci.free_sayisi++;
    }
}

void izci_realloc_guncelle(size_t eski_boyut, size_t yeni_boyut) {
    /* realloc eski bloğu serbest bırakıp yeni blok açtı —
       izleyiciye bunu bildiriyoruz */
    izci.toplam_serbest += eski_boyut;
    izci.free_sayisi++;
    izci.toplam_ayrildi += yeni_boyut;
    izci.malloc_sayisi++;
}

int izci_malloc_sayisi(void)  { return izci.malloc_sayisi; }
int izci_free_sayisi(void)    { return izci.free_sayisi; }

size_t izci_aktif_byte(void) {
    if (izci.toplam_ayrildi < izci.toplam_serbest) return 0;
    return izci.toplam_ayrildi - izci.toplam_serbest;
}

size_t izci_sizinti(void) {
    if (izci.toplam_ayrildi < izci.toplam_serbest) return 0;
    return izci.toplam_ayrildi - izci.toplam_serbest;
}

void bellek_raporu_yazdir(void) {
    size_t aktif = izci_aktif_byte();
    printf("=== BELLEK RAPORU ===\n");
    printf("malloc : %4d kez,  %8zu byte\n",
           izci.malloc_sayisi, izci.toplam_ayrildi);
    printf("free   : %4d kez,  %8zu byte\n",
           izci.free_sayisi,   izci.toplam_serbest);
    printf("aktif  :            %8zu byte (%.2f MB)\n",
           aktif, aktif / 1048576.0);
    if (aktif > 0)
        printf("SIZINTI: %zu byte ⚠\n", aktif);
    else
        printf("SIZINTI: 0 byte ✓\n");
    printf("====================\n");
}
