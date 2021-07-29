# 策略兼容，消除 warning
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# cmake 需要的最低版本
cmake_minimum_required(VERSION 3.5.1)

# 项目名
project(DAPR C CXX)

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

# 构建 dapr 中的 common.proto
set(common_src_save_path "${CMAKE_CURRENT_SOURCE_DIR}/dapr/proto/common/v1")
file(MAKE_DIRECTORY ${common_src_save_path})
# common.proto
set(COMMON_PROTO_SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../dapr/proto/common/v1/common.proto")
get_filename_component(common_proto_name_we "${COMMON_PROTO_SOURCE_FILE}" NAME_WE)   # common
get_filename_component(common_proto_absolute "${COMMON_PROTO_SOURCE_FILE}" ABSOLUTE) # /root/git/cpp-sdk/dapr/proto/common/v1/common.proto
set(common_pbcc_proto_srcs "${common_src_save_path}/${common_proto_name_we}.pb.cc")
set(common_pbh_proto_hdrs "${common_src_save_path}/${common_proto_name_we}.pb.h")
# 构建并保存在 common_src_save_path
get_filename_component(common_proto_absolute_path "${common_proto_absolute}" PATH) # /root/git/cpp-sdk/dapr/proto/common/v1
add_custom_command(
      OUTPUT "${common_pbcc_proto_srcs}" "${common_pbh_proto_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --cpp_out "${CMAKE_CURRENT_SOURCE_DIR}"
        -I "${CMAKE_CURRENT_SOURCE_DIR}/../"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "dapr/proto/common/v1/common.proto"
      DEPENDS "${common_proto_absolute}"
      )

# 构建 dapr 中的 appcallback.proto 和 dapr.proto
set(runtime_src_save_path "${CMAKE_CURRENT_SOURCE_DIR}/dapr/proto/runtime/v1")
file(MAKE_DIRECTORY ${runtime_src_save_path})
# appcallback.proto
set(APP_PROTO_SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../dapr/proto/runtime/v1/appcallback.proto")
get_filename_component(app_proto_name_we "${APP_PROTO_SOURCE_FILE}" NAME_WE)
get_filename_component(app_proto_absolute "${APP_PROTO_SOURCE_FILE}" ABSOLUTE)
set(app_pbcc_proto_srcs "${runtime_src_save_path}/${app_proto_name_we}.pb.cc")
set(app_pbh_proto_hdrs "${runtime_src_save_path}/${app_proto_name_we}.pb.h")
set(app_pbcc_grpc_srcs "${runtime_src_save_path}/${app_proto_name_we}.grpc.pb.cc")
set(app_pbh_grpc_hdrs "${runtime_src_save_path}/${app_proto_name_we}.grpc.pb.h")
# dapr.proto
set(DAPR_PROTO_SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../dapr/proto/runtime/v1/dapr.proto")
get_filename_component(dapr_proto_name_we "${DAPR_PROTO_SOURCE_FILE}" NAME_WE)
get_filename_component(dapr_proto_absolute "${DAPR_PROTO_SOURCE_FILE}" ABSOLUTE)
set(dapr_pbcc_proto_srcs "${runtime_src_save_path}/${dapr_proto_name_we}.pb.cc")
set(dapr_pbh_proto_hdrs "${runtime_src_save_path}/${dapr_proto_name_we}.pb.h")
set(dapr_pbcc_grpc_srcs "${runtime_src_save_path}/${dapr_proto_name_we}.grpc.pb.cc")
set(dapr_pbh_grpc_hdrs "${runtime_src_save_path}/${dapr_proto_name_we}.grpc.pb.h")
# 构建并保存在 runtime_src_save_path
get_filename_component(runtime_proto_absolute_path "${app_proto_absolute}" PATH)
add_custom_command(
    OUTPUT "${app_pbcc_proto_srcs}" "${app_pbh_proto_hdrs}" "${app_pbcc_grpc_srcs}" "${app_pbh_grpc_hdrs}"
    "${dapr_pbcc_proto_srcs}" "${dapr_pbh_proto_hdrs}" "${dapr_pbcc_grpc_srcs}" "${dapr_pbh_grpc_hdrs}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${runtime_src_save_path}"
        --cpp_out "${runtime_src_save_path}"
        -I "${runtime_proto_absolute_path}"
        -I "${CMAKE_CURRENT_SOURCE_DIR}/../"
        -I "${CMAKE_CURRENT_SOURCE_DIR}/../dapr/proto/common/v1"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${app_proto_absolute}" "${dapr_proto_absolute}"
    DEPENDS "${app_proto_absolute}" "${dapr_proto_absolute}"
    )

# 头文件目录
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${common_src_save_path}")
include_directories("${runtime_src_save_path}")

# 查找当前 CMakeLists 目录下所有的源文件
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} DIR_LIB_SRCS)

# 设置库文件的生成路径为当前 CMakeLists 目录下的 lib/ 目录
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib")

# 添加名为 dapr_server 的库文件
add_library(
    dapr_server #SHARED
      ${DIR_LIB_SRCS}
      ${common_pbcc_proto_srcs}
      ${common_pbh_proto_hdrs}
      ${app_pbcc_proto_srcs}
      ${app_pbh_proto_hdrs}
      ${app_pbcc_grpc_srcs}
      ${app_pbh_grpc_hdrs}
      ${dapr_pbcc_proto_srcs}
      ${dapr_pbh_proto_hdrs}
      ${dapr_pbcc_grpc_srcs}
      ${dapr_pbh_grpc_hdrs}
      )

# 链接库文件
target_link_libraries(
    dapr_server
      ${_REFLECTION}
      ${_GRPC_GRPCPP}
      ${_PROTOBUF_LIBPROTOBUF}
      )