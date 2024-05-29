#ifndef BUFFER_H
#define BUFFER_H

typedef struct Buffer
{
    char *buf;
    int length;
    int capacity;
} Buffer;

void init_buffer(Buffer buffer, int capacity);

void append_buffer(Buffer buffer, char *data, int len);

void clear_buffer(Buffer buffer);

void free_buffer(Buffer buffer);

#endif //BUFFER_H