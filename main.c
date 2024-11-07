#include <stdlib.h>
#include <stdio.h>
#include "mongoose.h"

static const char * const test_user = "test";
static const char * const test_pass = "test";

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev != MG_EV_HTTP_MSG) {
    return;
  }

  struct mg_http_message *hm = (struct mg_http_message *) ev_data;  
  struct mg_str *auth = mg_http_get_header(hm, "Authorization");
  if (auth == NULL) {
    mg_http_reply(c, 401, "", "%s\n", "Not authenticated");
    return;
  }

  if (auth->len <= 6) {
    mg_http_reply(c, 400, "", "%s\n", "Bad authentication scheme");
    return;
  }

  if (memcmp(auth->ptr, "Basic ", 6) != 0) {
    mg_http_reply(c, 400, "", "%s\n", "Bad authentication scheme");
    return;
  }

  char user[256];
  char pass[256];

  char buf[256];
  int n = mg_base64_decode(auth->ptr + 6, (int) auth->len - 6, buf);
  const char *p = (const char *) memchr(buf, ':', n > 0 ? (size_t) n : 0);
  if (p != NULL) {
    mg_snprintf(user, sizeof(user), "%.*s", (int) (p - buf), buf);
    mg_snprintf(pass, sizeof(pass), "%.*s", n - (int) (p - buf) - 1, p + 1);
  }

  printf("%s %s\n", test_user, user);
  if (memcmp(test_user, user, 4) != 0) {
    mg_http_reply(c, 401, "", "%s\n", "Not authenticated");
    return;
  }

  printf("%s %s\n", test_pass, pass);
  if (memcmp(test_pass, pass, 4) != 0) {
    mg_http_reply(c, 401, "", "%s\n", "Not authenticated");
  }

  mg_http_reply(c, 200, "", "%s\n", "It works!");
}

int main(void) {
  printf("started\n");
  struct mg_mgr mgr;
  mg_mgr_init(&mgr);
  mg_http_listen(&mgr, "http://0.0.0.0:8888", fn, &mgr);
  for (;;) {
    mg_mgr_poll(&mgr, 1000);
  }
  mg_mgr_free(&mgr);
  return EXIT_SUCCESS;
}

