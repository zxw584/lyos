#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <lyos/const.h>
#include <lyos/type.h>
#include <lyos/ipc.h>
#include <lyos/service.h>

char * req_path = NULL;

static char * requests[] = {
	"up",
	NULL
};

static void die(char * name, char * reason)
{
	fprintf(stderr, "%s: %s\n", name, reason);
	exit(EXIT_FAILURE);
}

static void print_usage(char * name, char * reason)
{
	fprintf(stderr, "Error: %s\n", reason);
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "    %s up <server>\n", name);
	fprintf(stderr, "\n");
}

static int parse_cmd(int argc, char * argv[])
{
	int index = 1;
	int req_nr = 0;

	int req_index;
	if (argc < index + 1) {
		print_usage(argv[0], "too few arguments");
		exit(EXIT_FAILURE);
	}

	/* get request type */
	for (req_index = 0; ; req_index++) {
		if (requests[req_index] == NULL) break;
		if (strcmp(argv[index], requests[req_index]) == 0) break;
	}
	if (requests[req_index] == NULL) {
		print_usage(argv[0], "bad request type");
		exit(EXIT_FAILURE);
	}

	req_nr = SERVMAN_REQ_BASE + req_index;
	index++;

	if (argc < index + 1) {
		print_usage(argv[0], "no binary path specific");
		exit(EXIT_FAILURE);
	}
	req_path = argv[index];

	/* verify the name */
	if (req_path[0] != '/') die(argv[0], "absolute path required");
	struct stat stat_buf;
	if (stat(req_path, &stat_buf) != 0) die(argv[0], "binary stat failed");

	return req_nr;
}

int main(int argc, char * argv[])
{	
	int request = parse_cmd(argc, argv);
	
	MESSAGE msg;
	struct service_up_req up_req;
	memset(&msg, 0, sizeof(msg));
	msg.type = request;
	switch (request) {
	case SERVICE_UP:
		up_req.cmdline = req_path;
		up_req.cmdlen = strlen(req_path);
		msg.BUF = &up_req;
		break;
	}

	if (send_recv(BOTH, TASK_SERVMAN, &msg) != 0) die(argv[0], "cannot send request to servman");
	if (msg.RETVAL != 0) die(argv[0],  "servman reply error");
	
	return EXIT_SUCCESS;
}