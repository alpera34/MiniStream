#include <stdio.h>
#include "../src/ministream.h"
#include "../src/bellek_izci.h"
#include "../src/linked_list.h"
#include "../src/hash_map.h"

#define OK(kosul, mesaj) \
    do { \
        if (kosul) printf("  ✓ %s\n", mesaj); \
        else       printf("  ✗ BASARISIZ: %s\n", mesaj); \
    } while(0)

/* ────────────────────────────────────────────
   1. Temel sızıntı: malloc var, free yok
   ──────────────────────────────────────────── */
void test_sarki_sizinti_yok(void) {
    printf("\n[TEST] sarki olustur -> sil -> sizinti yok\n");

    size_t once = izci_aktif_byte();

    Sarki* s = sarki_olustur(1, "Test", "Sanatci", "Album", 180, 2020);
    OK(izci_aktif_byte() > once, "malloc sonrasi aktif byte artti");

    sarki_sil(s);
    OK(izci_aktif_byte() == once, "free sonrasi aktif byte eski haline dondu");
    OK(izci_sizinti() == 0,       "SIZINTI: 0 byte");
}

/* ────────────────────────────────────────────
   2. Liste temizleme sızıntısı
   ──────────────────────────────────────────── */
void test_liste_temizle_sizinti_yok(void) {
    printf("\n[TEST] liste_olustur -> sarki ekle -> temizle -> sizinti yok\n");

    size_t once = izci_aktif_byte();

    CalmaListesi* l = liste_olustur(1, "Test Listesi");
    Sarki* s1 = sarki_olustur(10, "A", "X", "Y", 100, 2010);
    Sarki* s2 = sarki_olustur(20, "B", "X", "Y", 200, 2011);

    liste_sarki_ekle(l, s1);
    liste_sarki_ekle(l, s2);

    /* Önce şarkıları listeden çıkar, ref_sayisi 0 yap */
    liste_sarki_cikar(l, 0);
    liste_sarki_cikar(l, 0);

    sarki_sil(s1);
    sarki_sil(s2);
    liste_temizle(l);

    OK(izci_aktif_byte() == once, "tum bellek serbest birakildi");
    OK(izci_sizinti() == 0,       "SIZINTI: 0 byte");
}

/* ────────────────────────────────────────────
   3. HashMap temizleme sızıntısı
   ──────────────────────────────────────────── */
void test_hashmap_sizinti_yok(void) {
    printf("\n[TEST] hashmap olustur -> ekle -> temizle -> sizinti yok\n");

    size_t once = izci_aktif_byte();

    HashMap* map = hashmap_olustur();
    Sarki* s1 = sarki_olustur(1, "A", "X", "Y", 100, 2000);
    Sarki* s2 = sarki_olustur(2, "B", "X", "Y", 200, 2001);

    hashmap_ekle(map, s1);
    hashmap_ekle(map, s2);

    /* HashMap sadece pointer tutar — önce map'i, sonra şarkıları sil */
    hashmap_temizle(map);
    sarki_sil(s1);
    sarki_sil(s2);

    OK(izci_aktif_byte() == once, "tum bellek serbest birakildi");
    OK(izci_sizinti() == 0,       "SIZINTI: 0 byte");
}

/* ────────────────────────────────────────────
   4. Çok şarkı — toplu sızıntı kontrolü
   ──────────────────────────────────────────── */
void test_toplu_sizinti_yok(void) {
    printf("\n[TEST] 100 sarki olustur -> hepsini sil -> sizinti yok\n");

    size_t once = izci_aktif_byte();

    Sarki* sarkiler[100];
    for (int i = 0; i < 100; i++)
        sarkiler[i] = sarki_olustur(i, "Sarki", "Sanatci", "Album", 200, 2000);

    for (int i = 0; i < 100; i++)
        sarki_sil(sarkiler[i]);

    OK(izci_aktif_byte() == once, "100 sarkinin bellegi tamamen serbest birakildi");
    OK(izci_sizinti() == 0,       "SIZINTI: 0 byte");
}

/* ────────────────────────────────────────────
   main
   ──────────────────────────────────────────── */
int main(void) {
    printf("=== MINISTREAM BELLEK TESTLERI ===\n");

    test_sarki_sizinti_yok();
    test_liste_temizle_sizinti_yok();
    test_hashmap_sizinti_yok();
    test_toplu_sizinti_yok();

    printf("\n=== GENEL BELLEK RAPORU ===\n");
    bellek_raporu_yazdir();

    return 0;
}
