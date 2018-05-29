#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

    template<
            class Key,
            class T,
            class Compare = std::less<Key>
    >
    class map {

    public:
        class iterator;
        class const_iterator;
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::map as value_type by typedef.
         */
        typedef pair<const Key, T> value_type;

    private:

        enum colorT {RED, BLACK};
        struct node {
            value_type* data;
            node* ch[2];
            node* pre;
            node* nxt;
            node* pa;
            colorT color;

            node() {
                data = nullptr;
                ch[0] = ch[1] = pre = nxt = pa = nullptr;
                color = RED;
            }

            node(value_type val, colorT color): color(color) {
                data = new value_type(val);
                ch[0] = ch[1] = pre = nxt = pa = nullptr;
            }

            node(const node &other): pre(other.pre), nxt(other.nxt), color(other.color){
                data = other.data == nullptr ? nullptr : new value_type(*other.data);
                ch[0] = other.ch[0];
                ch[1] = other.ch[1];
                pa = other.pa;
            }

            node& operator=(const node &other) {
                if(this == &other)
                    return *this;
                delete data;
                data = other.data == nullptr ? nullptr : new value_type(*other.data);
                ch[0] = other.ch[0];
                ch[1] = other.ch[1];
                pa = other.pa;
                pre = other.pre;
                nxt = other.nxt;
                color = other.color;
                return *this;
            }

            value_type* operator ->() const {
                return data;
            }

            ~node() {
                delete data;
                data = nullptr;
            }
        };

        size_t sz;
        node* root;
        node* _begin;
        node* _end;

        void copySubtree(node* &u, node* const &other, node* pa) {
            if(other == nullptr)
                return;
            u = new node(*(other->data), other->color);
            u->pa = pa;
            copySubtree(u->ch[0], other->ch[0], u);
            copySubtree(u->ch[1], other->ch[1], u);
        }

        void removeSubtree(node* &u) {
            if(u == nullptr)
                return;
            removeSubtree(u->ch[0]);
            removeSubtree(u->ch[1]);
            delete u;
            u = nullptr;
        }

        void inorder(node* const &u, node* ptr[], size_t &now) {
            if(u == nullptr)
                return;
            inorder(u->ch[0], ptr, now);
            ptr[++now] = u;
            inorder(u->ch[1], ptr, now);
        }

        /**
         * insert y into the list after x
         */
        void insertList(node* x, node* y) {
            node* z = x->nxt;
            x->nxt = y;
            y->pre = x;
            y->nxt = z;
            z->pre = y;
        }

        void removeList(node* x) {
            node* p = x->pre, *q = x->nxt;
            p->nxt = q;
            q->pre = p;
        }

    public:
        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = map.begin(); --it;
         *       or it = map.end(); ++end();
         */
        class iterator {
            friend class map::const_iterator;

        private:
            map* mp;
            node* treeNode;

        public:
            iterator() {
                mp = nullptr;
                treeNode = nullptr;
            }

            iterator(map* mp, node* treeNode): mp(mp), treeNode(treeNode) {}

            iterator(const iterator &other):mp(other.mp), treeNode(other.treeNode) {}

            /**
             * return a new iterator which pointer n-next elements
             *   even if there are not enough elements, just return the answer.
             * as well as operator-
             */
            /**
             * iter++
             */
            iterator operator++(int) {
                iterator tmp = *this;
                this->operator++();
                return tmp;
            }
            /**
             * ++iter
             */
            iterator & operator++() {
                if(treeNode == nullptr || treeNode->nxt == nullptr)
                    throw invalid_iterator();
                treeNode = treeNode->nxt;
                return *this;
            }
            /**
             * iter--
             */
            iterator operator--(int) {
                auto tmp = *this;
                this->operator--();
                return tmp;
            }
            /**
             * --iter
             */
            iterator & operator--() {
                if(treeNode == nullptr || treeNode->pre->pre == nullptr)
                    throw invalid_iterator();
                treeNode = treeNode->pre;
                return *this;
            }

            value_type & operator*() const {
                return *(treeNode->data);
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const {
                return mp == rhs.mp && treeNode == rhs.treeNode;
            }
            bool operator==(const const_iterator &rhs) const {
                return mp == rhs.mp && treeNode == rhs.treeNode;

            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return this->operator==(rhs) == 0;
            }
            bool operator!=(const const_iterator &rhs) const {
                return this->operator==(rhs) == 0;
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type* operator->() const noexcept {
                return treeNode->operator->();
            }

            map* ptr() {
                return mp;
            }

            node* nd() {
                return treeNode;
            }
        };
        class const_iterator {
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
            friend class map::iterator;

        private:
            const map* mp;
            const node* treeNode;

        public:
            const_iterator () = default;
            const_iterator(const map *mp, node* treeNode): mp(mp), treeNode(treeNode) {}
            const_iterator(const const_iterator &other): mp(other.mp), treeNode(other.treeNode) {}
            explicit const_iterator(const iterator &other):mp(other.mp), treeNode(other.treeNode) {}

            const_iterator &operator=(const iterator &other) {
                mp = other.mp;
                treeNode = other.treeNode;
                return *this;
            }

            const_iterator operator++(int) {
                auto tmp = *this;
                this->operator++();
                return tmp;
            }

            const_iterator & operator++() {
                if(treeNode == nullptr || treeNode->nxt == nullptr)
                    throw invalid_iterator();
                treeNode = treeNode->nxt;
                return *this;
            }

            const_iterator operator--(int) {
                auto tmp = *this;
                this->operator--();
                return tmp;
            }

            const_iterator & operator--() {
                if(treeNode == nullptr || treeNode->pre->pre == nullptr)
                    throw invalid_iterator();
                treeNode = treeNode->pre;
                return *this;
            }

            value_type & operator*() const {
                return *(treeNode->data);
            }

            bool operator==(const iterator &rhs) const {
                return mp == rhs.mp && treeNode == rhs.treeNode;
            }
            bool operator==(const const_iterator &rhs) const {
                return mp == rhs.mp && treeNode == rhs.treeNode;

            }

            bool operator!=(const iterator &rhs) const {
                return this->operator==(rhs) == 0;
            }
            bool operator!=(const const_iterator &rhs) const {
                return this->operator==(rhs) == 0;
            }

            value_type* operator->() const noexcept {
                return treeNode->operator->();
            }
        };

        map() {
            sz = 0;
            root = nullptr;
            _begin = new node();
            _end = new node();
            _begin->pre = _end->nxt = nullptr;
            _begin->nxt = _end;
            _end->pre = _begin;
        }

        map(const map &other): sz(other.sz) {
            root = nullptr;
            _begin = new node();
            _end = new node();
            _begin->pre = _end->nxt = nullptr;
            _begin->nxt = _end;
            _end->pre = _begin;
            copySubtree(root, other.root, nullptr);
            size_t cnt = 0;
            auto tmp = new node*[sz + 5];
            inorder(root, tmp, cnt);
            node* last = _begin;
            for(auto i = 1; i <= cnt; i++) {
                insertList(last, tmp[i]);
                last = tmp[i];
            }
            delete [] tmp;
        }

        map & operator=(const map &other) {
            if(this == &other)
                return *this;

            removeSubtree(root);
            _begin->nxt = _end;
            _end->pre = _begin;

            sz = other.sz;
            copySubtree(root, other.root, nullptr);
            size_t cnt = 0;
            auto tmp = new node*[sz + 5];
            inorder(root, tmp, cnt);
            node* last = _begin;
            for(auto i = 1; i <= cnt; i++) {
                insertList(last, tmp[i]);
                last = tmp[i];
            }
            delete [] tmp;
            return *this;
        }

        ~map() {
            clear();
            delete _begin;
            delete _end;
        }
        /**
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */
        T & at(const Key &key) {
            iterator it = find(key);
            if(it == this->end())
                throw index_out_of_bound();
            return it->second;
        }
        const T & at(const Key &key) const {
            const_iterator it = find(key);
            if(it == this->cend())
                throw index_out_of_bound();
            return it->second;
        }
        /**
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T & operator[](const Key &key) {
            return insert(pair<Key, T>(key, T())).first->second;
        }
        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T & operator[](const Key &key) const {
            return this->at(key);
        }
        /**
         * return a iterator to the beginning
         */
        iterator begin() {
            return iterator(this, _begin->nxt);
        }
        const_iterator cbegin() const {
            return const_iterator(this, _begin->nxt);
        }
        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end() {
            return iterator(this, _end);
        }
        const_iterator cend() const {
            return const_iterator(this, _end);
        }
        /**
         * checks whether the container is empty
         * return true if empty, otherwise false.
         */
        bool empty() const {
            return sz == 0;
        }
        /**
         * returns the number of elements.
         */
        size_t size() const {
            return sz;
        }
        /**
         * clears the contents
         */
        void clear() {
            sz = 0;
            removeSubtree(root);
            _begin->nxt = _end;
            _end->pre = _begin;
        }
        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        pair<iterator, bool> insert(const value_type &value) {
            Key key = value.first;
            node *u = root, *p = nullptr;
            int d = -1;
            while(u != nullptr) {
                if(!Compare()(u->data->first, key) && !Compare()(key, u->data->first))
                    return pair<iterator, bool> (iterator(this, u), false);
                if(getColor(u->ch[0]) == RED && getColor(u->ch[1]) == RED ) {
                    u->color = RED;
                    u->ch[0]->color = u->ch[1]->color = BLACK;
                    insertAdjust(u);
                }
                d = Compare()(u->data->first, key);
                p = u;
                u = u->ch[d];
            }

            u = new node(value, RED);
            if(p) {
                p->ch[d] = u;
                u->pa = p;
                if(d == 1)
                    insertList(p, u);
                else
                    insertList(p->pre, u);
                insertAdjust(u);
            }   //not empty
            else {
                root = u;
                insertList(_begin, u);
            }   //empty
            insertAdjust(u);
            root->color = BLACK;
            sz++;
            return pair<iterator, bool> (iterator(this, u), true);
        }
        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void erase(iterator pos) {
            if(pos == this->end() || pos.ptr() != this)
                throw invalid_iterator();

            sz--;
            if(sz == 0) {
                removeList(root);
                delete root;
                root = nullptr;
                return;
            }

            Key key = pos.nd()->data->first;
            node *u = root, *p = nullptr;
            int d = -1;
            while(u != nullptr) {
                removeAdjust(u, p, d, key);
                if (!Compare()(u->data->first, key) && !Compare()(key, u->data->first)) {
                    if(u->ch[0] && u->ch[1]) {  //two
                        auto tmp = u->ch[1];
                        auto p0 = u;
                        int dd = 1;
                        while(tmp->ch[0]) {
                            p0 = tmp;
                            tmp = tmp->ch[0];
                            dd = 0;
                        }
                        if(d != -1)
                            p->ch[d] = tmp, tmp->pa = p;
                        else
                            root = tmp, tmp->pa = nullptr;
                        if(p0 == u) {
                            swap(u->ch[0], tmp->ch[0]);
                            if(u->ch[0])    u->ch[0]->pa = u;
                            if(tmp->ch[0])    tmp->ch[0]->pa = tmp;
                            u->ch[1] = tmp->ch[1];
                            if(u->ch[1])    u->ch[1]->pa = u;
                            tmp->ch[1] = u, u->pa = tmp;
                        }
                        else {
                            swap(u->ch[0], tmp->ch[0]);
                            swap(u->ch[1], tmp->ch[1]);
                            if(u->ch[0])    u->ch[0]->pa = u;
                            if(u->ch[1])    u->ch[1]->pa = u;
                            if(tmp->ch[0])    tmp->ch[0]->pa = tmp;
                            if(tmp->ch[1])    tmp->ch[1]->pa = tmp;
                            p0->ch[dd] = u, u->pa = p0;
                        }
                        swap(u->color, tmp->color); p = tmp;
                        u = tmp->ch[1];
                        d = 1;
                        continue;
                    }
                    else {
                        auto tmp = u->ch[0];
                        if(u->ch[1])
                            tmp = u->ch[1];
                        if(d != -1) {
                            p->ch[d] = tmp;
                            if(tmp)
                                tmp->pa = p;
                        }
                        else {
                            root = tmp;
                            if(tmp)
                                tmp->pa = nullptr;
                        }
                        root->color = BLACK;
                        removeList(u);
                        delete u;
                        return;
                    }
                }
                d = Compare()(u->data->first, key);
                p = u;
                u = u->ch[d];
            }
            throw invalid_iterator();
        }
        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         * The default method of check the equivalence is !(a < b || b > a)
         */
        size_t count(const Key &key) const {
            auto it = find(key);
            return (size_t)(it != this->cend());
        }
        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key &key) {
            auto it = find(root, key);
            if(it == nullptr)
                return this->end();
            return iterator(this, it);
        }
        const_iterator find(const Key &key) const {
            auto it = find(root, key);
            if(it == nullptr)
                return this->cend();
            return const_iterator(this, it);
        }

    private:
        node* find(node* u, const Key &key) const{
            while(u != nullptr)
            {
                if(Compare()(u->data->first, key))
                    u = u->ch[1];
                else if(Compare()(key, u->data->first))
                    u = u->ch[0];
                else
                    return u;
            }
            return nullptr;
        }

        colorT getColor(node* const u) const {
            if(u == nullptr)
                return BLACK;
            return u->color;
        }

        void LL(node *g) {
            auto p = g->ch[0];
            if(g == root)
                root = p, p->pa = nullptr;
            else {
                int d = g->pa->ch[1] && g->pa->ch[1] == g;
                g->pa->ch[d] = p;   p->pa = g->pa;
            }
            g->ch[0] = p->ch[1];
            if(p->ch[1])    p->ch[1]->pa = g;
            p->ch[1] = g;   g->pa = p;
        }

        void RR(node *g) {
            auto p = g->ch[1];
            if(g == root)
                root = p, p->pa = nullptr;
            else {
                int d = g->pa->ch[1] && g->pa->ch[1] == g;
                g->pa->ch[d] = p;   p->pa = g->pa;
            }
            g->ch[1] = p->ch[0];
            if(p->ch[0])    p->ch[0]->pa = g;
            p->ch[0] = g;   g->pa = p;
        }

        void LR(node *g) {
            RR(g->ch[0]);
            LL(g);
        }

        void RL(node *g) {
            LL(g->ch[1]);
            RR(g);
        }

        void insertAdjust(node* u) {
            node *p = u->pa, *g = nullptr;
            if(getColor(p) == BLACK)
                return;
            if(p == root) {
                p->color = BLACK;
                return;
            }
            g = p->pa;
            int d1 = g->ch[1] == p, d2 = p->ch[1] == u;
            if(d1 == 0) {
                if(d2 == 0)
                    LL(g);
                else
                    LR(g);
            }
            else {
                if(d2 == 0)
                    RL(g);
                else
                    RR(g);
            }
            g->color = RED;
            if(d1 ^ d2)
                u->color = BLACK;
            else
                p->color = BLACK;
        }

        void removeAdjust(node* &u, node* &p, int &d, const Key &key) {
            if(u->color == RED)
                return;
            if(u == root && getColor(u->ch[0]) == BLACK && getColor(u->ch[1]) == BLACK) {
                u->color = RED;
                return;
            }
            node* t = u == root ? nullptr : p->ch[d ^ 1];
            if(getColor(u->ch[0]) == BLACK && getColor(u->ch[1]) == BLACK) {
                if(!t) {
                    p->color = BLACK;
                    u->color = RED;
                }
                else if(getColor(t->ch[0]) == BLACK && getColor(t->ch[1]) == BLACK) {
                    p->color = BLACK;
                    u->color = t->color = RED;
                }
                else if(getColor(t->ch[d]) == RED) {
                    //inside
                    if(d == 0)
                        RL(p);
                    else
                        LR(p);
                    p->color = BLACK;
                    u->color = RED;
                }
                else {
                    //outside
                    if(d == 0)
                        RR(p);
                    else
                        LL(p);
                    p->color = BLACK;
                    t->ch[d ^ 1]->color = BLACK;
                    t->color = u->color = RED;
                }
            }
            else {
                if(Compare()(u->data->first, key) || Compare()(key, u->data->first)) {  //x != key
                    d = Compare()(u->data->first, key);
                    p = u; u = u->ch[d];
                    t = p->ch[d ^ 1];
                    if(getColor(u) == RED)
                        return;
                    if(d == 0)
                        RR(p);
                    else
                        LL(p);
                    p->color = RED;
                    t->color = BLACK;
                    removeAdjust(u, p, d, key);
                }
                else {
                    if(u->ch[0] && u->ch[1]) {
                        if(getColor(u->ch[1]) == BLACK) {
                            LL(u);
                            p = u->pa;
                            d = 1;
                            p->color = BLACK;
                            u->color = RED;
                        }
                        return;
                    }
                    if(u->ch[0]) {
                        LL(u);
                        p = u->pa;
                        d = 1;
                    }
                    else {
                        RR(u);
                        p = u->pa;
                        d = 0;
                    }
                    p->color = BLACK;
                    u->color = RED;
                }
            }
        }

    };

}

#endif
