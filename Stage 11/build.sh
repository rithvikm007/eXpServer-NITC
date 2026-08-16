gcc -g -o xps \
    main.c \
    core/xps_core.c \
    core/xps_loop.c \
    core/xps_pipe.c \
    lib/vec/vec.c \
    network/xps_connection.c \
    network/xps_listener.c \
    network/xps_upstream.c \
    utils/xps_logger.c \
    utils/xps_utils.c \
    utils/xps_buffer.c