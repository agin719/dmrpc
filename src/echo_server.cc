#include "echo.pb.h"
#include "rpc_server.h"
#include <glog/logging.h>

using namespace google::protobuf;

class EchoServiceImpl : public echo::EchoService
{
public:
    EchoServiceImpl() {}
    virtual ~EchoServiceImpl() {}

    void Echo(google::protobuf::RpcController* controller,
            const echo::EchoRequest* request,
            echo::EchoResponse* response,
            google::protobuf::Closure* done)
    {
        response->set_message(request->message());
        if (done) {
            done->Run();
        }
    }
    
    void Add(google::protobuf::RpcController* controller,
            const echo::Param* request,
            echo::Result* response,
            google::protobuf::Closure* done)
    {
        int x = request->x();
        int y = request->y();
        response->set_result(x+y);
        if (done) {
            done->Run();
        }
    }

};

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return -1;
    }
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();
    FLAGS_logtostderr = true;

    RpcServer rpc_server(::atoi(argv[1]));
    Service *service = new EchoServiceImpl();
    rpc_server.register_service(service);
    rpc_server.start();

    return 0;
}
