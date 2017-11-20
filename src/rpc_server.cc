#include "rpc_server.h"
#include "net.h"
#include "echo.pb.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <city.h>
#include <glog/logging.h>

using namespace net;
using namespace std;
using namespace google::protobuf;

void RpcServer::register_service(Service *service)
{
    const ServiceDescriptor * sd = service->GetDescriptor();
    for (int i = 0; i < sd->method_count(); i++)
    {
        const MethodDescriptor *method = sd->method(i);
        uint64_t opcode = ::CityHash64(method->full_name().c_str(), method->full_name().size());
        auto it = method_map_.find(opcode);
        if (it != method_map_.end())
        {
            LOG(ERROR) << "RpcServer::register_service " << method->full_name() << " already in method_map";
            continue;
        }
        method_map_.insert({opcode, make_shared<RpcMethod>(service, method)});
    }
}

void RpcServer::start()
{
    net::Socket socket;
    socket.listen(port_, bind(&RpcServer::recv, this, std::placeholders::_1, std::placeholders::_2));
}

void RpcServer::recv(shared_ptr<Socket> sock, string data)
{
    uint64_t opcode = (uint64_t) (*((const uint64_t*) data.c_str()));
    auto it = method_map_.find(opcode);
    if (it == method_map_.end())
    {
        LOG(ERROR) << "RpcMethod::recv find error";
        return;
    }
    const MethodDescriptor *method = it->second->method_;
    Service *service = it->second->service_;
    const Message *req = &service->GetRequestPrototype(method);
    const Message *resp = &service->GetResponsePrototype(method);
    Message *request = req->New();
    Message *response = resp->New();
    request->ParseFromArray(data.data()+sizeof(opcode), data.size()-sizeof(opcode));
    service->CallMethod(method, NULL, request, response, NULL);
    
    char buf[sizeof(opcode)+response->ByteSize()];
    memcpy(buf, &opcode, sizeof(opcode));
    response->SerializeToArray(buf+sizeof(opcode), response->ByteSize());
    string send_data(buf, sizeof(opcode)+response->ByteSize());
    sock->send(send_data);
}
