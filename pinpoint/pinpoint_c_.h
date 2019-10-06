#ifndef __PINPOINT_C_H__
#define __PINPOINT_C_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Agent Agent;
Agent *newAgent(const char *, const char *, long int);
void agent_send_info(Agent *);
const char *agent_get_id(Agent *);
const char *agent_get_starttime(Agent *);
const char *agent_gen_spanid(Agent *);
const char *agent_get_ip(Agent *);
const char *agent_get_port(Agent *);


typedef struct Span Span;
Span *newSpan(Agent *agent);
void span_send(Span *);
void span_send_id(Span *, const char *clientid, const char *parentid);
void span_send_idnseq(Span *, long int clientid, long int parentid, long int seq);
void span_set_rpc(Span *, const char *path);
void span_set_context(Span *, const char *agentid);
void span_set_parentAppname(Span *, const char *pAppname);
void span_set_parentSpanid(Span *, const char *spanid);
const char *span_get_parentSpanid(Span *);
void span_connect(Span *);
const char *span_get_spanid(Span *);


#ifdef __cplusplus
}
#endif
#endif
