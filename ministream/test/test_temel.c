#include <stdio.h>
#include <string.h>
#include "../src/ministream.h"
#include "../src/bellek_izci.h"
#include "../src/linked_list.h"
#include "../src/hash_map.h"

/* ── Yardımcı makro ── */
#define TEST(isim) printf("\n[TEST] %s\n", isim)
#define OK(kosul, mesaj) \
    do { \
        if (kosul) printf("  ✓ %s\n", mesaj); \
        else       printf("  ✗ BASARISIZ: %s\n", mesaj); \
    } while(0)

/* ────────────────────────────────────────────
   1. Şarkı oluşturma ve silme
   ──────────────────────────────────────────── */
void test_sarki_olustur(void) {
    TEST("sarki_olustur / sarki_sil");

    Sarki* s = sarki_olustur(1, "Bohemian Rhapsody", "Queen", "A Night at the Opera", 354, 1975);
    OK(s != NULL,            "sarki_olustur NULL donmedi");
    OK(s->id == 1,           "id dogru atandi");
    OK(s->ref_sayisi == 0,   "ref_sayisi baslangicta 0");
    OK(s->sure_sn == 354,    "sure_sn dogru");
    OK(strcmp(s->baslik, "Bohemian Rhapsody") == 0, "baslik dogru kopyalandi");

    int sonuc = sarki_sil(s);
    OK(sonuc == 0, "ref_sayisi==0 iken silindi");
}

/* ────────────────────────────────────────────
   2. ref_sayisi yönetimi
   ──────────────────────────────────────────── */
void test_ref_sayisi(void) {
    TEST("ref_sayisi");

    Sarki* s = sarki_olustur(2, "Stairway to Heaven", "Led Zeppelin", "Led Zeppelin IV", 482, 1971);
    CalmaListesi* l1 = liste_olustur(1, "Klasikler");
    CalmaListesi* l2 = liste_olustur(2, "Rock");

    liste_sarki_ekle(l1, s);
    liste_sarki_ekle(l2, s);
    OK(s->ref_sayisi == 2, "2 listede ref_sayisi == 2");

    liste_sarki_cikar(l1, 0);
    OK(s->ref_sayisi == 1, "1 cikarinca ref_sayisi == 1");

    /* ref_sayisi > 0 iken silinemez */
    int sonuc = sarki_sil(s);
    OK(sonuc == -1, "ref_sayisi>0 iken silinemez");

    /* l2'yi boşalt → ref_sayisi 0 olur → silinebilir */
    liste_sarki_cikar(l2, 0);
    OK(s->ref_sayisi == 0, "l2 bosaltinca ref_sayisi == 0");

    sonuc = sarki_sil(s);
    OK(sonuc == 0, "ref_sayisi==0 iken silindi");

    liste_temizle(l1);
    liste_temizle(l2);

    OK(izci_sizinti() == 0, "SIZINTI yok");
}

/* ────────────────────────────────────────────
   3. Çalma listesi kapasite büyümesi
   ──────────────────────────────────────────── */
void test_liste_kapasite(void) {
    TEST("liste kapasite realloc");

    CalmaListesi* l = liste_olustur(10, "Buyuyen Liste");
    Sarki* sarkiler[10];
    for (int i = 0; i < 10; i++) {
        sarkiler[i] = sarki_olustur(100 + i, "Test", "Sanatci", "Album", 200, 2020);
        liste_sarki_ekle(l, sarkiler[i]);
    }
    OK(l->sarki_sayisi == 10, "10 sarki eklendi");
    OK(l->kapasite >= 10,     "kapasite yeterince buyudu");

    /* Önce tüm şarkıları listeden çıkar */
    while (l->sarki_sayisi > 0)
        liste_sarki_cikar(l, 0);

    /* Sonra şarkıları sil */
    for (int i = 0; i < 10; i++)
        sarki_sil(sarkiler[i]);

    liste_temizle(l);
    OK(izci_sizinti() == 0, "SIZINTI yok");
}

/* ────────────────────────────────────────────
   4. linkedlist
   ──────────────────────────────────────────── */

void test_linked_list(void) {
    TEST("linked_list ara / boyut");

    size_t once = izci_aktif_byte();

    Sarki* bas = NULL;
    Sarki* s1 = sarki_olustur(1, "Song A", "Artist A", "Album A", 180, 2000);
    Sarki* s2 = sarki_olustur(2, "Song B", "Artist B", "Album B", 240, 2005);
    Sarki* s3 = sarki_olustur(3, "Song C", "Artist C", "Album C", 300, 2010);

    ll_ekle(&bas, s1);
    ll_ekle(&bas, s2);
    ll_ekle(&bas, s3);

    OK(ll_boyut(bas) == 3,      "ll_boyut == 3");
    OK(ll_ara(bas, 2) == s2,    "id=2 bulundu");
    OK(ll_ara(bas, 99) == NULL, "olmayan id NULL dondu");

    sarki_sil(s1);
    sarki_sil(s2);
    sarki_sil(s3);

    OK(izci_aktif_byte() == once, "SIZINTI yok");
}

/* ────────────────────────────────────────────
   5. Hash map
   ──────────────────────────────────────────── */
void test_hash_map(void) {
    TEST("hash_map ekle / ara");

    HashMap* map = hashmap_olustur();
    OK(map != NULL, "hashmap_olustur NULL donmedi");

    Sarki* s1 = sarki_olustur(10,   "Alpha", "X", "Y", 200, 2001);
    Sarki* s2 = sarki_olustur(1034, "Beta",  "X", "Y", 210, 2002); /* 1034 % 1024 = 10, cakisma */
    Sarki* s3 = sarki_olustur(500,  "Gamma", "X", "Y", 220, 2003);

    hashmap_ekle(map, s1);
    hashmap_ekle(map, s2);
    hashmap_ekle(map, s3);

    OK(hashmap_ara(map, 10)   == s1,   "id=10 bulundu");
    OK(hashmap_ara(map, 1034) == s2,   "id=1034 bulundu (cakisma cozuldu)");
    OK(hashmap_ara(map, 500)  == s3,   "id=500 bulundu");
    OK(hashmap_ara(map, 999)  == NULL, "olmayan id NULL dondu");

    hashmap_temizle(map);
    sarki_sil(s1);
    sarki_sil(s2);
    sarki_sil(s3);
    OK(izci_sizinti() == 0, "SIZINTI yok");
}

/* ────────────────────────────────────────────
   main
   ──────────────────────────────────────────── */
int main(void) {
    printf("=== MINISTREAM BIRIM TESTLERI ===\n");

    test_sarki_olustur();
    test_ref_sayisi();
    test_liste_kapasite();
    test_linked_list();
    test_hash_map();

    printf("\n=== GENEL BELLEK RAPORU ===\n");
    bellek_raporu_yazdir();

    return 0;
}
