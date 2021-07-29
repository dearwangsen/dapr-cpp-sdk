# 策略兼容，消除 warning
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# cmake 需要的最低版本
cmake_minimum_required(VERSION 3.5.1)

# 项目名
project(EXAMPLES C CXX)

# cmake 中使用 c++14
set (CMAKE_CXX_STANDARD 14)

# pb 协议兼容
set(protobuf_MODULE_COMPATIBLE TRUE)

# 用于链接的库文件
set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
set(_GRPC_GRPCPP gRPC::grpc++)

# 文件完整路径的信息表达式，用在生成 grpc proto 的命令
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

# 引入外部依赖包
find_package(Threads REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# 添加外部库目录 libdapr_server.a
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../src" dapr_server_binary_dir)

# 头文件目录
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../src/dapr/proto/common/v1")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../src/dapr/proto/runtime/v1")

# 生成可执行文件
add_executable(callee_server callee_server.cc)
target_link_libraries (
  callee_server
    dapr_server
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
    )

# 生成可执行文件
add_executable(caller_client caller_client.cc)
target_link_libraries (
  caller_client
    dapr_server
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
    )