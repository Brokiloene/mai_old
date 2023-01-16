#include "list.h"
#include "lab6_utils.h"

#include <zmq.h>

#include <stdio.h>    //printf & scanf & getchar & ungetc & sprintf
#include <stdlib.h>   //malloc & free & exit & EXIT_FAILURE
#include <unistd.h>   //fork & execv & sleep & usleep
#include <string.h>   //strcmp & strcat & strlen & memcpy & memset
#include <sys/wait.h> //wait


int main(int argc, char const *argv[])
{
    void *context   = zmq_ctx_new();
    void *requester = zmq_socket(context, ZMQ_REQ);
    check_null(requester, "requester error");
    
    List *childs    = list_create();
    check_null(childs, "list_create error");

    char cmd[10]    = {'0'};
    int c;
    printf("> ");

    while((c = getchar()) != EOF) {
        ungetc(c, stdin);

        scanf("%s", cmd);

        if (strcmp(cmd, "create") == 0) {
            int id, parent_id;
            scanf("%d %d", &id, &parent_id);
            id += PORT;
            if (parent_id == -1) {
                if (list_find(childs, id) == 1 || echo(childs, id, requester) != -1) {
                    printf("bad child id\n"); 
                    continue;   
                }

                int pid = fork();
                check_neg_one(pid, "fork error");

                if (pid == 0) {
                	zmq_close(requester);
    				zmq_ctx_destroy(context);
    				list_destroy(childs);

                    char id_str[MINSIZE];
                    memset(id_str, 0, MINSIZE);
                    sprintf(id_str, "%d", id);
                    char *argvc[] = {"computing_node", id_str, NULL};
                    check_neg_one(execv("computing_node", argvc), "execv computing_node error");
                }

                sleep(1); // если создать/удалить/снова создать процесс, ping не сработает
                if (ping(id) == 1) {
	                push_back(childs, id);
	                printf("computing_node № %d with pid [%d] has been created\n", id, pid);
	            }
            } 

            else {
            	parent_id += PORT;
               
                if (list_find(childs, parent_id) == 1 && ping(parent_id) == 1) {
                    Message m = {"create", parent_id, id, ""};

                    char address[MIDSIZE];
                    client_address_gen(parent_id, address);
                    check_neg_one(zmq_connect(requester, address), "zmq_connect error");
                    send_msg(&m, requester);
                    recv_msg(&m, requester);
                    zmq_disconnect(requester, address);

                    if (strcmp(m.cmd, "bad") == 0) {
                        printf("%s: error with creating [%d] by [%d]\n", m.cmd, m.id, m.num);
                    } else {
                        printf("%s: [%d] was created by [%d]\n", m.cmd, m.id, parent_id);
                    }
                }
                else {
                    int cur_id = echo(childs, parent_id, requester);
                    if (cur_id == -1 || ping(cur_id) != 1) {
                        printf("bad id\n");
                        continue;
                    }

                    Message m = {"create", parent_id, id, ""};

                    char address[MIDSIZE];
                    client_address_gen(cur_id, address);
                    check_neg_one(zmq_connect(requester, address), "zmq_connect error");
                    send_msg(&m, requester);
                    recv_msg(&m, requester);
                    zmq_disconnect(requester, address);

                    if (strcmp(m.cmd, "bad") == 0) {
                        printf("%s: error with creating [%d] by [%d]\n", m.cmd, m.id, m.num);
                    }
                    else if (strcmp(m.cmd, "ok") == 0) {
                        printf("%s: [%d] was created by [%d] via [%d]\n", m.cmd, m.id, parent_id, cur_id);
                    }
                }
            }
        } else if (strcmp(cmd, "exec") == 0) {
            int id;
            scanf("%d", &id);
            id += PORT;
            getchar();                    // scanf сам не считывает '\n'

            char *pattern = (char *) malloc(sizeof(char) * MAXSIZE), *text = NULL;  
            size_t len = 0;
            int pattern_size = getline(&pattern, &len, stdin);
            int text_size    = getline(&text,    &len, stdin);

            pattern_size -= 1;         // getline выдает количество считанных символов
            pattern[pattern_size] = '#';  // " для char*, ' для char
            char *all = strcat(pattern, text);

            Message m = {"exec", pattern_size, id, ""};
            memset(m.str, 0, MAXSIZE);
            memcpy(m.str, all, strlen(all));

            if (list_find(childs, id) == 1 && ping(id) == 1) {
            	char address[MIDSIZE];
	            client_address_gen(id, address);
	            check_neg_one(zmq_connect(requester, address), "zmq_connect error");
	            send_msg(&m, requester);
	            recv_msg(&m, requester);
                zmq_disconnect(requester, address);

                if (strcmp(m.cmd, "ok") == 0) {
                    printf("%s:\n%s \nproduced by [%d]\n", m.cmd, m.str, m.id);
                } else if (strcmp(m.cmd, "0 matches") == 0) {
                    printf("%s\n", m.cmd);
                }
            } else {
            	void *echo_requester = zmq_socket(context, ZMQ_REQ);
            	int cur_id = echo(childs, id, echo_requester);
            	zmq_close(echo_requester);

                if (cur_id == -1 || ping(cur_id) == -1) {
                    printf("bad id\n");
                    free(pattern);
        			free(text);
                    continue;
                }

                char address[MIDSIZE];
                client_address_gen(cur_id, address);
                check_neg_one(zmq_connect(requester, address), "zmq_connect error");
                send_msg(&m, requester);
                recv_msg(&m, requester);
                zmq_disconnect(requester, address);

                if (strcmp(m.cmd, "bad") == 0) {
                    printf("bad id\n");
                } else if (strcmp(m.cmd, "ok") == 0) {
                    printf("%s:\n%s \nproduced by [%d]\n", m.cmd, m.str, m.id);
                } else if (strcmp(m.cmd, "0 matches") == 0) {
                    printf("%s\n", m.cmd);
                }
            }
        	free(pattern);
        	free(text);
        } else if (strcmp(cmd, "ping") == 0) {
            int id;
            scanf("%d", &id);
            id += PORT;
            printf("[%d]: %d\n", id, ping(id));
        } else if (strcmp(cmd, "remove") == 0) {
            int id;
            scanf("%d", &id);
            id += PORT;

            Message m = {"remove", 0, id, ""};

            if (list_find(childs, id) == 1 && ping(id) == 1) {
	            char address[MIDSIZE];
	            client_address_gen(id, address);
	            check_neg_one(zmq_connect(requester, address), "zmq_connect error");
	            send_msg(&m, requester);
	            recv_msg(&m, requester);
                zmq_disconnect(requester, address);

	            list_delete(childs, id);

	            printf("%s: [%d] with pid %d has been removed\n", m.cmd, m.id, m.num);
	            wait(NULL);  // а то будет зомби
	            
            } else {
            	int cur_id = echo(childs, id, requester);
                if (cur_id == -1 || ping(cur_id) == -1) {
                    printf("bad id\n");
                    continue;
                }

                char address[MIDSIZE];
                client_address_gen(cur_id, address);
                check_neg_one(zmq_connect(requester, address), "zmq_connect error");
                send_msg(&m, requester);
                recv_msg(&m, requester);
                zmq_disconnect(requester, address);

                if (strcmp(m.cmd, "bad") == 0) {
                        printf("%s: cannot remove %d", m.cmd, m.id);
                } else if (strcmp(m.cmd, "ok") == 0) {
                    printf("%s: [%d] with pid %d has been removed via [%d]\n", m.cmd, m.id, m.num, cur_id);
                }
            }
        }
        else if (strcmp(cmd, "heartbit") == 0) {
        	int delay;
            scanf("%d", &delay);

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

					char address[MIDSIZE];
					client_address_gen(cur_id, address);
					check_neg_one(zmq_connect(requester, address), "zmq_connect error");
					send_msg(&m, requester);
					recv_msg(&m, requester);
					zmq_disconnect(requester, address);
				} else {
					printf("[%d]: bad\n", cur_id);
				}
            }
        }

        printf("> ");
    }

    for (int i = 0, sz = size(childs); i < sz; ++i) {

		int cur_id = list_get(childs, i);
		check_neg_one(cur_id, "get error");

		if (ping(cur_id) == 1) {
			Message m = {"remove", 0, cur_id, ""};

			char address[MIDSIZE];
			client_address_gen(cur_id, address);
			check_neg_one(zmq_connect(requester, address), "zmq_connect error");
			send_msg(&m, requester);
			recv_msg(&m, requester);
			zmq_disconnect(requester, address);
		}
	}
    
    zmq_close(requester);
    zmq_ctx_destroy(context);
    list_destroy(childs);

    return 0;
}