namespace cpp elasticmem.storage
namespace py elasticmem.kv

exception block_exception {
  1: string msg
}

exception chain_failure_exception {
  1: string msg
}

struct sequence_id {
  1: required i64 client_id,
  2: required i64 client_seq_no,
  3: required i64 server_seq_no,
}

service block_request_service {
  i64 get_client_id(),
  void register_client_id(1: i32 block_id, 2: i64 client_id),
  oneway void command_request(1: sequence_id seq, 2: i32 block_id, 3: i32 cmd_id, 4: list<string> arguments),
}

service block_response_service {
  oneway void response(1: sequence_id seq, 2: list<string> result),
}

service chain_request_service {
  list<string> run_command(1: i32 block_id, 2: i32 cmd_id, 3: list<string> arguments),
  oneway void chain_request(1: sequence_id seq, 2: i32 block_id, 3: i32 cmd_id, 4: list<string> arguments),
}

service chain_response_service {
  oneway void chain_ack(1: sequence_id seq),
}