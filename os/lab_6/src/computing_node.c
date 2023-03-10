#include "list.h"
#include "lab6_utils.h"

#include <zmq.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
	if (argc < 2) {
		perror("argc < 2");
		exit(EXIT_FAILURE);
	}
	int my_id = atoi(argv[1]);

	List *childs = list_create();
	check_null(childs, "list_create error");

	void *context = zmq_ctx_new();
    void *responder = zmq_socket(context, ZMQ_REP);

    char address[32];
	memset(address, 0, 32);
	memcpy(address, SERVER_PATTERN, sizeof(SERVER_PATTERN));
	strcat(address, argv[1]);

    check_neg_one(zmq_bind(responder, address), "zmq bind error");

    while(1) {
    	Message m;
    	recv_msg(&m, responder);

        if (strcmp(m.cmd, "create") == 0) {
        	if (m.num == my_id) {
        		if (list_find(childs, m.id) == 1 || my_id == m.id) {
					memcpy(&m.cmd, "bad", sizeof(char) * 10);
		        	send_msg(&m, responder);
				}
				else {
			    	int pid = fork();
					check_neg_one(pid, "fork error");

					if (pid == 0) {
						zmq_close(responder);
    					zmq_ctx_destroy(context);
    					list_destroy(childs);

						char id_str[8];
						memset(id_str, 0, 8);
						sprintf(id_str, "%d", m.id);
						char *argvc[] = {"computing_node", id_str, NULL};
						check_neg_one(execv("computing_node", argvc), "execv computing_node error");
					}
					sleep(1);
					
					memcpy(&m.cmd, "ok", sizeof(char) * 10);
					m.num = pid;
			        send_msg(&m, responder);

			        push_back(childs, m.id);
		    	}
	    	} else {
				if (list_find(childs, m.num) == 1 && ping(m.num) == 1) {
					Message m2 = {"create", m.num, m.id, ""};

					void *requester = zmq_socket(context, ZMQ_REQ);

					char address[32];
					client_address_gen(m2.num, address);
					check_neg_one(zmq_connect(requester, address), "zmq_connect error");
					send_msg(&m2, requester);
	        		recv_msg(&m2, requester);
	        		zmq_close(requester);

	        		send_msg(&m2, responder);
				} else {
					void *requester = zmq_socket(context, ZMQ_REQ);

					int cur_id = echo(childs, m.num, requester);
					if (cur_id == -1 || ping(cur_id) == -1) {
						printf("bad_id\n");
						memcpy(&m.cmd, "bad", sizeof(char) * 10);
						send_msg(&m, responder);
						continue;
					}
					Message m2 = {"create", m.num, m.id, ""};

					char address[32];
					client_address_gen(cur_id, address);
					check_neg_one(zmq_connect(requester, address), "zmq_connect error");
					send_msg(&m2, requester);
	        		recv_msg(&m2, requester);
	        		zmq_close(requester);

	        		send_msg(&m2, responder);
	    		}
	    	}
        } else if (strcmp(m.cmd, "exec") == 0) {
        	if (m.id == my_id) {
	        	find_substrings(&m);
	        	if (strlen(m.str) == 0) {
	        		memcpy(&m.cmd, "0 matches", sizeof(char) * 10);
	        		send_msg(&m, responder);
	        	} else {
	        		memcpy(&m.cmd, "ok", sizeof(char) * 10);
	        		send_msg(&m, responder);
	        	}

	        } else if (list_find(childs, m.id) == 1 && ping(m.id) == 1) {
	        	void *requester = zmq_socket(context, ZMQ_REQ);
	        	char address[32];
				client_address_gen(m.id, address);
				check_neg_one(zmq_connect(requester, address), "zmq_connect error");
				send_msg(&m, requester);
        		recv_msg(&m, requester);
        		zmq_close(requester);

        		send_msg(&m, responder);
	        } else {
	        	void *requester = zmq_socket(context, ZMQ_REQ);

				int cur_id = echo(childs, m.id, requester);
				if (cur_id == -1 || ping(cur_id) == -1) {
					printf("bad_id\n");
					memcpy(&m.cmd, "bad", sizeof(char) * 10);
					send_msg(&m, responder);
					continue;
				}
				char address[32];
				client_address_gen(cur_id, address);
				check_neg_one(zmq_connect(requester, address), "zmq_connect error");
				send_msg(&m, requester);
        		recv_msg(&m, requester);
        		zmq_close(requester);

        		send_msg(&m, responder);
	        }
        } else if (strcmp(m.cmd, "remove") == 0) {
        	if (m.id == my_id) {
        		for (int i = 0, sz = size(childs); i < sz; ++i) {
					int cur_id = list_get(childs, i);
					check_neg_one(cur_id, "get error");

					if (ping(cur_id) == 1) {
						void *requester = zmq_socket(context, ZMQ_REQ);

						Message m2 = {"remove", 0, cur_id, ""};

						char address[32];
						client_address_gen(cur_id, address);
						check_neg_one(zmq_connect(requester, address), "zmq_connect error");
						send_msg(&m2, requester);
		        		recv_msg(&m2, requester);
		        		zmq_close(requester);
	        		}
				}

        		memcpy(&m.cmd, "ok", sizeof(char) * 10);
				m.num = getpid();
            	send_msg(&m, responder);
            	break;
        	} else if (list_find(childs, m.id) == 1 && ping(m.id) == 1) {
        		Message m2 = {"remove", 0, m.id, ""};

				void *requester = zmq_socket(context, ZMQ_REQ);

				char address[32];
				client_address_gen(m2.id, address);
				check_neg_one(zmq_connect(requester, address), "zmq_connect error");
				send_msg(&m2, requester);
        		recv_msg(&m2, requester);
        		zmq_close(requester);

        		wait(NULL);
        		list_delete(childs, m.id);

        		memcpy(&m2.cmd, "ok", sizeof(char) * 10);
        		send_msg(&m2, responder);
        	} 
        	else {
        		void *requester = zmq_socket(context, ZMQ_REQ);

        		int cur_id = echo(childs, m.id, requester);
                if (cur_id == -1 || ping(cur_id) == -1) {
                    memcpy(&m.cmd, "bad", sizeof(char) * 10);
            		send_msg(&m, responder);
                    continue;
                }

                char address[32];
                client_address_gen(cur_id, address);
                check_neg_one(zmq_connect(requester, address), "zmq_connect error");
                send_msg(&m, requester);
                recv_msg(&m, requester);
                zmq_close(requester);

                memcpy(&m.cmd, "ok", sizeof(char) * 10);
            	send_msg(&m, responder);
        	}
        } else if (strcmp(m.cmd, "echo") == 0) {
        	if ((list_find(childs, m.id) == 1 && ping(m.id) == 1) || my_id == m.id) {
        		memcpy(&m.cmd, "ok", sizeof(char) * 10);
            	send_msg(&m, responder);
        	} else {
        		void *requester = zmq_socket(context, ZMQ_REQ);
        		int cur_id = echo(childs, m.id, requester);
        		zmq_close(requester);

        		if (cur_id == -1 || ping(cur_id) == -1) {
        			memcpy(&m.cmd, "bad", sizeof(char) * 10);
            		send_msg(&m, responder);
        		} else {
        			memcpy(&m.cmd, "ok", sizeof(char) * 10);
            		send_msg(&m, responder);
        		}
        	}
        } else if (strcmp(m.cmd, "heartbit") == 0) {
        	int delay = m.num;
        	void *requester = zmq_socket(context, ZMQ_REQ);
            for (int i = 0, sz = size(childs); i < sz; ++i) {
            	int cur_id = list_get(childs, i);
            	check_neg_one(cur_id, "get error");

            	int answers = 0;
            	for (int j = 0; j < 4; ++j) {
            		if (ping(cur_id) == 1) {
            			answers += 1;
            		}
            		usleep(delay * 1000);
            	}				
				if (answers == 4) {
					printf("[%d]: ready\n", cur_id);

					Message m = {"heartbit", delay, 0, ""};

					char address[32];
					client_address_gen(cur_id, address);
					check_neg_one(zmq_connect(requester, address), "zmq_connect error");
					send_msg(&m, requester);
					recv_msg(&m, requester);
					zmq_disconnect(requester, address);
				} else {
					printf("[%d]: bad\n", cur_id);
				}
			}
			zmq_close(requester);
			memcpy(&m.cmd, "ok", sizeof(char) * 10);
            send_msg(&m, responder);
        }
	}

	zmq_close(responder);
    zmq_ctx_destroy(context);
    list_destroy(childs);

    printf("[%d] dying...\n", my_id);

	return 0;
}