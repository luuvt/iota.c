#include <stdio.h>

#include "client/api/response_error.h"
#include "core/utils/allocator.h"

static char const *const key_err_code = "code";
static char const *const key_err_msg = "message";

void res_err_free(res_err_t *err) {
  if (err) {
    if (err->msg) {
      free(err->msg);
    }
    free(err);
  }
}

res_err_t *deser_error(cJSON *j_obj) {
  if (j_obj == NULL) {
    printf("[%s:%d] invalid parameter\n", __func__, __LINE__);
    return NULL;
  }

  // check if it is an error response;
  cJSON *err_obj = cJSON_GetObjectItemCaseSensitive(j_obj, key_error);
  if (err_obj == NULL) {
    // it is not exactly an error
    printf("INFO [%s:%d]: error object not found in this response\n", __func__, __LINE__);
    return NULL;
  }

  cJSON *err_code = cJSON_GetObjectItemCaseSensitive(err_obj, key_err_code);
  if (!err_code) {
    printf("[%s:%d]: error code found\n", __func__, __LINE__);
    return NULL;
  }
  if (!cJSON_IsNumber(err_code)) {
    printf("[%s:%d]: error code is not a number\n", __func__, __LINE__);
    return NULL;
  }

  cJSON *err_msg = cJSON_GetObjectItemCaseSensitive(err_obj, key_err_msg);
  if (err_msg == NULL) {
    printf("[%s:%d] error message not found\n", __func__, __LINE__);
    return NULL;
  }
  if (!cJSON_IsString(err_msg) || (err_msg->valuestring == NULL)) {
    printf("[%s:%d] error message is not a string\n", __func__, __LINE__);
    return NULL;
  }

  res_err_t *res_err = malloc(sizeof(res_err_t));
  if (res_err == NULL) {
    printf("[%s:%d] OOM\n", __func__, __LINE__);
    return NULL;
  }
  size_t len = strlen(err_msg->valuestring);
  res_err->msg = malloc(len + 1);
  if (res_err->msg == NULL) {
    free(res_err);
    printf("[%s:%d] OOM\n", __func__, __LINE__);
    return NULL;
  }
  strncpy(res_err->msg, err_msg->valuestring, len);
  res_err->msg[len] = '\0';
  res_err->code = err_code->valueint;
  return res_err;
}