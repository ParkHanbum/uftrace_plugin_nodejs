#include <iostream>
#include <string>
#include <grpcpp/grpcpp.h>

#include "Span.grpc.pb.h"
#include "Service.grpc.pb.h"
#include "Cmd.grpc.pb.h"
#include "Stat.grpc.pb.h"
#include "ThreadDump.grpc.pb.h"
#include "pinpoint_internal.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientWriter;

using namespace std;

class Agent
{
	private:
		v1::PAgentInfo agent_info;
		string agent_name;
		string starttime;
		string host;
		string port;
		string pid;
		ClientContext context;
		unique_ptr<v1::Agent::Stub> stub;

	public:
		Agent(string host, string port, long int type);
		string gen_agentid(string host, string port, long int type);
		void send_agentinfo();
		string get_agentid();
		string get_starttime();
		string gen_spanid();
		string get_ip();
		string get_port();
};

class Span
{
	private:
		google::protobuf::Empty empty;
		shared_ptr<Channel> channel;
		unique_ptr<v1::Span::Stub> stub;
		unique_ptr<ClientWriter<v1::PSpanMessage>> writer;
		ClientContext context;
		long int starttime;
		string spanid;
		string parent_spanid;

		v1::PSpanMessage msg;
		v1::PSpan span;
		v1::PTransactionId transactionId;
		v1::PParentInfo parentInfo;
		v1::PAcceptEvent acceptEvent;
		v1::PMessageEvent msgEvent;
		v1::PNextEvent nextEvent;

		void prepare_span(long int clientid, long int parentid, long int seq);

	public:
		Span(Agent *agent);
		void send_span();
		void send_span(string clientid, string parentid);
		void send_span(long int clientid, long int parentid, long int seq);
		void set_rpc(string path);
		void set_context(string agentid);
		void set_parentAppname(string pAppname);
		void set_parentSpanid(string spanid);
		string get_parentSpanid();
		void connect();

		string get_spanid();
};
