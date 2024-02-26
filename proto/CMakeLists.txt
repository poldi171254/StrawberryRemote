INCLUDE(FindProtobuf)

FIND_PACKAGE(Protobuf REQUIRED)

include_directories(${Protobuf_INCLUDE_DIR})

protobuf_generate_cpp(PROTO_SRC PROTO_HEADER RemoteMessages.proto)

ADD_LIBRARY(Proto OBJECT
	${PROTO_HEADER} 
	${PROTO_SRC}
)

target_sources(Proto PRIVATE
    RemoteMessages.proto
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(Proto PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
