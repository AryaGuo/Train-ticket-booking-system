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

    char filename[100];
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
        file = fopen(filename, "wb");

        root_off = head_off = tail_off = -1;
        append_off = start;
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
            file = fopen(filename, "wb+");
        }
        else
        {
            file = fopen(filename, "wb+");
            fread(&root_off, sizeof(int), 1, file);
            fread(&head_off, sizeof(int), 1, file);
            fread(&tail_off, sizeof(int), 1, file);
            fread(&append_off, sizeof(int), 1, file);
           
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
            fclose(file);
            file = fopen(filename,"wb+");
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
        if( K_size != 0)ret.keys.read_file(file, K_size);       else ret.keys.shorten_len(0);


        fread(&V_size, sizeof(short), 1, file);
        if( V_size != 0)ret.vals.read_file(file, V_size);       else ret.vals.shorten_len(0);

        fread(&Ch_size, sizeof(short), 1, file);
        if( Ch_size != 0)ret.childs.read_file(file, Ch_size);   else ret.childs.shorten_len(0);

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
        if(root_off == -1)  return false;
        else get_block(root_off, ret);
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


    void write_block(Node &now)
    {
        bool xxx = fseek(file, now.address, SEEK_SET);
        fwrite(&now.isLeaf, sizeof(bool), 1, file);


        short write_in = now.keys.size();

        fwrite(&write_in, sizeof(short), 1, file);
        if(now.keys.size()!= 0)      now.keys.write_file(file);
        write_in = now.vals.size();

        fwrite(&write_in, sizeof(short), 1, file);
        if(now.vals.size()!= 0)     now.vals.write_file(file);

        write_in = now.childs.size();
        fwrite(&write_in, sizeof(short), 1, file);
        if(now.childs.size()!= 0)   now.childs.write_file(file);

        fwrite(&now.next, sizeof(addType), 1, file);

    }


    void set_root(addType offset)
    {
        root_off = offset;
    }

    addType get_root()
    {
        return root_off;
    }


    void traverse()
    {
        if(head_off == -1)
        {
            std::cout<<"empty!\n";
            return;
        }
        std::cout<<root_off<<"root_off\n";
        Node now;
        get_block(head_off, now);

        while(true)
        {
            for(int i = 0; i < now.keys.size(); ++i)
                std::cout << now.keys[i] << ' ' << now.vals[i] << std::endl;

            if(now.next == -1)
                break;
            get_block(now.next, now);
        }
    }
};


};


#endif
