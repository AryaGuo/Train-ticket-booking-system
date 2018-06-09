#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <queue>
#include <cstring>
#include "BPlusTree.hpp"
#include "constant.h"
#include "Node.hpp"
#include "utility.hpp"


#define start 4096

/**
 *
 * isLeaf, key number, val number, child number, then vector values sequentially.
 * finally is one addType, means the next node.
 */

namespace sjtu {

    template<class Key_Type,
            class Value_Type,
            class Compare = std::less<Key_Type>,
            class Compare_child = std::less<addType>
    >
    class FileManager {
    private:
        typedef TreeNode<Key_Type, Value_Type> Node;

        char filename[100];
        FILE *file;
        bool isOpened;

        bool Cmp(const Key_Type &x, const Key_Type &y) const {       /// <
            static Compare _cmp;
            return _cmp(x, y);
        }

        bool Equal(const Key_Type &x, const Key_Type &y) const {     /// ==
            if ((Cmp(x, y) || Cmp(y, x))) return 0;
            else return 1;
        }

    public:
        addType root_off;
        addType head_off;
        addType tail_off;
        addType append_off;

    private:
        void createFile() {
            file = fopen(filename, "wb");
            root_off = head_off = tail_off = -1;
            append_off = start;
            fwrite(&root_off, sizeof(int), 1, file);
            fwrite(&head_off, sizeof(int), 1, file);
            fwrite(&tail_off, sizeof(int), 1, file);
            fwrite(&append_off, sizeof(int), 1, file);
            fclose(file);
        }

        void init() {
            file = fopen(filename, "r");
            if (file == nullptr) {
                createFile();
                file = fopen(filename, "r+b");
            } else {
                file = fopen(filename, "r+b");
                fread(&root_off, sizeof(int), 1, file);
                fread(&head_off, sizeof(int), 1, file);
                fread(&tail_off, sizeof(int), 1, file);
                fread(&append_off, sizeof(int), 1, file);
            }
        }

    public:
        FileManager() {
            filename[0] = '\0';
            isOpened = false;
            root_off = head_off = tail_off = -1;
            append_off = start;
            file = nullptr;
        }

        ~FileManager() {
            if (isOpened) {
                close_file();
            }
        }

        void set_fileName(const char *fname) {
            strcpy(filename, fname);
        }

        void clear_fileName() {
            filename[0] = '\0';
        }

        bool open_file() {
            if (isOpened) {
                return false;
            } else {
                init();
                isOpened = true;
                return true;
            }
        }

        bool close_file() {
            if (!isOpened) {
                std::cout << "false!";
                return false;
            } else {
                fseek(file, 0, SEEK_SET);
                fwrite(&root_off, sizeof(int), 1, file);
                fwrite(&head_off, sizeof(int), 1, file);
                fwrite(&tail_off, sizeof(int), 1, file);
                fwrite(&append_off, sizeof(int), 1, file);
                fclose(file);
                root_off = head_off = tail_off = -1;
                append_off = start;
                file = nullptr;
                isOpened = false;
                return true;
            }
        }

        bool clean() {
            if (!isOpened) return false;
            else {
                fclose(file);
                root_off = head_off = tail_off = -1;
                append_off = start;
                file = fopen(filename, "w+");
                fclose(file);
                file = fopen(filename, "wb+");
                fseek(file, 0, SEEK_SET);
                fwrite(&root_off, sizeof(int), 1, file);
                fwrite(&head_off, sizeof(int), 1, file);
                fwrite(&tail_off, sizeof(int), 1, file);
                fwrite(&append_off, sizeof(int), 1, file);
                return true;
            }
        }

        bool is_opened() {
            return isOpened;
        }

        void get_block(addType offset, Node &ret) {
            ret.address = offset;
            fread(&ret.isLeaf, sizeof(bool), 1, file);
            short K_size, V_size, Ch_size;

            fread(&K_size, sizeof(short), 1, file);
            if (K_size != 0)ret.keys.read_file(file, K_size); else ret.keys.shorten_len(0);
            fread(&V_size, sizeof(short), 1, file);
            if (V_size != 0)ret.vals.read_file(file, V_size); else ret.vals.shorten_len(0);
            fread(&Ch_size, sizeof(short), 1, file);
            if (Ch_size != 0)ret.childs.read_file(file, Ch_size); else ret.childs.shorten_len(0);
            fread(&ret.next, sizeof(addType), 1, file);
        }

        bool get_next_block(const Node &cur, Node &ret) {
            if (cur.next == -1)
                return false;
            else {
                get_block(cur.next, ret);
                return true;
            }
        }

        bool get_root(Node &ret) {
            if (root_off == -1) return false;
            else  get_block(root_off, ret);
            return true;
        }

        bool get_head(Node &ret) {
            if (head_off == -1) return false;
            if (head_off == 0)
                get_block(tree_utility_byte, ret);
            else
                get_block(head_off, ret);
            return true;
        }

        bool get_tail(Node &ret) {
            if (tail_off == -1) return false;
            if (tail_off == 0)
                get_block(tree_utility_byte, ret);
            else
                get_block(tail_off, ret);
            return true;
        }

        void append_block(Node &ret, bool isLeaf) {
            ret.clear();
            ret.address = append_off;
            ret.isLeaf = isLeaf;
            append_off += BlockSize;
        }

        void write_block(Node &now) {
            fwrite(&now.isLeaf, sizeof(bool), 1, file);
            short write_in = now.keys.size();
            fwrite(&write_in, sizeof(short), 1, file);
            if (now.keys.size() != 0) now.keys.write_file(file);
            write_in = now.vals.size();
            fwrite(&write_in, sizeof(short), 1, file);
            if (now.vals.size() != 0) now.vals.write_file(file);
            write_in = now.childs.size();
            fwrite(&write_in, sizeof(short), 1, file);
            if (now.childs.size() != 0) now.childs.write_file(file);
            fwrite(&now.next, sizeof(addType), 1, file);

        }

        void set_root(addType offset) {
            root_off = offset;
        }

        addType get_root() {
            return root_off;
        }

        sjtu::vector<Value_Type> traverse_multi(const Key_Type &K) {
            sjtu::vector<Value_Type> ans;
            if (head_off == -1) {
                return ans;
            }
            Node now;
            get_block(head_off, now);
            while (true) {
                if (Cmp(now.keys[now.keys.size() - 1], K)) {
                    if (now.next != -1)
                        get_block(now.next, now);
                    else return ans;
                } else {
                    int i;
                    for (i = 0; i < now.keys.size(); ++i) {
                        if (Equal(now.keys[i], K)) break;
                    }
                    --i;
                    while (true) {
                        i++;
                        if (!Equal(now.keys[i], K)) break;
                        ans.push_back(now.vals[i]);
                        if (i == now.keys.size() - 1) {
                            if (now.next == -1) {
                                return ans;
                            }
                            get_block(now.next, now);
                            i = -1;
                        }
                    }
                    break;
                }

            }
            return ans;
        }

        void traverse() {
            if (head_off == -1) {
                std::cout << "empty!\n";
                return;
            }
            std::cout << root_off << "root_off\n";
            Node now;
            get_block(head_off, now);
            int ans = 0;

            while (true) {
                for (int i = 0; i < now.keys.size(); ++i) {
                    ans++;
                }
                if (now.next == -1)
                    break;
                get_block(now.next, now);
            }
            std::cout << ans;
        }
    };
};

#endif