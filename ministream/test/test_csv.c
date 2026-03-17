#include <stdio.h>
#include "../src/ministream.h"
#include "../src/bellek_izci.h"
#include "../src/linked_list.h"
#include "../src/hash_map.h"
#include "../src/csv_oku.h"

int main(void) {
    Sarki*   ll_bas = NULL;
    HashMap* map    = hashmap_olustur();

    /* İlk 1000 şarkıyı yükle */
    int n = csv_yukle("data/sarkilar.csv", &ll_bas, map, 1000);

    if (n <= 0) {
        printf("Sarki yuklenemedi. data/sarkilar.csv mevcut mu?\n");
        hashmap_temizle(map);
        return 1;
    }

    /* ── Birkaç şarkı yazdır ── */
    printf("\nIlk 5 sarki (linked list):\n");
    Sarki* curr = ll_bas;
    int    goster = 0;
    while (curr && goster < 5) {
        printf("  [%d] %s — %s (%d) %d sn\n",
               curr->id, curr->baslik, curr->sanatci,
               curr->yil, curr->sure_sn);
        curr = curr->sonraki;
        goster++;
    }

    /* ── Hash map araması ── */
    printf("\nHashMap arama testi (id=0, id=5, id=999):\n");
    int test_idler[] = {0, 5, 999};
    for (int i = 0; i < 3; i++) {
        Sarki* bulunan = hashmap_ara(map, test_idler[i]);
        if (bulunan)
            printf("  id=%d bulundu: %s\n", test_idler[i], bulunan->baslik);
        else
            printf("  id=%d bulunamadi\n", test_idler[i]);
    }

    /* ── Bellek raporu ── */
    printf("\n");
    bellek_raporu_yazdir();

    /* ── Temizlik ── */
    hashmap_temizle(map);

    /* Linked list'teki tüm şarkıları sil */
    curr = ll_bas;
    while (curr) {
        Sarki* sonraki = curr->sonraki;
        curr->ref_sayisi = 0;   /* CSV testinde listeye eklenmedi */
        sarki_sil(curr);
        curr = sonraki;
    }

    printf("Temizlik sonrasi:\n");
    bellek_raporu_yazdir();

    return 0;
}
