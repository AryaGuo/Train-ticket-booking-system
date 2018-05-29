#ifndef VECTOR_H
#define VECTOR_H
#include <climits>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include "constant.h"

namespace sjtu {

    template <class ElemType>
    class vector {
    private:
        int vecSize = BlockSize * 2 / sizeof(ElemType);
        ElemType *vec;
        int len;
    public:
        explicit vector(const int &&size) {
            vec = (ElemType *)malloc(BlockSize * 2);
           // std::cout<<"Blocksize * 2"<<vecSize<<'\n';
            len = 0;
        }
        ~vector() {
            clear();
            free(vec);
        }
        void push_back(const ElemType &elem) {
            vec[len++] = elem;
        }
        void insert(const int &pos, const ElemType &elem) {

            for (int i = len; i > pos; i--){vec[i] = vec[i - 1];}
            vec[pos] = elem;
            ++len;
        }
        void erase(const int &pos) {
            for(int i = pos; i < len - 1; i++){vec[i] = vec[i+1];}
            --len;
            vec[len].~ElemType();
        }
        void pop_back() {
            --len;
            vec[len].~ElemType();
        }
        void clear() {
            for(int i = 0; i < len;i++)vec[i].~ElemType();

            len = 0;
        }
        short size() const {
            return (short)len;
        }
        ElemType &back() {
            return vec[len-1];
        }
        ElemType &front() {
            return vec[0];
        }
        vector<ElemType> &operator = (const vector<ElemType> &other) {
            if(this == &other) return *this;
            clear();
            len = other.len;
            for(int i = 0; i < len; ++i)
                vec[i] = other.vec[i];
            return *this;
        }
        ElemType& operator[] (const short &pos) {
            return vec[pos];
        }
        void assign(const vector<ElemType> &other, const short &left, const short &right) {
            for(int i = 0; i < len; i++)vec[i].~ElemType();

            //for(int i = right; i < len; i++){vec[i].~ElemType();}

            for(int i = left; i < right; i++) vec[i-left] = other.vec[i];

            len = right - left;
        }
        void file_read(FILE *&file, const short &item_num) {
            if(item_num < len)
            {
                for(int i = item_num; i < len; i++) vec[i].~ElemType();
            }
            len = item_num;
            fread(vec, sizeof(ElemType), (size_t) len, file);
        }
        void file_write(FILE *&file) {
            fwrite(vec, sizeof(ElemType), (size_t) len, file);
        }

        void shorten_len(short newLen) {
            if(newLen < len)
            {
                for(int i = newLen; i < len; i++)   vec[i].~ElemType();
            }
            len = newLen;
        }
    };
};

#endif
