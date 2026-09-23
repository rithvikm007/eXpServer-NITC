#include "xps_http_res.h"

xps_http_res_t *xps_http_res_create(xps_core_t *core, u_int code) {
    assert(core != NULL);

    // Allocate memory for the http_res struct
    xps_http_res_t* http_res = malloc(sizeof(xps_http_res_t));
    if (!http_res){
        logger(LOG_ERROR, "xps_http_res_create()", "failed to alloc memory for http_res. malloc() returned NULL");
        return NULL;
    }
    memset(http_res, 0, sizeof(xps_http_res_t));
    vec_init(&http_res->headers);
    http_res->body = NULL;

    const char *status_text;

    // Set the response line based on the provided status code
    switch (code) {
        case HTTP_OK: status_text = "OK"; break;
        case HTTP_CREATED: status_text = "Created"; break;

        case HTTP_MOVED_PERMANENTLY: status_text = "Moved Permanently"; break;
        case HTTP_MOVED_TEMPORARILY: status_text = "Moved Temporarily"; break;
        case HTTP_NOT_MODIFIED: status_text = "Not Modified"; break;

        case HTTP_BAD_REQUEST: status_text = "Bad Request"; break;
        case HTTP_UNAUTHORIZED: status_text = "Unauthorized"; break;
        case HTTP_FORBIDDEN: status_text = "Forbidden"; break;
        case HTTP_NOT_FOUND: status_text = "Not Found"; break;

        case HTTP_INTERNAL_SERVER_ERROR: status_text = "Internal Server Error"; break;
        case HTTP_NOT_IMPLEMENTED: status_text = "Not Implemented"; break;
        case HTTP_BAD_GATEWAY: status_text = "Bad Gateway"; break;
        case HTTP_SERVICE_UNAVAILABLE: status_text = "Service Unavailable"; break;

        default: status_text = "OK"; break;
    }

    // Format the response line
    snprintf(http_res->response_line, sizeof(http_res->response_line), "HTTP/1.1 %u %s", code, status_text);

    char time_buf[128];
    time_t now = time(NULL);
    struct tm *gmt = gmtime(&now);

    // Format the date in the required HTTP format
    strftime(time_buf, sizeof(time_buf), "%a, %d %b %Y %H:%M:%S GMT", gmt);

    // Set default headers
    xps_http_set_header(&(http_res->headers), "Date", time_buf);
    xps_http_set_header(&(http_res->headers), "Server", "eXpServer");
    xps_http_set_header(&(http_res->headers), "Access-Control-Allow-Origin", "*");
    
    return http_res;
}

void xps_http_res_destroy(xps_http_res_t *res){
    assert(res != NULL);

    // Free each header in the headers vector
    for(int i = 0;i < res->headers.length; i++){
        xps_keyval_t *header = res->headers.data[i];
        if(header){
            if (header->key) free(header->key);
            if (header->val) free(header->val);
            free(header);
        }
    }

    vec_deinit(&res->headers);
    if(res->body){
        xps_buffer_destroy(res->body);
    }
    free(res);
}

xps_buffer_t *xps_http_res_serialize(xps_http_res_t *http_res) {
    /* valid params */
    assert(http_res != NULL);

    // Serialize headers
    xps_buffer_t *headers_str = xps_http_serialize_headers(&(http_res->headers));
    if (headers_str == NULL) {
        logger(LOG_ERROR, "xps_http_res_serialize()", "failed to serialize headers");
        return NULL;
    }

    // Calculate length for final buffer
    size_t response_line_len = strlen(http_res->response_line);
    size_t headers_len = headers_str->len;
    size_t body_len = http_res->body ? http_res->body->len : 0;
    size_t final_len = response_line_len + 2 + headers_len + 2 + body_len; // +2 for CRLF after response line, +2 for CRLF after headers

    // Create instance for final buffer
    xps_buffer_t *buff = xps_buffer_create(final_len, final_len, NULL);
    if (buff == NULL) {
        logger(LOG_ERROR, "xps_http_res_serialize()", "failed to create buffer instance");
        xps_buffer_destroy(headers_str);
        return NULL;
    }

    // Copy everything
    /* copy response line */
    memcpy(buff->pos, http_res->response_line, response_line_len);
    buff->pos += response_line_len;

    *buff->pos++ = '\r';
    *buff->pos++ = '\n';

    /* copy headers */
    memcpy(buff->pos, headers_str->data, headers_len);
    buff->pos += headers_len;

    *buff->pos++ = '\r';
    *buff->pos++ = '\n';

    /* copy response body*/
    if (http_res->body) {
        memcpy(buff->pos, http_res->body->data, body_len);
        buff->pos += body_len;
    }

    xps_buffer_destroy(headers_str);
    buff->len = final_len;
    buff->pos = buff->data;
    return buff;
}

void xps_http_res_set_body(xps_http_res_t *http_res, xps_buffer_t *buff) {
    assert(http_res != NULL);
    assert(buff != NULL);

    http_res->body = buff;
}