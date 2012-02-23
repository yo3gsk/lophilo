#include "netlink.h"

struct nl_sock *sock;
struct nl_msg *msg;
int family;
static int parse_cb(struct nl_msg *msg, void *arg);

int main(void)
{
	// Allocate a new netlink socket
	sock = nl_socket_alloc();

	// Prepare socket to receive the answer by specifying the callback
	// function to be called for valid messages.
	printf("preparing socket\n");
	nl_socket_modify_cb(sock, NL_CB_VALID, NL_CB_CUSTOM, parse_cb, NULL);

	// Connect to generic netlink socket on kernel side
	genl_connect(sock);

	// Ask kernel to resolve family name to family id
	family = genl_ctrl_resolve(sock, LOPHILO_FAMILY_NAME);
	printf("family type: 0x%x\n", family);

	// family.id == group.id
	nl_join_groups(sock, family);

	printf("sending message\n");
	// Construct a generic netlink by allocating a new message, fill in
	// the header and append a simple integer attribute.
	msg = nlmsg_alloc();
	genlmsg_put(msg, NL_AUTO_PID, NL_AUTO_SEQ, family, 0, NLM_F_REQUEST,
	     LOPHILO_C_REGISTER, LOPHILO_VERSION_NR);
	nla_put_u32(msg, LOPHILO_R_TYPE, LOPHILO_TYPE_ADC);
	nla_put_u32(msg, LOPHILO_R_PERIOD_MS, 1000);
	nla_put_u32(msg, LOPHILO_R_SOURCE, LOPHILO_PIN_XA0);

	// Send message over netlink socket
	nl_send_auto_complete(sock, msg);
	// Free message
	nlmsg_free(msg);

	// Wait for the answer and receive it
	printf("waiting for messages\n");
	if(nl_recvmsgs_default(sock)) {
		printf("error in recvmsgs!\n");
	}
}

static int parse_cb(struct nl_msg *msg, void *arg)
{
	struct nlmsghdr *nlh = nlmsg_hdr(msg);
	struct nlattr *attrs[LOPHILO_U_MAX+1];

	printf("Received message!\n");
	// Validate message and parse attributes
	genlmsg_parse(nlh, 0, attrs, LOPHILO_U_MAX, lophilo_notify_policy);

	if (attrs[LOPHILO_U_SOURCE]) {
	 uint32_t value = nla_get_u32(attrs[LOPHILO_U_SOURCE]);
	 printf("source received: %d\n", value);
	} else {
	 printf("Error, cannot find attribute!\n");
	}

	return 0;
}
