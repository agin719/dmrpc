#ifndef _RPC_SERVER_H_
#define _RPC_SERVER_H_

#include <google/protobuf/service.h>
#include <map>
#include <memory>
#include "net.h"

struct RpcMethod
{
    RpcMethod(google::protobuf::Service *service, const google::protobuf::MethodDescriptor *method):
        service_(service),
        method_(method)
    {
    }
    google::protobuf::Service *service_;
    const google::protobuf::MethodDescriptor *method_;
};

class RpcServer 
{
public:
    RpcServer(int port): port_(port)
    {
    }
    void register_service(google::protobuf::Service *service);
    void start();
    void recv(std::shared_ptr<net::Socket> sock, std::string data);

private:
    int port_;
    std::map<uint64_t, std::shared_ptr<RpcMethod>> method_map_;
};

#endif
