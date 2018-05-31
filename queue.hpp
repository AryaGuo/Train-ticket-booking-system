//
// Created by Arya on 2018/5/28.
//

#ifndef TRAIN_TICKET_BOOKING_SYSTEM_QUEUE_HPP
#define TRAIN_TICKET_BOOKING_SYSTEM_QUEUE_HPP

#include "exceptions.hpp"

namespace sjtu {

    template <class T>
    class queue {
    private:
        struct node {
            T *data;
            node *next;
            node(): data(nullptr), next(nullptr){}
            node(const T &val): next(nullptr){
                data = new T(val);
            }
            node(const node &other): next(other.next) {
                data = new T(*other.data);
            }

            node& operator=(const node &other) {
                if(this == &other)
                    return *this;
                delete data;
                data = new T(*other.data);
                next = other.next;
            }

            ~node() {
                delete data;
            }
        };
        node *head, *tail;
        int sz;

    public:
        queue():head(nullptr), tail(nullptr), sz(0) {}

        void push(const T &obj) {
            sz++;
            auto *tmp = new node(obj);
            if(sz == 0) {
                head = tail = tmp;
            }
            else {
                tail->next = tmp;
                tail = tmp;
            }
        }

        void pop() {
            sz--;
            auto del = head->data;
            delete del;
            if(sz == 0) {
                head = tail = nullptr;
            }
            else {
                head = head->next;
            }
        }

        T front() {
            if(head)
                return *head->data;
            throw invalid_operation();
        }

        bool empty() {
            return sz == 0;
        }

        int size() {
            return sz;
        }

        ~queue() {
            while(head != tail) {
                auto del = head->data;
                delete del;
                head = head->next;
            }
            delete head->data;
        }

    };
}


#endif //TRAIN_TICKET_BOOKING_SYSTEM_QUEUE_HPP
