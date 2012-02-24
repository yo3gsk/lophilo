#include "client.h"
#include "netlink.h"
#include <unistd.h>

#define EMPTY_ARG NULL

static struct nl_sock *sock;
static struct nl_msg *msg;
static int family;

static int counter = 0;
static int group;

update_function_t update_function;

static int handler(struct nl_msg *msg, void *arg)
{
	struct nlmsghdr *nlh = nlmsg_hdr(msg);
	struct nlattr *attrs[LOPHILO_U_MAX+1];

	printf("Received message!\n");

	genlmsg_parse(nlh, 0, attrs, LOPHILO_U_MAX, lophilo_notify_policy);
	if (attrs[LOPHILO_U_SOURCE]) {
		uint32_t source = nla_get_u32(attrs[LOPHILO_U_SOURCE]);
		uint32_t value = nla_get_u32(attrs[LOPHILO_U_VALUE]);
		if(update_function)
			update_function(source, value);
	} else {
		 printf("Error, cannot find attribute!\n");
		 return NL_SKIP;
	}
	return NL_OK;
}

int lophilo_init(update_function_t uf)
{
	int rc;

	printf("client starting\n");
	sock = nl_socket_alloc();
	rc = genl_connect(sock);
	if(rc < 0) {
		printf("connection error\n");
		goto failure;
	}

	printf("LOOKUPS...\n");

	family = genl_ctrl_resolve(sock, LOPHILO_FAMILY_NAME);
	if(family < 0) {
		printf("could not resolve family %s", LOPHILO_FAMILY_NAME);
		goto close_socket;
	}
	printf("family type: 0x%x\n", family);

	group = genl_ctrl_resolve_grp(sock, LOPHILO_FAMILY_NAME, LOPHILO_MCGRP);
	if(group < 0) {
		printf("could not resolve group %s", LOPHILO_MCGRP);
		goto close_socket;
	}
	printf("mcgroup: 0x%x\n", group);

	printf("resetting\n");
	nl_close(sock);

	printf("CONNECTING...\n");
	printf("preparing socket\n");
	sock = nl_socket_alloc();
	// FIXME: NL_CB_VALID not working
	//nl_socket_modify_cb(sock, NL_CB_MSG_IN, NL_CB_CUSTOM, handler, EMPTY_ARG);
	nl_socket_modify_cb(sock, NL_CB_VALID, NL_CB_CUSTOM, handler, EMPTY_ARG);
	nl_join_groups(sock, group); // join group before connect
	nl_socket_disable_seq_check(sock); //we're going to get unsolicited messages
	nl_socket_recv_pktinfo(sock, 1); // for debugging...
	rc = genl_connect(sock);
	if(rc < 0) {
		printf("error connecting\n");
		goto failure;
	}

	update_function = uf;

	return 0;

close_socket:
	printf("closing socket...\n");
	nl_close(sock);
failure:
	printf("connection failed...\n");
	return 1;
}

int lophilo_register(enum lophilo_sources_types type, enum lophilo_sources source, int period) 
{
	int rc;
	if(!sock) {
		goto failure;
	}
	printf("creating message\n");
	msg = nlmsg_alloc();
	genlmsg_put(msg, NL_AUTO_PID, NL_AUTO_SEQ, family, 0, NLM_F_REQUEST,
	     LOPHILO_CMD_REGISTER, LOPHILO_VERSION_NR);
	printf("adding attributes\n");
	rc = 0;
	rc |= nla_put_u32(msg, LOPHILO_R_TYPE, type);
	rc |= nla_put_u32(msg, LOPHILO_R_SOURCE, source);
	rc |= nla_put_u32(msg, LOPHILO_R_PERIOD_MS, period);
	if(rc) {
		printf("error adding attributes\n");
		goto free_message;
	}

	printf("sending registration\n");
	//rc = nl_send_auto_complete(sock, msg);
	rc = nl_send(sock, msg);
	if(rc < 0) {
		printf("failed to send registration\n");
		goto free_message;
	}
	return 0;

free_message:
	nlmsg_free(msg);
failure:
	printf("exiting...\n");
	return 1;
}

int lophilo_loop(void) 
{
	printf("looping to read messages\n");
	while(sock) {
		//printf("?");
		if(nl_recvmsgs_default(sock)) {
			//printf("!");
		}
	}
	return 1;

}
