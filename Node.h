#ifndef TREENODE_H
#define TREENODE_H

#include "vector.h"
#include "constant.h"

namespace sjtu {

        template<class Key_Type,
                class Value_Type,
                class Compare = std::less<Key_Type>,
                class Compare_child = std::less<addType>
             >
        class TreeNode {
        public:
        addType address;          // 文件中地址

        sjtu::vector<addType> childs;
        sjtu::vector<Key_Type> keys;
        sjtu::vector<Value_Type> vals;

        addType next;

        bool isLeaf;


        bool Cmp(const Key_Type &x, const Key_Type &y) const{       ///<
            static Compare _cmp;
            return _cmp(x, y);
        }
        bool Equal(const Key_Type &x, const Key_Type &y) const{     /// ==
            if ((Cmp(x, y) || Cmp(y, x))) return 0;
            else return 1;
        }


        bool Cmp_child(const addType &x, const addType &y) const{       ///<
            static Compare_child _cmp;
            return _cmp(x, y);
        }
        bool Equal_child(const addType &x, const addType &y) const{     /// ==
            if ((Cmp(x, y) || Cmp(y, x))) return 0;
            else return 1;
        }



        explicit TreeNode(addType _address = -1, bool _isleaf = true):
        childs(),vals(),keys()
        {
            address = _address;
            next = -1;
            isLeaf = _isleaf;
        }

        TreeNode(const TreeNode &other) noexcept:
                childs(),
                vals(),
                keys()
        {
            address = other.address;
            next = other.next;
            isLeaf = other.isLeaf;
            keys = other.keys;
            if (isLeaf)
                vals = other.vals;
            else
                childs = other.childs;
        }

        TreeNode &operator= (const TreeNode &other) {
            address = other.address;
            childs = other.childs;
            keys = other.keys;
            vals = other.vals;
            next = other.next;
            isLeaf = other.isLeaf;
            return *this;
        }

        /**找最小的i使得 key[i] > K，没有则返回-1;*/
        int search_sup(Key_Type K) {
            int pos = 0;
            if(keys.size() < 10) {
                while (pos < keys.size() && !Cmp(K,keys[pos])) pos++;

                if (pos == keys.size()) return -1;
                else return pos;
            }
            else {
                int low = 0, high = keys.size() - 1;
                int mid;
                while(high - low >= 10) {
                    mid = (low + high) / 2;
                    if(!Cmp(K,keys[mid]))
                        low = mid;
                    else
                        high = mid;
                }
                while (low < keys.size() && !Cmp(K,keys[low])) ++low;
                if (low == keys.size()) return -1;
                else return low;
            }
        }

        /**找确切key[i] = K */
        int search_exact(const Key_Type &K) {
            if(keys.size() < 10) {
                int pos = 0;
                while (pos < keys.size() && !Equal(K,keys[pos]) ) ++pos;

                if (pos == keys.size()) return -1;
                else return pos;
            }
            else {
                int low = 0, high = keys.size() - 1;
                int mid;
                while(high - low >= 10) {
                    mid = (low + high) / 2;
                    if( Equal(K,keys[mid]) )
                        return mid;
                    else if( Cmp(keys[mid] , K))
                        low = mid;
                    else
                        high = mid;
                }
                while (low < keys.size() && !Equal(K,keys[low]) ) ++low;

                if (low == keys.size()) return -1;
                else return low;
            }
        }

        /**   清空node      */
        void clear_content() {
            childs.clear();
            keys.clear();
            vals.clear();
        }

        void clear() {
            address = next = -1;
            isLeaf = true;

            childs.clear();
            keys.clear();
            vals.clear();
        }

        /**   找块中孩子，失败返回-1      */
        int search_child(addType child) {
            if(childs.size() < 10) {
                int i = 0;
                while (i < childs.size() && !Equal_child(childs[i],child)) ++i;
                if (i == childs.size()) {
                    return -1;
                }
                else return i;
            }
            else {
                int low = 0, high = childs.size() - 1;
                int mid;
                while(high - low >= 10) {
                    mid = (low + high) / 2;
                    if(Equal_child(childs[mid],child))
                        return mid;
                    else if(Cmp_child(childs[mid],child))
                        low = mid;
                    else
                        high = mid;
                }
                while (low < childs.size() && !Equal_child(childs[low],child)) ++low;

                if (low == childs.size()) return -1;
                else return low;
            }
        }
    };


};
#endif
