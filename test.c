#include "channel.c"

int main() {
    struct receiver r;
    struct sender s;
    channel_init(&r, &s, 64);
    channel_destroy(&r, &s);
    return 0;
}