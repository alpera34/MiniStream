#include <stdio.h>
#include <string.h>
#include "ministream.h"
#include "bellek_izci.h"

#define BASLANGIC_KAPASITE 4

/* ── Şarkı ── */
Sarki* sarki_olustur(int id, const char* baslik, const char* sanatci,
                     const char* album, int sure_sn, int yil) {
    Sarki* s = (Sarki*)izlenen_malloc(sizeof(Sarki));
    if (!s) return NULL;

    s->id         = id;
    s->sure_sn    = sure_sn;
    s->yil        = yil;
    s->ref_sayisi = 0;
    s->sonraki    = NULL;

    strncpy(s->baslik,  baslik,  sizeof(s->baslik)  - 1);
    strncpy(s->sanatci, sanatci, sizeof(s->sanatci) - 1);
    strncpy(s->album,   album,   sizeof(s->album)   - 1);

    s->baslik[sizeof(s->baslik)   - 1] = '\0';
    s->sanatci[sizeof(s->sanatci) - 1] = '\0';
    s->album[sizeof(s->album)     - 1] = '\0';

    return s;
}

int sarki_sil(Sarki* sarki) {
    if (!sarki) return -1;
    if (sarki->ref_sayisi > 0) {
        printf("UYARI: '%s' hala %d listede kullaniliyor!\n",
               sarki->baslik, sarki->ref_sayisi);
        return -1;
    }
    izlened_free(sarki, sizeof(Sarki));
    return 0;
}

/* ── Çalma Listesi ── */
CalmaListesi* liste_olustur(int id, const char* isim) {
    CalmaListesi* l = (CalmaListesi*)izlenen_malloc(sizeof(CalmaListesi));
    if (!l) return NULL;

    l->id           = id;
    l->sarki_sayisi = 0;
    l->kapasite     = BASLANGIC_KAPASITE;

    strncpy(l->isim, isim, sizeof(l->isim) - 1);
    l->isim[sizeof(l->isim) - 1] = '\0';

    l->sarkilar = (Sarki**)izlenen_malloc(sizeof(Sarki*) * l->kapasite);
    if (!l->sarkilar) {
        izlened_free(l, sizeof(CalmaListesi));
        return NULL;
    }
    return l;
}

int liste_sarki_ekle(CalmaListesi* liste, Sarki* sarki) {
    if (!liste || !sarki) return -1;

    if (liste->sarki_sayisi >= liste->kapasite) {
        /*
         * realloc izleyici dışı — ama eski pointer dizisini
         * serbest bırakmak yerine büyütüyor. Bunu izleyiciye
         * doğru yansıtmak için: eski boyutu free, yeni boyutu malloc
         * olarak kaydet.
         */
        size_t eski_boyut = sizeof(Sarki*) * liste->kapasite;
        int    yeni_kapasite = liste->kapasite * 2;
        size_t yeni_boyut = sizeof(Sarki*) * yeni_kapasite;

        Sarki** yeni = (Sarki**)realloc(liste->sarkilar, yeni_boyut);
        if (!yeni) return -1;

        /* İzleyiciye: eski bloğu serbest bıraktık, yeni bloğu aldık */
        izci_realloc_guncelle(eski_boyut, yeni_boyut);

        liste->sarkilar = yeni;
        liste->kapasite = yeni_kapasite;
    }

    liste->sarkilar[liste->sarki_sayisi++] = sarki;
    sarki->ref_sayisi++;
    return 0;
}

int liste_sarki_cikar(CalmaListesi* liste, int idx) {
    if (!liste || idx < 0 || idx >= liste->sarki_sayisi) return -1;

    liste->sarkilar[idx]->ref_sayisi--;
    liste->sarkilar[idx] = liste->sarkilar[--liste->sarki_sayisi];
    return 0;
}

void liste_temizle(CalmaListesi* liste) {
    if (!liste) return;
    /* kapasite: izleyicinin bildiği son boyut */
    izlened_free(liste->sarkilar, sizeof(Sarki*) * liste->kapasite);
    izlened_free(liste, sizeof(CalmaListesi));
}
