#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include "FileManager.h"
#include "Node.h"
#include <cstring>
#include <cmath>

#include "vector.h"
#include "utility.hpp"

namespace sjtu
{
template<class Key_Type,
         class Value_Type,
         class Compare = std::less<Key_Type>,
         class Compare_value = std::less<Value_Type>
         >
/**
b+树本体
*/
class BPlusTree
{
    bool Cmp(const Key_Type &x, const Key_Type &y) const        ///<
    {
        static Compare _cmp;
        return _cmp(x, y);
    }
    bool Equal(const Key_Type &x, const Key_Type &y) const      /// ==
    {
        if (Cmp(x, y) || Cmp(y, x)) return 0;
        else return 1;
    }


    bool Cmp_value(const Value_Type &x, const Value_Type &y) const        ///<
    {
        static Compare_value _cmp;
        return _cmp(x, y);
    }
    bool Equal_value(const Value_Type &x, const Value_Type &y) const      /// ==
    {
        if (Cmp_value(x, y) || Cmp_value(y, x)) return 0;
        else return 1;
    }

private:
    typedef TreeNode<Key_Type, Value_Type> Node;

    struct retT
    {
        bool success;
        bool modified;
        retT(bool a, bool b): success(a), modified(b) { }
        retT() {}
    };

private:
    char filename[50];

    int branch_degree, leaf_degree;
    int K_byte, V_byte; /**     size of   key_byte, value_byte            */

    Node pool[50];
public:
    int cnt;            /**    记录pool的num     */
    FileManager<Key_Type, Value_Type> bm;
private:

    retT erase_node(Node &cur, const Key_Type &K)
    {
        int sbl_off = 1;

        if(cur.isLeaf)
        {
            int delpos = cur.search_exact(K);

            if(delpos == -1)
                return retT(false, false);
            else
            {
                cur.keys.erase(delpos);
                cur.vals.erase(delpos);
                return retT(true, true);
            }
        }
        else
        {
            Node &ch = pool[cnt++];
            ch.clear();

            int ch_pos = find_child(cur, K, ch);

            retT ret_info = erase_node(ch, K);

            if(!ret_info.success)
            {
                cnt--;
                return retT(false, false);
            }

            if(ch.isLeaf)
            {
                if(ch.keys.size() >= (int) ceil(leaf_degree / 2.0))
                {
                    bm.write_block(ch);
                    cnt--;
                    return retT(true, false);
                }
                /**  大于一半有东西直接返回并写入 ，否则开始骚操作     */

                else
                {
                    Node &sbl = pool[cnt++];
                    int sbl_pos;
                    int key_pos;
                    Key_Type K_bt;
                    Node *l_node, *r_node;
                    if(ch_pos == cur.childs.size()-1)
                    {
                        bm.get_block(cur.childs[cur.childs.size()-2], sbl);
                        sbl_pos = cur.childs.size() - 2;
                    }
                    else
                    {
                        bm.get_block(cur.childs[ch_pos + sbl_off], sbl);
                        sbl_pos = ch_pos + sbl_off;
                    }


                    if(ch_pos < sbl_pos)
                    {
                        key_pos = ch_pos;
                        l_node = &ch;
                        r_node = &sbl;
                    }
                    else
                    {
                        key_pos = sbl_pos;
                        l_node = &sbl;
                        r_node = &ch;
                    }

                    K_bt = cur.keys[key_pos];

                    if(ch.keys.size() + sbl.keys.size() <= leaf_degree)
                    {
                       // std::cout<<"hebing leaf\n";
                        l_node->next = r_node->next;
                        if (bm.tail_off == r_node->address)
                            bm.tail_off = l_node->address;

                        for (int i = 0; i < r_node->keys.size(); ++i)
                        {
                            l_node->keys.push_back(r_node->keys[i]);
                            l_node->vals.push_back(r_node->vals[i]);
                        }

                        cur.keys.erase(key_pos);
                        cur.childs.erase(key_pos + 1);

                        bm.write_block(*l_node);
                        //bm.write_block(*r_node);    /**     尽管这个右儿子没甚用      */

                        cnt -= 2;
                        return retT(true, true);
                    }
                    else                             /**   重新分配      */
                    {
                        if(sbl_pos < ch_pos)
                        {
                            while(ch.keys.size() < (int) ceil(leaf_degree / 2.0)){
                            Key_Type key_shift = sbl.keys.back();
                            Value_Type val_shift = sbl.vals.back();

                            sbl.keys.pop_back();
                            sbl.vals.pop_back();

                            ch.keys.insert(0, key_shift);
                            ch.vals.insert(0, val_shift);

                            cur.keys[key_pos] = ch.keys[0];
                            }
                            bm.write_block(ch);
                            bm.write_block(sbl);


                            cnt -= 2;
                            return retT(true, true);
                        }

                        else
                        {
                            while(ch.keys.size() < (int) ceil(leaf_degree / 2.0))
                            {


                            Key_Type key_shift = sbl.keys.front();
                            Value_Type val_shift = sbl.vals.front();

                            sbl.keys.erase(0);
                            sbl.vals.erase(0);

                            ch.keys.push_back(key_shift);
                            ch.vals.push_back(val_shift);

                            cur.keys[key_pos] = sbl.keys[0];
                            }
                            bm.write_block(ch);
                            bm.write_block(sbl);

                            cnt -= 2;
                            return retT(true, true);
                        }
                    }
                }
            }
            /**   子节点非叶子节点      */
            else
            {
                if(ch.childs.size() >= (int) ceil(branch_degree / 2.0))
                {
                    if(ret_info.modified)
                        bm.write_block(ch);
                    cnt--;
                    return retT(true, false);
                }

                else
                {

                    Node &sbl = pool[cnt++];
                    int sbl_pos;
                    int key_pos;
                    Key_Type K_bt;
                    Node *l_node, *r_node;

                    if(ch_pos == cur.childs.size()-1)
                    {
                        bm.get_block(cur.childs[cur.childs.size()-2], sbl);
                        sbl_pos = cur.childs.size() - (int) 2;
                    }
                    else
                    {
                        bm.get_block(cur.childs[ch_pos + sbl_off], sbl);
                        sbl_pos = ch_pos + sbl_off;
                        //sbl_off *= -1;
                    }

                        if(ch_pos < sbl_pos)
                        {
                            key_pos = ch_pos;
                            l_node = &ch;
                            r_node = &sbl;
                        }
                        else
                        {
                            key_pos = sbl_pos;
                            l_node = &sbl;
                            r_node = &ch;
                        }

                    K_bt = cur.keys[key_pos];

                    if(ch.childs.size() + sbl.keys.size() <= branch_degree)
                    {

                       // std::cout<<"hebing branch\n";
                       // system("pause");
                        l_node->keys.push_back(K_bt);
                        for (int i = 0; i < r_node->keys.size(); ++i)
                            l_node->keys.push_back(r_node->keys[i]);
                        for(int i = 0; i < r_node->childs.size(); ++i)
                            l_node->childs.push_back(r_node->childs[i]);

                        cur.keys.erase(key_pos);
                        cur.childs.erase(key_pos + (int) 1);

                        bm.write_block(*l_node);
                        bm.write_block(*r_node);

                        cnt -= 2;
                        return retT(true, true);
                    }

                    else
                    {
                        if(sbl_pos < ch_pos)
                        {
                            while(ch.childs.size() < (int) ceil(branch_degree / 2.0)){
                            Key_Type key_shift = sbl.keys.back();
                            addType child_shift = sbl.childs.back();

                            sbl.keys.pop_back();
                            sbl.childs.pop_back();

                            ch.keys.insert(0, K_bt);
                            ch.childs.insert(0, child_shift);

                            cur.keys[key_pos] = key_shift;
                            }
                            bm.write_block(ch);
                            bm.write_block(sbl);

                            cnt -= 2;
                            return retT(true, true);
                        }

                        else
                        {
                            while(ch.childs.size() >= (int) ceil(branch_degree / 2.0)){
                            Key_Type key_shift = sbl.keys.front();
                            addType child_shift = sbl.childs.front();

                            sbl.keys.erase(0);
                            sbl.childs.erase(0);

                            ch.keys.push_back(key_shift);
                            ch.childs.push_back(child_shift);

                            cur.keys[key_pos] = key_shift;
                            }
                            bm.write_block(ch);
                            bm.write_block(sbl);

                            cnt -= 2;
                            return retT(true, true);
                        }
                    }
                }
            }
        }
    }

    Key_Type split_branch(Node &B, Node &B_next)
    {
         int mid = (int) ceil(branch_degree / 2.0);
         Key_Type mid_key = B.keys[mid - 1];

         B_next.childs.assign(B.childs, mid, B.childs.size());
         B_next.keys.assign(B.keys, mid, B.keys.size());

         B.childs.shorten_len(mid);
         B.keys.shorten_len(mid - 1);
         return mid_key;
    }

    void split_leaf(Node &L, Node &L_next)
    {

        L_next.keys.assign(L.keys, L.keys.size()/2, L.keys.size());
        L_next.vals.assign(L.vals, L.vals.size()/2, L.vals.size());
        L.keys.shorten_len(L.keys.size()/2);
        L.vals.shorten_len(L.vals.size()/2);
    }
    bool insert_in_leaf(Node &cur, const Key_Type &K, const Value_Type &V)
    {

        int i = cur.search_sup(K);
       // std::cout<<"search_sup\n";

        if (i == -1)
        {
            cur.keys.push_back(K);
            cur.vals.push_back(V);
            return true;
        }
      //  std::cout<<"insert!ssssssssssssssssssssssssssssssssssss\n";
        cur.keys.insert(i, K);
        cur.vals.insert(i, V);
        return true;
    }

    retT insert_node(Node &cur, const Key_Type &K, const Value_Type &V)
    {
       // std::cout<<"cur.keys.size "<<cur.keys.size()<<'\n';
        if (cur.isLeaf)
        {
            //std::cout<<"isleaf\n";
            bool success = insert_in_leaf(cur, K, V);

            return retT(success, success);
        }
        else
        {
            Node &ch = pool[cnt++];

            int ch_pos = find_child(cur, K, ch);
            retT ret_info = insert_node(ch, K, V);

            if (!ret_info.success)
            {
                cnt--;
                return retT(false, false);
            }

            if (ch.isLeaf)
            {
                if (ch.keys.size() <= leaf_degree)
                {
                    bm.write_block(ch);
                    cnt--;
                    return retT(true, false);
                }

                else
                {
                    //std::cout<<"isleaf spilt_leaf\n";
                    //std::cout<<"                    .size()>leaf_degree\n";
                    Node &newLeaf = pool[cnt++];
                    bm.append_block(newLeaf, true);



                    if (bm.tail_off == ch.address)
                        bm.tail_off = newLeaf.address;

                    split_leaf(ch, newLeaf);
                    newLeaf.next = ch.next;
                    ch.next = newLeaf.address;


                    cur.childs.insert(ch_pos + 1, newLeaf.address);
                    cur.keys.insert(ch_pos, newLeaf.keys[0]);

                    bm.write_block(ch);
                    bm.write_block(newLeaf);

                    cnt -= 2;
                    return retT(true, true);
                }
            }
            else
            {
                if (ch.childs.size() <= branch_degree)
                {
                    bm.write_block(ch);
                    cnt--;
                    return retT(true, false);
                }
                else
                {
                    //std::cout<<"spilt_branch\n";
                   // system("pause");
                    Key_Type mid_key;
                    Node &newBranch = pool[cnt++];
                    bm.append_block(newBranch, false);

                    mid_key = split_branch(ch, newBranch);
                    cur.childs.insert(ch_pos + 1, newBranch.address);
                    cur.keys.insert(ch_pos, mid_key);

                    bm.write_block(ch);
                    bm.write_block(newBranch);
                    cnt -= 2;
                    return retT(true, true);
                }
            }

        }
    }


    int find_child(Node &cur, const Key_Type K, Node &child)
    {
        int i = cur.search_sup(K);

        if (i == -1)
        {
            bm.get_block(cur.childs.back(), child);
            return cur.childs.size() - 1;
        }
        else
        {
            bm.get_block(cur.childs[i], child);
            return i;
        }
    }


    void search_to_leaf(Key_Type K, Node &ret)
    {
        bm.get_root(ret);

        while (!ret.isLeaf)
        {
            find_child(ret, K, ret);
        }
    }



    int find_child_muti(Node &cur, const Key_Type K)
    {

        int i = cur.search_sup_muti(K);

        if (i == -1)
        {
            bm.get_block(cur.childs.back(), cur);

            return cur.childs.size() - 1;
        }
        else
        {
            bm.get_block(cur.childs[i], cur);
            return i;
        }
    }




    void search_to_leaf_muti(Key_Type K, Node &ret)
    {
        bm.get_root(ret);

        while (!ret.isLeaf)
        {
            find_child_muti(ret, K);
        }
    }


public:
    explicit BPlusTree(const char *fname)
    {
        strcpy(filename, fname);
        bm.set_fileName(fname);

        K_byte = sizeof(Key_Type), V_byte = sizeof(Value_Type);
        cnt = 0;

        leaf_degree = (BlockSize - node_utility_byte) / (K_byte + V_byte);
        branch_degree = (BlockSize - node_utility_byte) / (sizeof(addType) + K_byte);

        //    leaf_degree = branch_degree = 20;

        std::cout<<"degree "<<leaf_degree<<' '<<branch_degree<<'\n';

    }

    ~BPlusTree()
    {
        if (bm.is_opened())
            bm.close_file();            /**  简单粗暴，但是会不会炸呢。。。。  (感觉会，很慌)     */
    }


    bool clean()
    {
        if(!bm.is_opened())  return false;
        else
        {
            for(int i = 0; i < cnt; i++)
            {
                pool[i].clear();
            }
            cnt = 0;
            bm.clean();
            return true;
        }
    }



    bool open_file()
    {
        return bm.open_file();
    }
    bool close_file()
    {
        return bm.close_file();
    }

    sjtu::vector<Value_Type> find_muti(const Key_Type &K)
    {
        sjtu::vector<Value_Type> ans;
        Node &now = pool[cnt++];
        Node &pre = pool[cnt++];
        search_to_leaf_muti(K, now);
        int i = now.search_exact(K);
        cnt--;

        if (i == -1)
            {
                while(true)
                {
                    bm.get_block(now.next,now);
                    i = now.search_exact(K);
                    if(i != -1) break;
                }
            }
                i--;
                while(1)
                {
                    i++;
                    if(!Equal(now.keys[i], K)) break;
                    ans.push_back(now.vals[i]);
                    if(i == now.keys.size() - 1) {
                            if(now.next == -1)
                            {
                                return ans;
                            }
                            bm.get_block(now.next,now); i = -1;
                    }
                }

            return ans;
    }



    bool erase_muti(const Key_Type &K, const Value_Type &V)
    {
        sjtu::vector<Value_Type> tem = find_muti(K);
        if(tem.size() == 0) return false;

        while(  erase(K)    ){

        }

        for(int i = 0; i < tem.size(); ++i)
        {
            if(!Equal_value(V,tem[i])) insert(K,tem[i]);
        }

        return true;


    }


    pair<bool, Value_Type> find(const Key_Type &K)
    {
        Node &ret = pool[cnt++];
        search_to_leaf(K, ret);
        int i = ret.search_exact(K);
        cnt--;
        if (i == -1)
            return pair<bool, Value_Type>(false, Value_Type());                        /**这里会炸吗，感觉不会,没有什么奇怪的valuetype吧*/
        else
            return pair<bool, Value_Type>(true, ret.vals[i]);
    }

    bool modify(Key_Type K, Value_Type V)
    {
        Node &leafNode = pool[cnt++];

        search_to_leaf(K, leafNode);
        int keyPos = leafNode.search_exact(K);

        if (keyPos == -1)
        {
            cnt--;
            return false;
        }
        else
        {
            leafNode.vals[keyPos] = V;
            bm.write_block(leafNode);
            cnt--;
            return true;
        }
    }

    bool insert(const Key_Type &K, const Value_Type &V)
    {
        //std::cout<<"insert "<<K<<' '<<V<<'\n';
        Node &root = pool[cnt++];

        if (bm.root_off == -1)
        {
           // std::cout<<"root_off == -1 "<<K<<' '<<V<<'\n';
            bm.append_block(root, true);     ///什么都没有

            root.keys.push_back(K);
            root.vals.push_back(V);

            bm.set_root(root.address);
            bm.write_block(root);

            bm.head_off = root.address;
            bm.tail_off = root.address;
           // std::cout<<root.address<<" root.address! \n";
            cnt--;
            return true;
        }
       // std::cout<<"trst\n";
       // std::cout<<root.keys.size()<<'\n';
       // if(K >= 25) bm.jiade(root);
       // else
            bm.get_root(root);
        //std::cout<<root.keys.size()<<'\n';

        retT ret_info = insert_node(root, K, V);

        if (!ret_info.success)
        {
            cnt--;
           // std::cout<<"false "<<K<<' '<<V<<'\n';
            return false;                               /** 真的false就很尴尬了 */
        }
        /**考虑是否调整树 spilt 或者加树高*/
        else
        {
            if (root.isLeaf)
            {

                if (root.keys.size() > leaf_degree)
                {
                   // std::cout<<">leaf degree\n";
                    Node &newLeaf = pool[cnt++];
                    Node &newRoot = pool[cnt++];
                   // std::cout<<"newleaf & newroot\n";


                    bm.append_block(newLeaf, true);
                    //std::cout<<"append_block(newleaf)\n";


                    split_leaf(root, newLeaf);
                  //  std::cout<<"spilt_leaf\n";


                    newLeaf.next = root.next;
                    root.next = newLeaf.address;
                   // std::cout<<"next address\n";


                    if (bm.tail_off == root.address)
                        bm.tail_off = newLeaf.address;

                  //  std::cout<<"then add new root\n";

                    bm.append_block(newRoot, false);
                    newRoot.childs.push_back(root.address);
                    newRoot.childs.push_back(newLeaf.address);          //question
                    newRoot.keys.push_back(newLeaf.keys[0]);
                    bm.root_off = newRoot.address;

                    bm.write_block(root);
                  //  puts("");puts("");
                    bm.write_block(newLeaf);
                  //  puts("");puts("");
                    bm.write_block(newRoot);
                   // puts("");puts("");
                    cnt -= 3;
                  //  std::cout<<"is leaf  > degree"<<K<<' '<<V<<'\n';
                    return true;
                }
                /**否则直接写进去就好了*/
                else
                {
                    if(ret_info.modified)
                    {
                        bm.write_block(root);
                    }
                    cnt--;
                    //std::cout<<"is leaf <= degree "<<K<<' '<<V<<'\n';
                    return true;
                }
            }
            else
            {

                if (root.childs.size() > branch_degree)   /**spilt, 树高度++*/
                {
                    Node &newBranch = pool[cnt++];
                    Node &newRoot = pool[cnt++];
                    Key_Type mid_key;

                    bm.append_block(newBranch, false);

                    mid_key = split_branch(root, newBranch);

                    bm.append_block(newRoot, false);
                    newRoot.childs.push_back(root.address);
                    newRoot.childs.push_back(newBranch.address);
                    newRoot.keys.push_back(mid_key);
                    bm.root_off = newRoot.address;

                    bm.write_block(root);
                    bm.write_block(newBranch);
                    bm.write_block(newRoot);
                    cnt -= 3;
                   // std::cout<<"not leaf >=degree"<<K<<' '<<V<<'\n';
                    return true;
                }
                else
                {
                    if(ret_info.modified)
                    {
                        bm.write_block(root);
                    }
                    //std::cout<<"not leaf <degree "<<K<<' '<<V<<'\n';
                    cnt--;
                    return true;
                }

            }
        }
    }

    bool erase(Key_Type K)                   /**说起来万一key值不唯一就好玩了（保证解体）*/
    {
        if (bm.root_off == -1)
        {
            return false;
        }

        Node &root = pool[cnt++];
        retT ret_info;
        bm.get_root(root);
        ret_info = erase_node(root, K);

        if(!ret_info.success)
        {
            cnt--;
            return false;
        }

        else
        {
            if(root.isLeaf)                     /**  把b+树置为空    */
            {
                if(root.keys.size() == 0)
                {
                    bm.root_off = bm.head_off = bm.tail_off = -1;
                }
                if(ret_info.modified)
                    bm.write_block(root);
            }
            else
            {

                if(root.childs.size() == 1)   /**   减小树高*/
                {
                    std::cout<<"sub height\n";
                    bm.root_off = root.childs[0];
                }
                else if(ret_info.modified)
                    bm.write_block(root);
            }
            cnt--;
            return true;
        }
    }






};
};
#endif
