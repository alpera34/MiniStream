#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "ministream.h"

#define TABLO_BOYUTU 1024

/* Her kova bir linked list başıdır (chaining ile çakışma çözümü) */
typedef struct {
    Sarki* kovalar[TABLO_BOYUTU];
} HashMap;

/* Boş hash map oluştur */
HashMap* hashmap_olustur(void);

/* Hash map'i serbest bırak (Sarki nesnelerine dokunmaz) */
void hashmap_temizle(HashMap* map);

/* Şarkıyı map'e ekle */
void hashmap_ekle(HashMap* map, Sarki* sarki);

/* ID'ye göre ara — O(1) ortalama */
Sarki* hashmap_ara(HashMap* map, int id);

#endif
