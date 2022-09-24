/*
 * ============================================================================
 *
 *        Authors:  Prashant Pandey <ppandey@cs.stonybrook.edu>
 *                  Rob Johnson <robj@vmware.com>   
 *
 * ============================================================================
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

#include "experimental/users/bradleybear/cqf/include/gqf.h"
#include "experimental/users/bradleybear/cqf/include/gqf_int.h"
#include "third_party/openssl/rand.h"

void cqf_test(uint64_t qbits, uint64_t rbits)
{
  QF qf;
  uint64_t nhashbits = qbits + rbits;
  uint64_t nslots = (1ULL << qbits);
  uint64_t nvals = 95*nslots/100;
  uint64_t key_count = 4;
  uint64_t *vals;

  /* Initialise the CQF */
  if (!qf_malloc(&qf, nslots, nhashbits, 0, QF_HASH_INVERTIBLE, 0)) {
    fprintf(stderr, "Can't allocate CQF.\n");
    abort();
  }

  qf_set_auto_resize(&qf, true);

  /* Generate random values */
  vals = (uint64_t*)malloc(nvals*sizeof(vals[0]));
  RAND_bytes((unsigned char *)vals, sizeof(*vals) * nvals);
  srand(0);
  for (uint64_t i = 0; i < nvals; i++) {
    vals[i] = (1 * vals[i]) % qf.metadata->range;
    /*vals[i] = rand() % qf.metadata->range;*/
    /*fprintf(stdout, "%lx\n", vals[i]);*/
  }

  fprintf(stderr, "Here I am 2\n");
  /* Insert keys in the CQF */
  for (uint64_t i = 0; i < nvals; i++) {
    int ret = qf_insert(&qf, vals[i], 0, key_count, 0);
    if (ret < 0) {
      fprintf(stderr, "failed insertion for key: %lx %d.\n", vals[i], 50);
      if (ret == QF_NO_SPACE)
        fprintf(stderr, "CQF is full.\n");
      else if (ret == QF_COULDNT_LOCK)
        fprintf(stderr, "TRY_ONCE_LOCK failed.\n");
      else
        fprintf(stderr, "Does not recognise return value.\n");
      abort();
    }
  }

  fprintf(stderr, "Here I am: %d\n", __LINE__);
  /* Lookup inserted keys and counts. */
  for (uint64_t i = 0; i < nvals; i++) {
    uint64_t count = qf_count_key_value(&qf, vals[i], 0, 0);
    if (count < key_count) {
      fprintf(stderr, "failed lookup after insertion for %lx %ld.\n", vals[i],
              count);
      abort();
    }
  }
  fprintf(stderr, "Here I am: %d\n", __LINE__);
#if 0
  for (uint64_t i = 0; i < nvals; i++) {
    uint64_t count = qf_count_key_value(&qf, vals[i], 0, 0);
    if (count < key_count) {
      fprintf(stderr, "failed lookup during deletion for %lx %ld.\n", vals[i],
              count);
      abort();
    }
    if (count > 0) {
      /*fprintf(stdout, "deleting: %lx\n", vals[i]);*/
      qf_delete_key_value(&qf, vals[i], 0, QF_NO_LOCK);
      /*qf_dump(&qf);*/
      uint64_t cnt = qf_count_key_value(&qf, vals[i], 0, 0);
      if (cnt > 0) {
        fprintf(stderr, "failed lookup after deletion for %lx %ld.\n", vals[i],
                cnt);
        abort();
      }
    }
  }
#endif

  fprintf(stdout, "Testing iterator and unique indexes.\n");
  /* Initialize an iterator and validate counts. */
  QFi qfi;
  qf_iterator_from_position(&qf, &qfi, 0);
  QF unique_idx;
  if (!qf_malloc(&unique_idx, qf.metadata->nslots, nhashbits, 0,
                 QF_HASH_INVERTIBLE, 0)) {
    fprintf(stderr, "Can't allocate set.\n");
    abort();
  }

  fprintf(stderr, "Here I am: %d\n", __LINE__);
  int64_t last_index = -1;
  int i = 0;
  qf_iterator_from_position(&qf, &qfi, 0);
  while(!qfi_end(&qfi)) {
    uint64_t key, value, count;
    qfi_get_key(&qfi, &key, &value, &count);
    if (count < key_count) {
      fprintf(stderr, "Failed lookup during iteration for: %lx. Returned count: %ld\n",
              key, count);
      abort();
    }
    int64_t idx = qf_get_unique_index(&qf, key, value, 0);
    if (idx == QF_DOESNT_EXIST) {
      fprintf(stderr, "Failed lookup for unique index for: %lx. index: %ld\n",
              key, idx);
      abort();
    }
    if (idx <= last_index) {
      fprintf(stderr, "Unique indexes not strictly increasing.\n");
      abort();
    }
    last_index = idx;
    if (qf_count_key_value(&unique_idx, key, 0, 0) > 0) {
      fprintf(stderr, "Failed unique index for: %lx. index: %ld\n",
              key, idx);
      abort();
    }
    qf_insert(&unique_idx, key, 0, 1, 0);
    int64_t newindex = qf_get_unique_index(&unique_idx, key, 0, 0);
    if (idx < newindex) {
      fprintf(stderr, "Index weirdness: index %dth key %ld was at %ld, is now at %ld\n",
              i, key, idx, newindex);
      //abort();
    }

    i++;
    qfi_next(&qfi);
  }

  /* remove some counts  (or keys) and validate. */
  fprintf(stdout, "Testing remove/delete_key.\n");
  for (uint64_t i = 0; i < nvals; i++) {
    uint64_t count = qf_count_key_value(&qf, vals[i], 0, 0);
    /*if (count < key_count) {*/
    /*fprintf(stderr, "failed lookup during deletion for %lx %ld.\n", vals[i],*/
    /*count);*/
    /*abort();*/
    /*}*/
    int ret = qf_delete_key_value(&qf, vals[i], 0, 0);
    count = qf_count_key_value(&qf, vals[i], 0, 0);
    if (count > 0) {
      if (ret < 0) {
        fprintf(stderr, "failed deletion for %lx %ld ret code: %d.\n",
                vals[i], count, ret);
        abort();
      }
      uint64_t new_count = qf_count_key_value(&qf, vals[i], 0, 0);
      if (new_count > 0) {
        fprintf(stderr, "delete key failed for %lx %ld new count: %ld.\n",
                vals[i], count, new_count);
        abort();
      }
    }
  }

  fprintf(stdout, "Validated the CQF.\n");
}

