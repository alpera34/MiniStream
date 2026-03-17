#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../src/ministream.h"
#include "../src/bellek_izci.h"
#include "../src/linked_list.h"
#include "../src/hash_map.h"

/* ── Zamanlayıcı yardımcısı ── */
static double ms_fark(struct timespec baslangic, struct timespec bitis) {
    return (bitis.tv_sec  - baslangic.tv_sec)  * 1000.0 +
           (bitis.tv_nsec - baslangic.tv_nsec) / 1e6;
}

/* ────────────────────────────────────────────
   Arama Benchmark: LinkedList vs HashMap
   ──────────────────────────────────────────── */
void benchmark_arama(int n) {
    /* N şarkı oluştur */
    Sarki** sarkiler = (Sarki**)malloc(sizeof(Sarki*) * n);
    Sarki*  ll_bas   = NULL;
    HashMap* map     = hashmap_olustur();

    for (int i = 0; i < n; i++) {
        sarkiler[i] = sarki_olustur(i, "Sarki", "Sanatci", "Album", 200, 2000);
        ll_ekle(&ll_bas, sarkiler[i]);
        hashmap_ekle(map, sarkiler[i]);
    }

    /* 1000 rastgele arama — LinkedList */
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int q = 0; q < 1000; q++) {
        int hedef = rand() % n;
        ll_ara(ll_bas, hedef);
    }
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double ll_ms = ms_fark(t0, t1);

    /* 1000 rastgele arama — HashMap */
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int q = 0; q < 1000; q++) {
        int hedef = rand() % n;
        hashmap_ara(map, hedef);
    }
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double hm_ms = ms_fark(t0, t1);

    double fark = (ll_ms > 0.0) ? (ll_ms / hm_ms) : 0.0;
    printf("| %7d | %10.3f ms | %10.3f ms | %8.1fx |\n",
           n, ll_ms, hm_ms, fark);

    /* Temizlik */
    hashmap_temizle(map);
    for (int i = 0; i < n; i++)
        sarki_sil(sarkiler[i]);
    free(sarkiler);
}

/* ────────────────────────────────────────────
   Bellek Benchmark: Kopya vs Pointer modeli
   ──────────────────────────────────────────── */
#define N_SARKI         1000
#define N_LISTE         20
#define SARKI_PER_LISTE 50

void benchmark_bellek(void) {
    printf("\n=== BELLEK BENCHMARK ===\n");

    size_t once = izci_aktif_byte();

    Sarki* sarkiler[N_SARKI];
    for (int i = 0; i < N_SARKI; i++)
        sarkiler[i] = sarki_olustur(i, "Sarki", "Sanatci", "Album", 200, 2000);

    CalmaListesi* listeler[N_LISTE];
    for (int i = 0; i < N_LISTE; i++) {
        listeler[i] = liste_olustur(i, "Liste");
        for (int j = 0; j < SARKI_PER_LISTE; j++)
            liste_sarki_ekle(listeler[i], sarkiler[j % N_SARKI]);
    }

    size_t pointer_byte = izci_aktif_byte() - once;
    size_t kopya_byte   = (size_t)(N_SARKI + N_LISTE * SARKI_PER_LISTE) * sizeof(Sarki);

    printf("Pointer modeli (gercek olcum) : %zu byte (%.2f MB)\n",
           pointer_byte, pointer_byte / 1048576.0);
    printf("Kopya modeli  (teorik hesap)  : %zu byte (%.2f MB)\n",
           kopya_byte, kopya_byte / 1048576.0);
    printf("Bellek tasarrufu              : %.1fx\n",
           kopya_byte > 0 ? (double)kopya_byte / pointer_byte : 0.0);

    /* Temizlik — önce listeleri boşalt, sonra şarkıları sil */
    for (int i = 0; i < N_LISTE; i++) {
        while (listeler[i]->sarki_sayisi > 0)
            liste_sarki_cikar(listeler[i], 0);
        liste_temizle(listeler[i]);
    }
    for (int i = 0; i < N_SARKI; i++)
        sarki_sil(sarkiler[i]);

   size_t son = izci_aktif_byte();
   printf("SIZINTI: %zu byte %s\n", son, son == 0 ? "✓" : "⚠");
}

/* ────────────────────────────────────────────
   main
   ──────────────────────────────────────────── */
int main(void) {
    srand(42);

    printf("=== ARAMA BENCHMARK (1000 sorgu) ===\n");
    printf("| %7s | %12s | %12s | %8s |\n",
           "N sarki", "LinkedList", "HashMap", "Fark");
    printf("|---------|--------------|--------------|----------|\n");

    int boyutlar[] = {100, 1000, 10000, 100000};
    for (int i = 0; i < 4; i++)
        benchmark_arama(boyutlar[i]);

    benchmark_bellek();

    printf("\n=== GENEL BELLEK RAPORU ===\n");
    bellek_raporu_yazdir();

    return 0;
}
