# 启动服务端
dapr run --app-id callee --app-protocol grpc --app-port 6000  ./callee_server

# 启动客户端
dapr run --app-id echo_app --app-protocol grpc --app-port 6100 ./caller_client