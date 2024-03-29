#include "lab6_utils.h"
#include "list.h"

#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


void send_msg(Message *m, void *socket)
{
    zmq_msg_t msg;
    zmq_msg_init(&msg);
    zmq_msg_init_size(&msg, sizeof(Message)); //check -1
    memcpy(zmq_msg_data(&msg), m, sizeof(Message)); //check NULL
    zmq_msg_send(&msg, socket, 0); //check -1
    zmq_msg_close(&msg);
}

void recv_msg(Message *m, void *socket)
{
    zmq_msg_t msg;
    zmq_msg_init(&msg);
    zmq_msg_recv(&msg, socket, 0); //check -1
    memcpy(m, zmq_msg_data(&msg), sizeof(Message));
    zmq_msg_close(&msg);
}

int *__z_function(char *str, int size)
{
    int *z = calloc(size, sizeof(int));
    //printf("%s", str);
    for (int i = 1, l = 0, r = 0; i < size; ++i) {
        if (i <= r) {
            z[i] = min(r - i + 1, z[i - l]);
        }
        while(i + z[i] < size && str[z[i]] == str[i + z[i]]) {
            ++z[i];
        }
        if (i + z[i] - 1 > r) {
            l = i, r = i + z[i] - 1;
        }
    }
    return z;
}

void print_array(int size, int *arr)
{
    for (int i = 0; i < size; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void print_part_of_str(char *str, int from, int to, char *res, int *len)
{
    for (int i = from; i <= to; ++i) {
        *len += sprintf(res + *len, "%c", str[i]);
    }
}

void find_substrings(Message *m)
{
    int all_size = strlen(m->str);
    int *z       = __z_function(m->str, all_size);
    char *res = (char *) malloc(sizeof(char) * MAXSIZE);
    memset(res, 0, sizeof(char) * MAXSIZE);
    for (int i = m->num, len = 0; i < all_size; ++i) {
        if (z[i] == m->num) {
            len += sprintf(res + len, "(%d): ", i - m->num);
            print_part_of_str(m->str, m->num + 1, i - 1, res, &len);
            len += sprintf(res + len, "%s", GREEN);
            print_part_of_str(m->str, i, i + m->num - 1, res, &len);
            len += sprintf(res + len, "%s", END_COL);
            print_part_of_str(m->str, i + m->num, all_size - 1, res, &len);
        }
    }
    free(z);
    memcpy(m->str, res, sizeof(char) * MAXSIZE);
}

void client_address_gen(int id, char *address)
{
	memset(address, 0, MIDSIZE);
	memcpy(address, ADDRESS_PATTERN, sizeof(ADDRESS_PATTERN));
	char s[MINSIZE];
	memset(s, 0, MINSIZE);
	sprintf(s, "%d", id);
	strcat(address, s);
}

int ping(int id)
{
	char id_str[MINSIZE];
	memset(id_str, 0, MINSIZE);
	sprintf(id_str, "%d", id);

	char inproc_address[MIDSIZE];
	memset(inproc_address, 0, MIDSIZE);
	memcpy(inproc_address, PING_PATTERN, sizeof(PING_PATTERN));
	strcat(inproc_address, id_str);

	char address[MIDSIZE];
	memset(address, 0, MIDSIZE);
	memcpy(address, ADDRESS_PATTERN, sizeof(ADDRESS_PATTERN));
	strcat(address, id_str);

	void *context = zmq_ctx_new();
	void *requester = zmq_socket(context, ZMQ_REQ);
	zmq_connect(requester, address);

	zmq_socket_monitor(requester, inproc_address, ZMQ_EVENT_CONNECTED | ZMQ_EVENT_CONNECT_RETRIED);
	void *pair_socket = zmq_socket(context, ZMQ_PAIR);
	zmq_connect(pair_socket, inproc_address);

	zmq_msg_t m;
	zmq_msg_init(&m);
	zmq_msg_recv(&m, pair_socket, 0);
	uint8_t* data = (uint8_t*)zmq_msg_data(&m);
    uint16_t event = *(uint16_t*)(data);

    zmq_close(requester);
    zmq_close(pair_socket);
    zmq_msg_close(&m);
    zmq_ctx_destroy(context);

    if (event == ZMQ_EVENT_CONNECT_RETRIED) {
    	return -1;
    } else {
    	return 1;
    }
}

int echo(List *childs, int id, void *socket) // вернет id узла, у которого в потомках есть искомый id
{
	for (int i = 0, sz = size(childs); i < sz; ++i) {
		int cur_id = list_get(childs, i);
		check_neg_one(cur_id, "echo error");
		if (cur_id == id) {
			return cur_id;
		}
		Message m = {"echo", 0, id, ""}; //id

		char address[MIDSIZE];
		client_address_gen(cur_id, address);
		check_neg_one(zmq_connect(socket, address), "zmq_connect error");
		send_msg(&m, socket);
		recv_msg(&m, socket);
		zmq_disconnect(socket, address);

		if (strcmp(m.cmd, "ok") == 0) {
			return cur_id;
		}
	}
	return -1;
}