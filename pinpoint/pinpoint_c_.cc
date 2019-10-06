#include "pinpoint.h"
#include "pinpoint_c_.h"

extern "C" {
	Agent *newAgent(const char *host, const char *port, long int type) 
	{
		return new Agent(host, port, type);
	}
	void agent_send_info(Agent *agent)
	{
		agent->send_agentinfo();
	}
	const char *agent_get_id(Agent *agent)
	{
		return strdup(agent->get_agentid().c_str());
	}
	const char *agent_get_starttime(Agent *agent)
	{
		return strdup(agent->get_starttime().c_str());
	}
	const char *agent_gen_spanid(Agent *agent)
	{
		return strdup(agent->gen_spanid().c_str());
	}
	const char *agent_get_ip(Agent *agent)
	{
		return strdup(agent->get_ip().c_str());
	}
	const char *agent_get_port(Agent *agent)
	{
		return strdup(agent->get_port().c_str());
	}

	Span *newSpan(Agent *agent)
	{
		return new Span(agent);
	}
	void span_send(Span *span)
	{
		span->send_span();
	}
	void span_send_id(Span *span, const char *clientid, const char *parentid)
	{
		span->send_span(clientid, parentid);
	}
	void span_send_idnseq(Span *span, long int clientid, long int parentid, long int seq)
	{
		span->send_span(clientid, parentid, seq);
	}
	void span_set_rpc(Span *span, const char *path)
	{
		span->set_rpc(path);
	}
	void span_set_context(Span *span, const char *agentid)
	{
		span->set_context(agentid);
	}
	void span_set_parentAppname(Span *span, const char *pAppname)
	{
		span->set_parentAppname(pAppname);
	}
	void span_set_parentSpanid(Span *span, const char *spanid)
	{
		span->set_parentSpanid(spanid);
	}
	const char *span_get_parentSpanid(Span *span)
	{
		return strdup(span->get_parentSpanid().c_str());
	}
	void span_connect(Span *span)
	{
		span->connect();
	}
	const char *span_get_spanid(Span *span)
	{
		std::string id = span->get_spanid();
		return strdup(id.c_str());
	}
}
