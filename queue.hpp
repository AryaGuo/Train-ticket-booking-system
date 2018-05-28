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
        T *head, *tail;
        int sz;

    public:
        queue():head(nullptr), tail(nullptr), sz(0) {}

        void push(const T &obj) {
            //TODO
        }

        void pop() {
            //TODO
        }

        T front() {
            if(head)
                return *head;
            throw
        }

        bool empty() {
            return sz == 0;
        }

        int size() {
            return sz;
        }

        ~queue() {
            //TODO
        }

    };
}


#endif //TRAIN_TICKET_BOOKING_SYSTEM_QUEUE_HPP
