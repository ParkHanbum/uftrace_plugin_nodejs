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

Span::Span(Agent *agent)
{
	string agentid = agent->get_agentid();
	string agent_starttime = agent->get_starttime();
	set_context(agentid);
	connect();
	spanid = agent->gen_spanid();
	span.set_starttime(get_time());
	span.set_version(SPAN_GRPC_VERSION);
	span.set_servicetype(1000);
	span.set_applicationservicetype(1000);
	span.set_spanid(CLIENT_SPANID);
	span.set_parentspanid(ROOT_PARENT_ID);
	span.set_starttime(get_time());

	// set the parent info
	parentInfo.set_parentapplicationtype(1000);

	transactionId.set_agentid(agentid);
	transactionId.set_agentstarttime(stol(agent_starttime));
	transactionId.set_sequence(UNIQ_SEQ);

	acceptEvent.set_endpoint("endpoint");
	acceptEvent.set_remoteaddr("remoteaddr");
}

void Span::set_context(string agentid)
{
	context.AddMetadata("agentid", agentid);
	context.AddMetadata("applicationname", agentid);
	context.AddMetadata("starttime", get_stime());
}

void Span::set_parentSpanid(string spanid)
{
	parent_spanid = spanid;
}

void Span::set_parentAppname(string pAppname)
{
	parentInfo.set_parentapplicationname(pAppname);
}

void Span::set_rpc(string path)
{
	acceptEvent.set_rpc(path);
}

void Span::prepare_span(long int clientid, long int parentid, long int seq)
{
	span.set_elapsed(get_time() - span.starttime());
	span.set_spanid(clientid);
	span.set_parentspanid(parentid);
	transactionId.set_sequence(seq);

	v1::PSpanEvent *spanEvent = span.add_spanevent();
	spanEvent->set_sequence(seq);
	spanEvent->set_depth(1);
	spanEvent->set_startelapsed(111);
	spanEvent->set_endelapsed(222);

	nextEvent.mutable_messageevent()->CopyFrom(msgEvent);
	spanEvent->mutable_nextevent()->CopyFrom(nextEvent);
	acceptEvent.mutable_parentinfo()->CopyFrom(parentInfo);
	span.mutable_acceptevent()->CopyFrom(acceptEvent);
	span.mutable_transactionid()->CopyFrom(transactionId);

	// set span to spanmessage
	msg.mutable_span()->CopyFrom(span);
}

void Span::send_span()
{
	writer->Write(msg);
	fprintf(stderr, "write \n");
	writer->WritesDone();
	fprintf(stderr, "write 2\n");

	Status status = writer->Finish();
	fprintf(stderr, "write 3\n");


	if (!status.ok()) {
		status.error_message();
		status.error_code();
	}
}

void Span::send_span(string clientid, string parentid)
{
	prepare_span(stol(clientid), stol(parentid), UNIQ_SEQ);
	send_span();
}


void Span::send_span(long int clientid, long int parentid, long int seq)
{
	prepare_span(clientid, parentid, seq);
	send_span();
}

string Span::get_spanid()
{
	return spanid;
}

void Span::connect()
{
	channel = grpc::CreateChannel(HOST_SPAN, grpc::InsecureChannelCredentials());
	stub = v1::Span::NewStub(channel);
	writer = (stub->SendSpan(&context, &empty));
}
