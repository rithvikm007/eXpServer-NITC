#include "xps_utils.h"

bool is_valid_port(u_int port) { return port >= 0 && port <= 65535; }

struct addrinfo *xps_getaddrinfo(const char *host, u_int port) {
    assert(host != NULL);
    assert(is_valid_port(port));

    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    char port_str[20];
    sprintf(port_str, "%u", port);

    int err = getaddrinfo(host, port_str, &hints, &result);
    if (err != 0) {
        logger(LOG_ERROR, "xps_getaddrinfo()", "getaddrinfo() error");
        return NULL;
    }

    char ip_str[30];
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)result->ai_addr;
    if (inet_ntop(result->ai_family, &(ipv4->sin_addr), ip_str, sizeof(ip_str)) == NULL) {
        logger(LOG_ERROR, "xps_getaddrinfo()", "inet_ntop() failed");
        perror("Error message");
        freeaddrinfo(result);
        return NULL;
    }

    logger(LOG_DEBUG, "xps_getaddrinfo()", "host: %s, port: %u, resolved ip: %s", host, port, ip_str);

    return result;
}

int make_socket_non_blocking(u_int sock_fd) {
    // Get the current socket flags
    int flags = fcntl(sock_fd, F_GETFL, 0);
    if (flags < 0) {
        logger(LOG_ERROR, "make_socket_non_blocking()", "failed to get flags");
        perror("Error message");
        return E_FAIL;
    }

    // Set flags with O_NONBLOCK
    if (fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        logger(LOG_ERROR, "make_socket_non_blocking()", "failed to set flags");
        perror("Error message");
        return E_FAIL;
    }

    return OK;
}

char *get_remote_ip(u_int sock_fd) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    char ipstr[INET_ADDRSTRLEN];

    if (getpeername(sock_fd, (struct sockaddr *)&addr, &addr_len) != 0) {
        logger(LOG_ERROR, "get_remote_ip()", "getpeername() failed");
        perror("Error message");
        return NULL;
    }

    char *ip_str = malloc(INET_ADDRSTRLEN);
    if (ip_str == NULL) {
        logger(LOG_ERROR, "get_remote_ip()", "malloc() failed for 'ip_str");
        return NULL;
    }

    inet_ntop(AF_INET, &addr.sin_addr, ip_str, INET_ADDRSTRLEN);

    return ip_str;
}

void vec_filter_null(vec_void_t *v) {
  assert(v != NULL);

  vec_void_t temp;
  vec_init(&temp);

  for (int i = 0; i < v->length; i++) {
    void *curr = v->data[i];
    if (curr != NULL)
      vec_push(&temp, curr);
  }

  vec_clear(v);
  for (int i = 0; i < temp.length; i++)
    vec_push(v, temp.data[i]);

  vec_deinit(&temp);
}

const char *get_file_ext(const char *file_path) {
  // Find the last occurrence of dot
  const char *dot = strrchr(file_path, '.');

  // Check if dot is present and it is not the first character
  return dot && dot > strrchr(file_path, '/') ? dot : NULL;
}

char *str_from_ptrs(const char *start, const char *end) {
  assert(start != NULL);
  assert(end != NULL);
  assert(start <= end);

  size_t len = end - start;

  char *str = malloc(sizeof(char) * (len + 1));
  if (str == NULL) {
    logger(LOG_ERROR, "str_from_ptrs()", "malloc() failed for 'str'");
    return NULL;
  }

  memcpy(str, start, len);
  str[len] = '\0';

  return str;
}


bool str_starts_with(const char *str, const char *prefix) {
	assert(str != NULL);
	assert(prefix != NULL);

	size_t prefix_len = strlen(prefix);
	size_t str_len = strlen(str);

	if (str_len < prefix_len)
		return false;

	if (strncmp(str, prefix, prefix_len) != 0)
		return false;

	// checking boundary condition eg /api2 and /api (these should not match)
	// BUT if prefix ends with '/', it's already a clear boundary
	if (prefix_len > 0 && prefix[prefix_len - 1] == '/') {
		return true;
	}

	char next = str[prefix_len];
	return (next == '\0' || next == '/');
}

char *path_join(const char *path_1, const char *path_2) {
	assert(path_1 != NULL);
	assert(path_2 != NULL);

	size_t joined_path_size = strlen(path_1) + strlen(path_2) + 4;
	char *joined_path = malloc(joined_path_size);
	snprintf(joined_path, joined_path_size, "%s/%s", path_1, path_2);

	return joined_path;
}

char *str_create(const char *str) {
	assert(str != NULL);

	char *new_str = malloc(strlen(str) + 1);
	if (new_str == NULL) {
		logger(LOG_ERROR, "str_create()", "malloc() failed for 'new_str'");
		return NULL;
	}
	strcpy(new_str, str);

	return new_str;
}

bool is_dir(const char *path) {
	assert(path != NULL);

	struct stat path_stat;
	if (stat(path, &path_stat) != 0)
		return false;

	return S_ISDIR(path_stat.st_mode);
}

bool is_file(const char *path) {
	assert(path != NULL);

	struct stat path_stat;
	if (stat(path, &path_stat) != 0)
		return false;

	return S_ISREG(path_stat.st_mode);
}

bool is_abs_path(const char *path) {
	assert(path != NULL);
	return path[0] == '/';
}