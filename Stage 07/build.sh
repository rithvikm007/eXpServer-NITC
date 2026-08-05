gcc -g -o xps \
    main.c \
    core/xps_core.c \
    core/xps_loop.c \
    lib/vec/vec.c \
    network/xps_connection.c \
    network/xps_listener.c \
    utils/xps_logger.c \
    utils/xps_utils.c