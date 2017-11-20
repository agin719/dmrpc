
file(GLOB_RECURSE proto_files ${CMAKE_CURRENT_SOURCE_DIR}/*.proto)
set(protobuf_executable protoc)
set(proto_input_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(proto_output_dir ${CMAKE_CURRENT_SOURCE_DIR})

set(proto_src "")

foreach (proto_file ${proto_files})
    get_filename_component(proto_name ${proto_file} NAME_WE)

    set(output_file 
        ${proto_output_dir}/${proto_name}.pb.cc
        ${proto_output_dir}/${proto_name}.pb.h
    )
    list(APPEND proto_src ${output_file})
    add_custom_command(
        OUTPUT ${output_file}
        COMMAND ${protobuf_executable} ${proto_file} --cpp_out=${proto_output_dir} -I${proto_input_dir}
        DEPENDS ${proto_file}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endforeach()

#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} srcs)
#list(REMOVE_ITEM srcs echo_client.cc echo_server.cc)
set(srcs 
    net.cc 
    ${proto_src}
    rpc_server.cc
    my_rpc_channel.cc)

add_library(dmrpc ${srcs})
target_link_libraries(dmrpc protobuf glog cityhash)
set_target_properties(dmrpc PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

add_executable(echo_client echo_client.cc)
target_link_libraries(echo_client dmrpc protobuf glog) 

add_executable(echo_server echo_server.cc)
target_link_libraries(echo_server dmrpc protobuf glog) 
