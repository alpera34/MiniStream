#ifndef MINISTREAM_H
#define MINISTREAM_H

#include <stdlib.h>

/* ── Struct Tanımları ── */
typedef struct Sarki {
    int id;
    char baslik[100];
    char sanatci[100];
    char album[100];
    int sure_sn;
    int yil;
    int ref_sayisi;
    struct Sarki* sonraki;
} Sarki;

typedef struct {
    int id;
    char isim[50];
    Sarki** sarkilar;
    int sarki_sayisi;
    int kapasite;
} CalmaListesi;

typedef struct {
    int id;
    char isim[50];
    CalmaListesi** listeler;
    int liste_sayisi;
} Kullanici;

/* ── Şarkı Fonksiyonları ── */
Sarki* sarki_olustur(int id, const char* baslik, const char* sanatci,
                     const char* album, int sure_sn, int yil);
int    sarki_sil(Sarki* sarki);

/* ── Çalma Listesi Fonksiyonları ── */
CalmaListesi* liste_olustur(int id, const char* isim);
int           liste_sarki_ekle(CalmaListesi* liste, Sarki* sarki);
int           liste_sarki_cikar(CalmaListesi* liste, int idx);
void          liste_temizle(CalmaListesi* liste);

#endif
