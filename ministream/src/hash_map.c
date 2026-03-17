#include <stdio.h>
#include "hash_map.h"
#include "bellek_izci.h"
#include "linked_list.h"

/* Hash fonksiyonu */
static int hash_fonk(int id) {
    return id % TABLO_BOYUTU;
}

HashMap* hashmap_olustur(void) {
    HashMap* map = (HashMap*)izlenen_malloc(sizeof(HashMap));
    if (!map) return NULL;

    for (int i = 0; i < TABLO_BOYUTU; i++)
        map->kovalar[i] = NULL;

    return map;
}

void hashmap_temizle(HashMap* map) {
    if (!map) return;
    /*
     * Kovalar yalnızca pointer tutar — Sarki nesneleri
     * başka yerde (ll veya sarki_olustur) yönetilir.
     * Sadece HashMap struct'ını serbest bırakıyoruz.
     */
    izlened_free(map, sizeof(HashMap));
}

void hashmap_ekle(HashMap* map, Sarki* sarki) {
    if (!map || !sarki) return;
    int idx = hash_fonk(sarki->id);
    /* Kovanın linked list'ine ekle */
    ll_ekle(&map->kovalar[idx], sarki);
}

Sarki* hashmap_ara(HashMap* map, int id) {
    if (!map) return NULL;
    int idx = hash_fonk(id);
    return ll_ara(map->kovalar[idx], id);
}
