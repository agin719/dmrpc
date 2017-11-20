#include "echo.pb.h"
#include "my_rpc_channel.h"
#include <glog/logging.h>

void echo_done()
{
    LOG(INFO) << "echo done";
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s <ip> <port>\n", argv[0]);
        return -1;
    }
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();
    FLAGS_logtostderr = true;
    
    MyRpcChannel channel(argv[1], ::atoi(argv[2]));
    echo::EchoService::Stub stub(&channel);
    echo::EchoRequest request;
    echo::EchoResponse response;
    request.set_message("hello rpc");
    
    stub.Echo(NULL, &request, &response, google::protobuf::NewCallback(echo_done));
    LOG(INFO) << response.message();
    echo::Param param;
    echo::Result result;
    param.set_x(12);
    param.set_y(21);
    stub.Add(NULL, &param, &result, NULL);
    LOG(INFO) << result.result();
    while (1);
    return 0;
}
