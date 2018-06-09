BPIusTree(const char*)

~BPIusTree():void

open\_file():bool

close\_file():bool

find(const Key\_Type&): Value\_Type

modify(Key\_Type, Value\_Type): bool

insert(const Key\_Type&, const Value\_Type&): bool

erase(Key\_Type): bool
- - - -
idUser      < id, user >

sale        < trainId, train >

nSale       < trainId, train >

locTrain    < loc, trainSet >

direct      < {loc1, loc2, catalog}, trainSet >

transfer    < {loc1, loc2, catalog}, {time, train1, train2, loc0, loc1, loc2} >

trKindTicket    < {trainId, date, kind}, ticketSet >

trDate          < {trainId, date}, ticketSet >

idTicket        < {id, date, catalog}, ticketSet >

infoOrderId     < {id, trainId, loc1, loc2, date, kind}, OrderId >

orderIdTicket   < OrderId, ticket >

station         <{trainNum, loc}, Station>
- - - -
# register
	if 写入用户信息成功
		return id
	else
		return -1

# query\_file
	在userTree中find(id)
	if 找到
		输出信息
	else return 0

# modify\_profile
	在userTree中find(id)
	if 找到
		在userTree中modify信息
	else return 0

# modify\_privilege
在userTree中find(id1)、find(id2)
if id都存在 且 权限满足要求
	在userTree中modify信息
	return 1
else	return 0

- - - -

# query\_ticket(loc1, loc2, date, catalogs)
	for catalogSet
		查找{loc1, loc2, catalog}的trainSet
		for trainSet中所有trainId
			查找ticket中所有{trainId, date, catalog}的购票记录ticketSet
			for ticketSet
				if 这次购买和[loc1, loc2]有交
					这趟车的某kind票数 -= num

# query\_transfer
	for catalogSet
		for 包含loc1的trainSet1
			for 包含loc2的trainSet2
				for tr1 in trainSet1
					for tr2 in trainSet2
# buy\_ticket(id, num, train\_id, loc1, loc2, date, ticket\_kind)
	查找ticket中所有{trainId, date, kind}的购票记录
			for ticketSet
				if 这次购买和[loc1, loc2]有交
					票数 -= num
	if 票数 > 0
		购买成功
		查找{id, trainId, loc1, loc2, date, kind}的购票记录（合并或新建）
		修改ticket相关的索引
# query\_order(id, date, catalog)
	查找ticket中所有{id, date, catalog}的购票记录
# refund\_ticket
	查找{id, trainId, loc1, loc2, date, kind}的购票记录
	if 数量 > num
		修改数量
		return 1
	else
		return 0
- - - -
# add\_train
	if S和N中都没有trainId
		往N中添加train
		return 1
	else
		return 0
# sale\_train
	if N中没有trainId
		return 0
	N中删除id
	S中添加id
	//修改direct
	for 当前火车所有(loc1, loc2)
		如果存在{loc1, loc2, catalog}
			往trainSet中加入当前火车
		else
			插入
	//修改transfer
	for 当前火车所有(loc1, loc2)
		用<loc1/2, TrainSet>
		for 所有(loc0, loc1)
			尝试更新/新建 中转
		for 所有(loc2, loc3)
			尝试更新/新建 中转

# query\_train(S/N)
# delete\_train(N)
# modify\_train(N)
