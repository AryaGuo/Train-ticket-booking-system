#ifndef CONSTANT_H
#define CONSTANT_H



namespace sjtu {

    typedef  int  addType; /** or others  */

    const addType tree_utility_byte = 16;       ///  root_offset, head_offset, tail_offset, append_offset 4 + 4 + 4 + 4
    const addType node_utility_byte = 15;       ///  address, isLeaf, K_size, V_size, Ch_size, next. 4 + 1 + 2 + 2 + 2 + 4

    const int BlockSize = 4096;                 ///亟待优化

    const int maxnum_station = 2000;            ///  一个车站最大火车数
    const int maxnum_loc = 500;                 ///  从一个站到另一个站的最大火车数 （这两个不知道准不准确）

    const int maxnum_user = 40;                 ///  name       最大长度
    const int maxnum_other = 20;                ///  password, email, phone, id, loc 等等 最大长度
    const int maxnum_cat = 10;                  ///  catalog          最大长度
    const int maxnum_ticket = 5;                ///  ticket_type            最大长度 (当然这些都先写着，并没有用到)

};

#endif
