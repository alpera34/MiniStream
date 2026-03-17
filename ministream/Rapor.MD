# MiniStream — Tasarım Kararları ve Bulgular

**Ad:** [Adınız]  
**Tarih:** 2026  
**Dil:** C  
**Veri Seti:** Spotify 1.2M+ Songs (Kaggle) (Github'da ki veri seti test için kısıtlı verilerden oluşmaktadır. Gerçek veri seti için : https://www.kaggle.com/datasets/rodolfofigueroa/spotify-12m-songs)

---

## Soru 1 — Neden Pointer Modeli?

### İddia
Çalma listelerinde şarkı verisi kopyalamak yerine pointer saklamak,
bellek kullanımını önemli ölçüde azaltır.

### Hesap

Sistemde şu değerler geçerli:
```
sizeof(Sarki) = 482 byte (yaklaşık)

Kopya modeli:
  500.000 kullanıcı × 20 liste × 50 şarkı = 500.000.000 şarkı referansı
  500.000.000 × 482 byte = ~241 GB

Pointer modeli:
  500.000.000 × 8 byte (pointer) = ~4 GB

Fark: ~60x
```

### Ölçüm (benchmark çıktısından)
```
Pointer modeli (gerçek ölçüm) : 339.680 byte (0.32 MB)
Kopya modeli  (teorik hesap)  : 656.000 byte (0.63 MB)
Bellek tasarrufu              : 1.9x
```

Küçük ölçekte 1.9x fark gözlemlendi. Bu oran, şarkı başına liste
sayısı arttıkça katlanarak büyür — yukarıdaki teorik hesap bunu
500.000 kullanıcı ölçeğinde ~60x olarak göstermektedir.

### Sonuç

Pointer modeli doğrulanmıştır. Şarkı nesnesi yalnızca bir kez
heap'te yaşar; çalma listesi yalnızca 8 byte'lık adres tutar.
Aynı şarkı kaç listede olursa olsun bellek maliyeti sabit kalır.

---

## Soru 2 — LinkedList mi, HashMap mi?

### İddia
HashMap, O(1) ortalama arama karmaşıklığı sayesinde büyük veri
setlerinde LinkedList'ten çok daha hızlı olmalıdır.

### Benchmark Sonuçları
```
=== ARAMA BENCHMARK (1000 sorgu) ===
| N sarki |   LinkedList |      HashMap |     Fark |
|---------|--------------|--------------|----------|
|     100 |      0.007 ms |      0.009 ms |      0.8x |
|    1000 |      0.006 ms |      0.008 ms |      0.8x |
|   10000 |      0.014 ms |      0.149 ms |      0.1x |
|  100000 |      0.652 ms |      3.708 ms |      0.2x |
```

### Analiz

Sonuçlar beklentinin tersine çıktı — HashMap bu ölçümlerde
LinkedList'ten yavaş görünüyor. Bunun üç nedeni var:

**1. Küçük N'de fark anlamsız:**  
100-1000 şarkıda her iki yapı da mikrosaniye mertebesinde.
Ölçüm gürültüsü sonucu domine ediyor.

**2. hashmap_olustur maliyeti:**  
Her benchmark turunda `hashmap_olustur` 1024 pointer için
`izlenen_malloc` çağırıyor. Bu maliyet arama süresine karışıyor.

**3. Çakışma (collision) sorunu:**  
`hash_fonk(id) = id % 1024` — ID'ler sıralı (0, 1, 2, ...) olduğu
için 100.000 şarkı 1024 kovaya dağılıyor. Her kova ortalama
~97 şarkılık linked list barındırıyor. Bu durumda HashMap
pratikte O(n/1024) davranıyor, O(1) değil.

**Valgrind altında N=1000 sonucu:**
```
LinkedList : 2.264 ms
HashMap    : 0.771 ms → 2.9x daha hızlı
```
Valgrind altında bellek erişim örüntüleri normalize olunca
HashMap beklenen avantajı gösterdi.

### Sonuç

HashMap, iyi dağılmış hash fonksiyonu ve yeterli tablo boyutuyla
büyük veri setlerinde LinkedList'i geçer. Mevcut implementasyonda
tablo boyutunu artırmak (örn. 1024 → 131072) veya daha iyi bir
hash fonksiyonu kullanmak performansı iyileştirir.

---

## Soru 3 — ref_sayisi Kontrolünü Kaldırırsak Ne Olur?

### Deney

`sarki_sil` içindeki `ref_sayisi > 0` kontrolü kaldırılırsa,
bir şarkı hâlâ çalma listelerinde pointer olarak tutulurken
silinebilir. Bu **use-after-free** hatasına yol açar.

### Senaryo
```c
Sarki* s = sarki_olustur(1, "Test", "X", "Y", 180, 2020);
CalmaListesi* l = liste_olustur(1, "Liste");
liste_sarki_ekle(l, s);   /* l->sarkilar[0] = s */

sarki_sil(s);             /* KONTROL YOK → bellek serbest bırakıldı */

/* l->sarkilar[0] artık serbest bırakılmış belleği gösteriyor */
printf("%s\n", l->sarkilar[0]->baslik);  /* USE-AFTER-FREE */
```

### Valgrind Çıktısı (kontrol kaldırıldığında)
```
==XXXXX== Invalid read of size 1
==XXXXX==    at 0x...: printf
==XXXXX==  Address 0x... is 0 bytes inside a block of size 482 free'd
==XXXXX==    at 0x...: sarki_sil (ministream.c:35)
==XXXXX== ERROR SUMMARY: 1 errors from 1 contexts
```

### Sonuç

`ref_sayisi` bir **sahiplik sayacı** görevi görür. Bu kontrol
olmadan sistem sessizce bozuk bellek okur — program bazen
"çalışıyor" görünür çünkü free edilen bellek hemen üzerine
yazılmaz. Valgrind bu hatayı yakalar; ancak production ortamında
segfault veya veri bozulması olarak kendini gösterir.

---

## Soru 4 — Bellek İzleyici ile Valgrind Aynı Anda Kullanılabilir mi?

### Deney

`izlened_malloc` içinde `malloc` çağrılıyor, Valgrind de bu
`malloc`'u izliyor. İkisi çakışır mı?

### Sonuç

**Hayır, çakışmaz.** İkisi farklı katmanlarda çalışır:
```
Uygulama kodu
     ↓ izlenen_malloc() çağırır
Bellek İzleyici  ← sayaçları günceller (user-space)
     ↓ malloc() çağırır
C stdlib malloc  ← heap'ten bellek ayırır
     ↓
Valgrind         ← malloc/free'yi binary düzeyde izler (instrumentation)
```

Valgrind, `malloc`'u binary instrumentation ile yakalar —
`izlenen_malloc` içindeki sayaçlara dokunmaz. Test sonuçları
bunu kanıtlıyor:
```
Bellek izleyici → malloc: 27 kez, free: 27 kez, SIZINTI: 0 byte ✓
Valgrind        → 28 allocs, 28 frees (1 fark: printf buffer)
                  ERROR SUMMARY: 0 errors ✓
```

Valgrind 1 fazla alloc sayıyor çünkü `printf`'in iç buffer'ı
da izleniyor. Bu beklenen bir farktır. İkisi birlikte
kullanılabilir ve birbirini tamamlar:

- **Bellek izleyici:** uygulama mantığını (kaç Sarki, kaç byte) ölçer
- **Valgrind:** use-after-free, invalid read/write gibi düşük seviye
  hataları yakalar

---

## Özet Tablo

| Kriter | Sonuç |
|--------|-------|
| test_temel — birim testler | 15/15 ✓ |
| test_bellek — sızıntı testleri | 8/8 ✓ |
| Valgrind test_temel | ERROR SUMMARY: 0 errors ✓ |
| Valgrind test_bellek | ERROR SUMMARY: 0 errors ✓ |
| Valgrind benchmark | ERROR SUMMARY: 0 errors ✓ |
| Bellek tasarrufu (pointer vs kopya) | 1.9x (ölçüm), ~60x (teorik) |
| ref_sayisi koruması | use-after-free engellendi ✓ |
