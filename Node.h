#ifndef TREENODE_H
#define TREENODE_H

#include "vector.h"
#include "constant.h"

namespace sjtu {

        template<class Key_Type, class Value_Type>
        class TreeNode {
        public:
        addType address;          // 文件中地址

        sjtu::vector<addType> childs;
        sjtu::vector<Key_Type> keys;
        sjtu::vector<Value_Type> vals;

        addType next;

        bool isLeaf;

        explicit TreeNode(addType _address = -1, bool _isleaf = true):
                childs(BlockSize / sizeof(addType)),
                vals(BlockSize / sizeof(Value_Type)),
                keys(BlockSize / sizeof(Key_Type))
        {
            address = _address;
            next = -1;
            isLeaf = _isleaf;
        }

        TreeNode(const TreeNode &other) noexcept:
                childs(BlockSize / sizeof(addType)),
                vals(BlockSize / sizeof(Value_Type)),
                keys(BlockSize / sizeof(Key_Type))
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
                while (pos < keys.size() && keys[pos] <= K) pos++;

                if (pos == keys.size()) return -1;
                else return pos;
            }
            else {
                int low = 0, high = keys.size() - 1;
                int mid;
                while(high - low >= 10) {
                    mid = (low + high) / 2;
                    if(keys[mid] <= K)
                        low = mid;
                    else
                        high = mid;
                }
                while (low < keys.size() && keys[low] <= K) ++low;
                if (low == keys.size()) return -1;
                else return low;
            }
        }

        /**找确切key[i] = K */
        int search_exact(const Key_Type &K) {
            if(keys.size() < 10) {
                int pos = 0;
                while (pos < keys.size() && keys[pos] != K) ++pos;

                if (pos == keys.size()) return -1;
                else return pos;
            }
            else {
                int low = 0, high = keys.size() - 1;
                int mid;
                while(high - low >= 10) {
                    mid = (low + high) / 2;
                    if(keys[mid] == K)
                        return mid;
                    else if(keys[mid] < K)
                        low = mid;
                    else
                        high = mid;
                }
                while (low < keys.size() && keys[low] != K) ++low;

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
                while (i < childs.size() && childs[i] != child) ++i;
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
                    if(childs[mid] == child)
                        return mid;
                    else if(childs[mid] < child)
                        low = mid;
                    else
                        high = mid;
                }
                while (low < childs.size() && childs[low] != child) ++low;

                if (low == childs.size()) return -1;
                else return low;
            }
        }
    };


};
#endif
