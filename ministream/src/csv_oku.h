#ifndef CSV_OKU_H
#define CSV_OKU_H

#include "ministream.h"
#include "hash_map.h"

int  csv_yukle(const char* dosya_yolu,
               Sarki**  ll_bas,
               HashMap* map,
               int      max_sarki);

void csv_alan_cikart(const char* satir,
                     int         sutun_idx,
                     char*       cikti,
                     int         cikti_boyut);

#endif
