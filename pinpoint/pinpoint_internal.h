#ifndef __PINPOINT_INTERNAL__H__
#define __PINPOINT_INTERNAL__H__


static const char *APPNAME = "my_application";
static const char *caller_app_name = "caller";
static const char *callee_app_name = "callee";
static const int SPAN_GRPC_VERSION = 1;
// This means that the request received by the server is from the user.
static const int ROOT_PARENT_ID = -1;

// according to pinpoint configuration.
static const std::string HOST_SPAN = "localhost:9993";
static const std::string HOST_STAT = "localhost:9991";

// utils
long int get_time();
std::string get_stime();
long int get_pid();
std::string get_spid();
std::string get_stid();

static const long int CLIENT_SPANID = 10000000;
static const long int SERVER_SPANID = 20000000;
static const long int UNIQ_SEQ = 929292;

#endif // __PINPOINT_INTERNAL__H__
