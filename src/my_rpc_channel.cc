#include "my_rpc_channel.h"
#include "echo.pb.h"
#include <city.h>
#include <glog/logging.h>

using namespace std;
using namespace google::protobuf;

MyRpcChannel::MyRpcChannel(const char *ip, int port) :
    sock_()
{
    int rt = sock_.connect(ip, port);
    if (rt < 0) {
        LOG(ERROR) << "MyRpcChannel::MyRpcChannel connect failed";
        assert(0);
    }
}

void MyRpcChannel::CallMethod(const MethodDescriptor* method, 
        RpcController* controller,
        const Message* request,
        Message* response,
        Closure* done)
{
    uint64_t opcode = ::CityHash64(method->full_name().c_str(), method->full_name().size());
    char buf[sizeof(opcode)+request->ByteSize()];
    memcpy(buf, &opcode, sizeof(opcode));
    request->SerializeToArray(buf+sizeof(opcode), request->ByteSize());
    string send_data(buf, sizeof(opcode)+request->ByteSize());
    sock_.send(send_data);

    string data;
    sock_.recv(data);
    response->ParseFromArray(data.c_str()+sizeof(opcode), data.size()-sizeof(opcode));
    if (done){
        done->Run();
    }
}
