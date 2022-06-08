#ifndef SRC_CHANNEL_H
#define SRC_CHANNEL_H

#include <stdlib.h>
#include <pthread.h>

struct sender {
    size_t n_size;
    int* channel;
    pthread_mutex_t mutex;
    int* data_avali;
    pthread_cond_t read_data_cond; // when buffer is not empty -- then consumer reads
};

struct receiver {
    size_t n_size;
    int* channel;
    pthread_mutex_t mutex;
    int* data_avali;
    pthread_cond_t read_data_cond;
};

void channel_init(struct receiver* recv, struct sender* sender,
                  size_t msg_sz);

void channel_get(struct receiver* channel, void* data);

void channel_send(struct sender* channel, void* data);

void sender_dup(struct sender* dest, struct sender* src);

void channel_destroy(struct receiver* recv, struct sender* sender);

#endif