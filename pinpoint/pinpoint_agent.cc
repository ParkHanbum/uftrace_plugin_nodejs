#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <grpcpp/grpcpp.h>
#include "Span.grpc.pb.h"
#include "Service.grpc.pb.h"
#include "Cmd.grpc.pb.h"
#include "Stat.grpc.pb.h"
#include "ThreadDump.grpc.pb.h"
#include "pinpoint.h"
#include "pinpoint_internal.h"

using namespace std;

Agent::Agent(string host, string port, long int type) {
	agent_name = gen_agentid(host, port, type);
	starttime = get_stime();
	pid = get_spid();

	agent_info.set_hostname(agent_name);
	agent_info.set_ip(host);
	agent_info.set_ports(port);
	agent_info.set_servicetype(type);

	context.AddMetadata("agentid", agent_name);
	context.AddMetadata("applicationname", agent_name);
	context.AddMetadata("starttime", starttime);

	shared_ptr<Channel> channel = grpc::CreateChannel(HOST_STAT, grpc::InsecureChannelCredentials());
	stub = v1::Agent::NewStub(channel);
}

string Agent::gen_spanid()
{
	return string(pid + get_stime());
}

string Agent::get_ip()
{
	return agent_info.ip();
}

string Agent::get_port()
{
	return agent_info.ports();
}

string Agent::gen_agentid(string host, string port, long int type)
{
	string server_type = "UNKNOWN";
	switch (type) {
		case 1000L:
			server_type = "S";
			break;
	}
	
	// STAND-SERVERNAME-PORT
	//string res = string(server_type + get_stid() + "_" + host + "_" + port);
	string res = string(server_type + "_" + host + "_" + port);

	if (res.length() >= 24) {
		cout << "Agentid : " << res << " is too long. (24 character maximize). " << endl;
		exit(1);
	}

	return res;
}

void Agent::send_agentinfo()
{
	v1::PResult result;

	Status status = stub->RequestAgentInfo(&context, agent_info, &result);

	if (status.ok()) {
		cout << result.success() << result.message() << endl;
	} else {
		cout << status.error_code() << ": " << status.error_message() << endl;
	}
}

string Agent::get_starttime()
{
	return starttime;
}

string Agent::get_agentid()
{
	return agent_name;
}

