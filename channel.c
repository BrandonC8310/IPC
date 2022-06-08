#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "channel.h"

void channel_init(struct receiver* recv, struct sender* sender,
                  size_t msg_sz) {
    if (recv == NULL || sender == NULL)
        return;
    recv->n_size = msg_sz;
    sender->n_size = msg_sz;
    int* channel =  malloc(sizeof(int) * 2);
    channel[0] = -1; channel[1] = -1;
    recv->channel = channel;
    sender->channel = channel;
    pipe(channel);
    pthread_mutex_t mutex;
//    = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(&mutex, NULL);
    recv->mutex = mutex;
    sender->mutex = mutex;
    int* data_avali = malloc(sizeof(int));
    *data_avali = 0;
    recv->data_avali = data_avali;
    sender->data_avali = data_avali;
    pthread_cond_t read_data_cond;
//    = malloc(sizeof(pthread_cond_t));
    pthread_cond_init(&read_data_cond, NULL);
    recv->read_data_cond = read_data_cond;
    sender->read_data_cond = read_data_cond;
}

void channel_get(struct receiver* channel, void* data) {
    if (data == NULL || channel == NULL) {
        return;
    }
    pthread_mutex_lock(&channel->mutex);

    while (channel->data_avali == 0) {
        // sleep on condition variable:
        pthread_cond_wait(&channel->read_data_cond, &channel->mutex);
    }

    // woken up, execute critical section:
    size_t n_read = read(channel->channel[0], data, channel->n_size);
    if (n_read == (size_t) -1) {
        perror("read failed");
    } else {
        (*channel->data_avali)--;
    }
    if (*channel->data_avali == 0) {
        // queue is empty
    }
    pthread_mutex_unlock(&channel->mutex);

}

void channel_send(struct sender* channel, void* data) {
    if (data == NULL || channel == NULL) {
        return;
    }
    pthread_mutex_lock(&channel->mutex);
    size_t n_write = write(channel->channel[1], data, channel->n_size);
    if (n_write == (size_t) -1) {
        printf("%d %d\n", channel->channel[0], channel->channel[1]);
        perror("write failed");
    } else {
        (*channel->data_avali)++;
    }
    pthread_cond_signal(&channel->read_data_cond);
    pthread_mutex_unlock(&channel->mutex);
}

void sender_dup(struct sender* dest, struct sender* src) {
    if (dest == NULL || src == NULL) {
        return;
    }
    dest->n_size = src->n_size;
    dest->mutex = src->mutex;
    dest->data_avali = src->data_avali;
    dest->channel = src->channel;
    dest->read_data_cond = src->read_data_cond;
}

void channel_destroy(struct receiver* recv, struct sender* sender) {
    if (recv != NULL) {
        pthread_mutex_destroy(&recv->mutex);
        pthread_cond_destroy(&recv->read_data_cond);
        free(recv->data_avali);
        free(recv->channel);
    }
    if (sender != NULL) {
        pthread_mutex_destroy(&sender->mutex);
        pthread_cond_destroy(&sender->read_data_cond);
    }
}
