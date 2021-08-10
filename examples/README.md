# 在examples/路径执行构建命令
rm -rf build/ && mkdir build && cd build && cmake .. && make -j 8

# 启动服务端
dapr run --app-id callee --app-protocol grpc --app-port 6000  ./callee_server

# 启动客户端
dapr run --app-id echo_app --app-protocol grpc --app-port 6100 ./caller_client