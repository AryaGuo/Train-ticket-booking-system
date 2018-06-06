#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <queue>
#include <cstring>
#include "BPlusTree.h"
#include "constant.h"
#include "Node.h"


#define start 4096

/**

 *
 * isLeaf, key number, val number, child number, then vector values sequentially.
 * finally is one addType, means the next node.
 */

namespace sjtu
{

template<class Key_Type,
         class Value_Type,
         class Compare = std::less<Key_Type>,
         class Compare_child = std::less<addType>
         >
class FileManager
{
private:
    typedef TreeNode<Key_Type, Value_Type> Node;

    char filename[50];
    FILE *file;

    bool isOpened;


public:

    addType root_off;
    addType head_off;
    addType tail_off;

    addType append_off;
private:

    void createFile()
    {
        file = fopen(filename, "w");

        root_off = head_off = tail_off = -1;
        append_off = 4096;
        fwrite(&root_off, sizeof(int), 1, file);
        fwrite(&head_off, sizeof(int), 1, file);
        fwrite(&tail_off, sizeof(int), 1, file);
        fwrite(&append_off, sizeof(int), 1, file);

        fclose(file);
    }

    void init()
    {
        file = fopen(filename, "r");
        if(file==nullptr)
        {
            createFile();
            file = fopen(filename, "r+");
        }
        else
        {
            file = fopen(filename, "r+");
            fread(&root_off, sizeof(int), 1, file);
            fread(&head_off, sizeof(int), 1, file);
            fread(&tail_off, sizeof(int), 1, file);
            fread(&append_off, sizeof(int), 1, file);
            //  std::cout<<root_off<<' '<<head_off<<' '<<tail_off<<' '<<append_off<<'\n';
        }
    }

public:
    FileManager()
    {
        filename[0] = '\0';
        isOpened = false;
        root_off = head_off = tail_off = -1;
        append_off = start;
        file = nullptr;
    }

    ~FileManager()
    {
        if(isOpened)
        {
            close_file();
        }
    }


    void set_fileName(const char *fname)
    {
        strcpy(filename, fname);
    }
    void clear_fileName()
    {
        filename[0] = '\0';
    }

    bool open_file()
    {
        if(isOpened)
        {
            return false;
        }
        else
        {
            init();
            isOpened = true;
            return true;
        }
    }
    bool close_file()
    {
        if(!isOpened)
        {
            return false;
        }
        else
        {
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

    bool clean()
    {
        if(!isOpened) return false;

        else
        {
            fclose(file);
            root_off = head_off = tail_off = -1;
            append_off = start;
            file = fopen(filename,"w+");
            fseek(file, 0, SEEK_SET);
            fwrite(&root_off, sizeof(int), 1, file);
            fwrite(&head_off, sizeof(int), 1, file);
            fwrite(&tail_off, sizeof(int), 1, file);
            fwrite(&append_off, sizeof(int), 1, file);


            return true;
        }
    }



    bool is_opened()
    {
        return isOpened;
    }

    void get_block(addType offset, Node &ret)
    {
        ret.address = offset;
        fseek(file, offset, SEEK_SET);

        fread(&ret.isLeaf, sizeof(bool), 1, file);
        short K_size, V_size, Ch_size;
        fread(&K_size, sizeof(short), 1, file);
        fread(&V_size, sizeof(short), 1, file);
        fread(&Ch_size, sizeof(short), 1, file);

        if( K_size != 0)ret.keys.file_read(file, K_size);       else ret.keys.shorten_len(0);
        if( V_size != 0)ret.vals.file_read(file, V_size);       else ret.vals.shorten_len(0);
        if( Ch_size != 0)ret.childs.file_read(file, Ch_size);   else ret.childs.shorten_len(0);

        fread(&ret.next, sizeof(addType), 1, file);
    }


    bool get_next_block(const Node &cur, Node &ret)
    {

        if(cur.next == -1)
            return false;
        else
        {
            get_block(cur.next, ret);
            return true;
        }
    }

    bool get_root(Node &ret)
    {
        if(root_off == -1) {   return false;}
        else
            {  get_block(root_off, ret);}
        return true;
    }
    bool get_head(Node &ret)
    {
        if(head_off == -1) return false;

        if(head_off == 0)
            get_block(tree_utility_byte, ret);
        else
            get_block(head_off, ret);
        return true;
    }
    bool get_tail(Node &ret)
    {
        if(tail_off == -1) return false;

        if(tail_off == 0)
            get_block(tree_utility_byte, ret);
        else
            get_block(tail_off, ret);
        return true;
    }


    void append_block(Node &ret, bool isLeaf)
    {
        ret.clear();
        ret.address = append_off;
        ret.isLeaf = isLeaf;

        append_off += BlockSize;
    }

    void write_block(Node &cur)
    {
        fseek(file, cur.address, SEEK_SET);

        fwrite(&cur.isLeaf, sizeof(bool), 1, file);

        short   tmp = cur.keys.size();              fwrite(&tmp, sizeof(short), 1, file);
                tmp = cur.vals.size();              fwrite(&tmp, sizeof(short), 1, file);
                tmp = cur.childs.size();            fwrite(&tmp, sizeof(short), 1, file);

        if(cur.keys.size()!= 0)cur.keys.file_write(file);
        if(cur.vals.size()!= 0)cur.vals.file_write(file);
        if(cur.childs.size()!= 0)cur.childs.file_write(file);
        fwrite(&cur.next, sizeof(addType), 1, file);
    }

    void write_block_skip_father(Node &cur)
    {
        fseek(file, cur.address, SEEK_SET);

        fwrite(&cur.isLeaf,sizeof(bool), 1, file);

        short   tmp = cur.keys.size();          fwrite(&tmp, sizeof(short), 1, file);
                tmp = cur.vals.size();          fwrite(&tmp, sizeof(short), 1, file);
                tmp = cur.childs.size();        fwrite(&tmp, sizeof(short), 1, file);

        cur.keys.file_write(file);
        cur.vals.file_write(file);
        cur.childs.file_write(file);

        fseek(file, sizeof(addType), SEEK_CUR);
        fwrite(&cur.next, sizeof(addType), 1, file);
    }
    void set_root(addType rtoff)
    {
        root_off = rtoff;
    }

    addType get_root()
    {
        return root_off;
    }


    void traverse()
    {
        if(head_off == -1)
        {
            printf("traverse empty tree\n");
            return;
        }
        std::cout<<root_off<<"root_off\n";
        Node cur;
        get_block(head_off, cur);

        while(true)
        {
            for(int i = 0; i < cur.keys.size(); ++i)
                std::cout << cur.keys[i] << ' ' << cur.vals[i] << std::endl;

            if(cur.next == -1)
                break;
            get_block(cur.next, cur);
        }
    }
};


};


#endif
