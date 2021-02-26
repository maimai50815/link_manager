#ifndef LINK_RPC_H
#define LINK_RPC_H

#include "common.h"

namespace link_master
{
class LinkRpc
{
public:
	LinkRpc();
	static void execute(TopicInfo&);
private:
	const int master_port_ = 9000;
};

inline LinkRpc::LinkRpc()
{
}

inline void LinkRpc::execute(TopicInfo& info)
{
}

}/* end of namespace */
#endif
