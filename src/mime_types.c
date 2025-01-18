#include "mime_types.h"
#include <string.h>

const char *get_mime_type(const char *path) {
    if (strstr(path, ".html")) {
        return "text/html";
    } else if (strstr(path, ".css")) {
        return "text/css";
    } else if (strstr(path, ".js"))  {
        return "application/javascript";
    } else if (strstr(path, ".jpg") || strstr(path, ".jpeg")) {
        return "image/jpeg";
    } else if (strstr(path, ".png")) {
        return "image/png";
    }
    return "application/octet-stream";
}