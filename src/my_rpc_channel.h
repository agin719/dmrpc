#ifndef _MY_RPC_CHANNEL_H_
#define _MY_RPC_CHANNEL_H_

#include <google/protobuf/service.h>
#include <google/protobuf/message.h>
#include "net.h"

class MyRpcChannel : public google::protobuf::RpcChannel 
{
public:
    MyRpcChannel(const char* ip, int port);
    virtual ~MyRpcChannel() {}

    virtual void CallMethod(const google::protobuf::MethodDescriptor* method,
            google::protobuf::RpcController* controller,
            const google::protobuf::Message* request,
            google::protobuf::Message* response,
            google::protobuf::Closure* done);
private:
    net::Socket sock_;
};

#endif
