#ifndef LOGGER_H
#define LOGGER_H

#include <stddef.h>
#include <string.h>
#include <cstdio>
#include <pthread.h>

#define CELL_SIZE 1024
#define CELL_CNT 4

static const char* log_path = "./log.txt";

enum buffer_status{
    FREE,
    FULL
};




struct cell_buffer
{
    char *buffer;
    size_t size;        // 当前大小
    size_t capacity;    // 总容量

    buffer_status status;

    cell_buffer* prev;
    cell_buffer* next;

    cell_buffer(size_t capacity): size(0), capacity(capacity), status(FREE), prev(nullptr), next(nullptr){
        buffer = new char[capacity];
    }
    bool append(const char* data, size_t len){
        size_t remain = capacity - size;
        if(remain >= len){
            memcpy(buffer + size, data, len);
            size += len;
            return true;
        }
        else{
            return false;
        }
    }

    void write(FILE* fp){
        size_t write_len = fwrite(buffer, sizeof(char), size, fp);
        if(write_len != size){
            fprintf(stderr, "write log to disk error, wt_len %u\n", write_len);
        }
    }

    void clear(){
        size = 0;
        status = FREE;
    }

    inline bool is_full() const{
        return size == capacity;
    }
    inline bool is_empty() const{
        return size == 0;
    }

    cell_buffer(const cell_buffer&) = delete;
    cell_buffer& operator=(const cell_buffer&) = delete;
};





class ring_logger
{
public:
    static ring_logger& get_instance(){
        static ring_logger instance;
        return instance;
    }

    void init_path(const char *path);

    bool push(const char* data, size_t len);
    bool pop(char* data, size_t len);

    bool consume();

private:
    cell_buffer* produce_ptr;
    cell_buffer* consume_ptr;
    size_t _cell_cnt;

    FILE* fp;

    static pthread_mutex_t _mutex;
    
    ring_logger();
    ~ring_logger();

    ring_logger(const ring_logger&);
    ring_logger& operator=(const ring_logger&);
};

ring_logger::ring_logger(): 
    _cell_cnt(CELL_CNT), 
    fp(nullptr), 
    produce_ptr(nullptr), 
    consume_ptr(nullptr)
{
    produce_ptr = consume_ptr = new cell_buffer(CELL_SIZE);
    cell_buffer* tmp_ptr = produce_ptr;
    for(int i = 0; i < CELL_CNT; i++){
        cell_buffer* new_cell = new cell_buffer(CELL_SIZE);
        tmp_ptr->next = new_cell;
        new_cell->prev = tmp_ptr;
        tmp_ptr = new_cell;
    }
    tmp_ptr->next = produce_ptr;
}

ring_logger::~ring_logger(){
    cell_buffer* cur = produce_ptr;
    cell_buffer* next = nullptr;
    do{
        next = cur->next;
        delete cur;
        cur = next;
    }while(cur != produce_ptr);
}

void ring_logger::init_path(const char *path){
    if(fp != nullptr){
        fclose(fp);
    }
    fp = fopen(path, "a");
    if(fp == nullptr){
        fprintf(stderr, "open log file error\n");
    }
}

bool ring_logger::push(const char* data, size_t len){
    pthread_mutex_lock(&_mutex);

    if(produce_ptr->is_full()){
        if(produce_ptr->next == consume_ptr){
            cell_buffer* new_cell = new cell_buffer(CELL_SIZE);
            new_cell->prev = produce_ptr;
            produce_ptr->next = new_cell;
            new_cell->next = consume_ptr;
            consume_ptr->prev = new_cell;
            
        }
        else
        produce_ptr = produce_ptr->next;
    }

    consume_ptr->append(data, len);
    return true;
}

bool ring_logger::consume(){
    if(fp == nullptr){
        init_path(log_path);
    }

    return false;
}







#endif //LOGGER_H