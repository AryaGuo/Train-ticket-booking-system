#ifndef CONSTANT_H
#define CONSTANT_H



namespace sjtu {

    typedef  int  addType; /** or others  */

    const addType tree_utility_byte = 16;       ///  root_offset, head_offset, tail_offset, append_offset 4 + 4 + 4 + 4
    const addType node_utility_byte = 15;       ///  address, isLeaf, K_size, V_size, Ch_size, next. 4 + 1 + 2 + 2 + 2 + 4

    const int BlockSize = 4096;                 ///ؽ���Ż�

    const int maxnum_station = 2000;            ///  һ����վ������
    const int maxnum_loc = 500;                 ///  ��һ��վ����һ��վ�������� ����������֪��׼��׼ȷ��

    const int maxnum_user = 40;                 ///  name       ��󳤶�
    const int maxnum_other = 20;                ///  password, email, phone, id, loc �ȵ� ��󳤶�
    const int maxnum_cat = 10;                  ///  catalog          ��󳤶�
    const int maxnum_ticket = 5;                ///  ticket_type            ��󳤶� (��Ȼ��Щ����д�ţ���û���õ�)

    const int TRAIN_CAPACITY = 2000;
};

#endif
