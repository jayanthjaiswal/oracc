#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <xmlrpc-c/base.h>
#include "oraccnet.h"

static char *
status_file(struct call_info *cip)
{
  char *fname = malloc(strlen(varoracc) + strlen(cip->session) + strlen("status") + 3);
  sprintf(fname, "%s/%s/status", varoracc, cip->session);
  return fname;
}

/* return xmlrpc status struct on success, xmlrpc error struct on failure */
xmlrpc_value *
status_get(xmlrpc_env * envP, struct call_info *cip)
{
  char *fname = NULL, *buf = NULL;
  int res = -1;
  struct stat st;

  if (!cip)
    return request_error(envP, "oracc-xmlrpc: status_get: NULL call_info", NULL);
  if (!cip->session)
    return request_error(envP, "oracc-xmlrpc: status_get: no session set in call_info", NULL);
  fname = status_file(cip);
  if ((res = stat(fname, &st)) < 0)
    return request_error(envP, "oracc-xmlrpc: unable to stat status for session %s\n%s", cip->session, strerror(errno), NULL);
  buf = malloc((size_t)st.st_size);
  if ((res = open(fname, O_RDONLY)) < 0)
    return request_error(envP, "oracc-xmlrpc: unable to read status for session %s\n%s", cip->session, strerror(errno), NULL);
  if ((res = read(res, &buf, st.st_size)) < 0)
    return request_error(envP, "oracc-xmlrpc: read of %d bytes failed for session %s\n%s", (int)st.st_size, cip->session, strerror(errno), NULL);
  return request_status(envP, buf);
}

/* return NULL on success, xmlrpc error struct on failure */
xmlrpc_value *
status_set(xmlrpc_env *envP, struct call_info *cip, const char *s)
{
  char *fname = NULL;
  int res = -1;
  if (!cip)
    return request_error(envP, "oracc-xmlrpc: status_get: NULL call_info", NULL);
  if (!cip->session)
    return request_error(envP, "oracc-xmlrpc: status_get: no session set in call_info", NULL);
  fname = status_file(cip);
  fprintf(stderr, "status_set: attempting to set status to %s in %s\n", s, fname);
  if ((res = open(fname, O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR)) < 0)
    return request_error(envP, "oracc-xmlrpc: unable to write status for session %s\n%s", cip->session, strerror(errno), NULL);
  if ((res = write(res, s, strlen(s))) < 0)
    return request_error(envP, "oracc-xmlrpc: write of %d bytes failed for session %s\n%s", strlen(s), cip->session, strerror(errno), NULL);
  return NULL;
}