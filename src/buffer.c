#include "buffer.h"
#include <string.h>
#include <stdlib.h>

void init_buffer(Buffer buffer, int capacity){
    buffer.buf = (char *)malloc(capacity);
    buffer.length = 0;
    buffer.capacity = capacity;
}

void append_buffer(Buffer buffer, char *data, int len){
    if(buffer.length + len > buffer.capacity){
        char *newBuffer = (char *)malloc(buffer.capacity * 2);
        strncpy(newBuffer, buffer.buf, buffer.length);
        free(buffer.buf);
        buffer.buf = newBuffer;
        buffer.capacity *= 2;
    }
    for(int i = 0; i < len; i++){
        buffer.buf[buffer.length + i] = data[i];
    }
    buffer.length += len;
}

void clear_buffer(Buffer buffer){
    buffer.length = 0;
    memset(buffer.buf, 0, buffer.capacity);
}

void free_buffer(Buffer buffer){
    free(buffer.buf);
}
