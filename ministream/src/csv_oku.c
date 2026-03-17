#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csv_oku.h"
#include "bellek_izci.h"
#include "linked_list.h"
#include "hash_map.h"

/* ── Kaggle CSV sütun indeksleri ──────────────────────
   id=0, name=1, album=2, album_id=3, artists=4,
   artist_ids=5, track_number=6, disc_number=7,
   explicit=8, danceability=9, energy=10, key=11,
   loudness=12, mode=13, speechiness=14,
   acousticness=15, instrumentalness=16, liveness=17,
   valence=18, tempo=19, duration_ms=20, time_signature=21,
   year=22, release_date=23
   ────────────────────────────────────────────────── */
#define SUTUN_ID          0
#define SUTUN_BASLIK      1
#define SUTUN_ALBUM       2
#define SUTUN_SANATCI     4
#define SUTUN_SURE_MS    20
#define SUTUN_YIL        22

#define MAX_SATIR_UZUNLUK 1024
#define MAX_ALAN_UZUNLUK   256

/* ────────────────────────────────────────────
   Yardımcı: CSV satırından alan çıkar
   Virgülle ayrılmış, tırnak içinde virgül
   olabilir — basit state machine ile çözeriz.
   ──────────────────────────────────────────── */
void csv_alan_cikart(const char* satir,
                     int         sutun_idx,
                     char*       cikti,
                     int         cikti_boyut) {
    int  sutun    = 0;
    int  i        = 0;
    int  j        = 0;
    int  tirnak   = 0;   /* tırnak içinde miyiz? */

    cikti[0] = '\0';

    while (satir[i] != '\0' && satir[i] != '\n') {
        char c = satir[i];

        if (c == '"') {
            tirnak = !tirnak;   /* tırnak aç/kapat */
            i++;
            continue;
        }

        if (c == ',' && !tirnak) {
            if (sutun == sutun_idx) {
                /* İstediğimiz sütunu doldurduk */
                break;
            }
            sutun++;
            if (sutun == sutun_idx)
                j = 0;   /* bu sütunun başı */
            i++;
            continue;
        }

        /* İstediğimiz sütundaysak karakteri kaydet */
        if (sutun == sutun_idx && j < cikti_boyut - 1)
            cikti[j++] = c;

        i++;
    }

    cikti[j] = '\0';

    /* Baş/son boşlukları temizle */
    int bas = 0;
    while (cikti[bas] == ' ') bas++;
    if (bas > 0) memmove(cikti, cikti + bas, strlen(cikti + bas) + 1);

    int son = (int)strlen(cikti) - 1;
    while (son >= 0 && (cikti[son] == ' ' || cikti[son] == '\r'))
        cikti[son--] = '\0';
}

/* ────────────────────────────────────────────
   Ana yükleme fonksiyonu
   ──────────────────────────────────────────── */
int csv_yukle(const char* dosya_yolu,
              Sarki**  ll_bas,
              HashMap* map,
              int      max_sarki) {

    FILE* f = fopen(dosya_yolu, "r");
    if (!f) {
        fprintf(stderr, "HATA: '%s' acilamadi.\n", dosya_yolu);
        return -1;
    }

    char satir[MAX_SATIR_UZUNLUK];

    /* İlk satır başlık — atla */
    if (!fgets(satir, sizeof(satir), f)) {
        fclose(f);
        return -1;
    }

    int yuklenen = 0;

    while (yuklenen < max_sarki && fgets(satir, sizeof(satir), f)) {

        /* Boş satırı atla */
        if (satir[0] == '\n' || satir[0] == '\r') continue;

        /* ── Alanları çıkar ── */
        char baslik[100]  = {0};
        char sanatci[100] = {0};
        char album[100]   = {0};
        char sure_str[20] = {0};
        char yil_str[10]  = {0};

        csv_alan_cikart(satir, SUTUN_BASLIK,   baslik,    sizeof(baslik));
        csv_alan_cikart(satir, SUTUN_SANATCI,  sanatci,   sizeof(sanatci));
        csv_alan_cikart(satir, SUTUN_ALBUM,    album,     sizeof(album));
        csv_alan_cikart(satir, SUTUN_SURE_MS,  sure_str,  sizeof(sure_str));
        csv_alan_cikart(satir, SUTUN_YIL,      yil_str,   sizeof(yil_str));

        /* Zorunlu alan kontrolü */
        if (baslik[0] == '\0' || sanatci[0] == '\0') continue;

        int sure_ms = atoi(sure_str);
        int yil     = atoi(yil_str);
        int sure_sn = sure_ms / 1000;

        /* ── Şarkıyı oluştur ve yapılara ekle ── */
        Sarki* s = sarki_olustur(yuklenen, baslik, sanatci, album,
                                 sure_sn, yil);
        if (!s) continue;

        ll_ekle(ll_bas, s);
        hashmap_ekle(map, s);
        yuklenen++;
    }

    fclose(f);
    printf("%d sarki yuklendi: %s\n", yuklenen, dosya_yolu);
    return yuklenen;
}
