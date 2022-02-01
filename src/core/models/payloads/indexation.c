// Copyright 2020 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <string.h>

#include "core/models/payloads/indexation.h"

indexation_t *indexation_new() {
  indexation_t *idx = malloc(sizeof(indexation_t));
  if (idx) {
    idx->data = byte_buf_new();
    if (idx->data) {
      idx->index = byte_buf_new();
      if (idx->index) {
        return idx;
      }
      byte_buf_free(idx->data);
      free(idx);
      return NULL;
    }
    free(idx);
    return NULL;
  }
  return NULL;
}

void indexation_free(indexation_t *idx) {
  if (idx) {
    byte_buf_free(idx->data);
    byte_buf_free(idx->index);
    free(idx);
  }
}

indexation_t *indexation_create(char const *index, byte_t data[], uint32_t data_len) {
  indexation_t *idx = NULL;

  if ((strlen(index) * 2) > MAX_INDEXCATION_INDEX_BYTES) {
    printf("[%s:%d] invalid index", __func__, __LINE__);
    return NULL;
  }

  if ((idx = indexation_new()) != NULL) {
    // add index string
    if (!byte_buf_set(idx->index, (byte_t *)index, strlen(index) + 1)) {
      printf("[%s:%d] append index failed", __func__, __LINE__);
      indexation_free(idx);
      return NULL;
    }

    // add a binary array to data
    if (!byte_buf_set(idx->data, data, data_len)) {
      printf("[%s:%d] append data failed", __func__, __LINE__);
      indexation_free(idx);
      return NULL;
    }
  }
  return idx;
}

size_t indexation_serialize_length(indexation_t *idx) {
  // payload type
  size_t len = sizeof(uint32_t);
  // index length
  len += sizeof(uint16_t);
  len += strlen((char const *)idx->index->data);
  // data length
  len += sizeof(uint32_t);
  len += idx->data->len;
  return len;
}

size_t indexation_payload_serialize(indexation_t *idx, byte_t buf[]) {
  if (!idx) {
    printf("[%s:%d] NULL parameter\n", __func__, __LINE__);
    return 0;
  }

  byte_t *offset = buf;
  // payload type, set to value 2 to denote an indexation payload.
  uint32_t idx_type = 2;
  memcpy(offset, &idx_type, sizeof(uint32_t));
  offset += sizeof(uint32_t);

  // index
  uint16_t index_len = strlen((char const *)idx->index->data);
  memcpy(offset, &index_len, sizeof(uint16_t));
  offset += sizeof(uint16_t);
  memcpy(offset, idx->index->data, index_len);
  offset += index_len;

  // data
  uint32_t data_len = (uint32_t)idx->data->len;
  memcpy(offset, &data_len, sizeof(uint32_t));
  offset += sizeof(uint32_t);
  memcpy(offset, idx->data->data, idx->data->len);
  offset += idx->data->len;
  return (offset - buf) / sizeof(byte_t);
}
