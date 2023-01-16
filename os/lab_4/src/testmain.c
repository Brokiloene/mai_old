// 14 вариант Child1 переводит строки в нижний регистр. Child2 убирает все задвоенные пробелы.

#include <stdio.h>    // STDOUT_FILENO & size_t
#include <sys/mman.h> // mmap & shm_open
#include <sys/stat.h> // S_xxxx
#include <fcntl.h>    // O_xxxx   
#include <unistd.h>   // ftruncate      
#include <string.h>   // memcpy
#include <semaphore.h>// sem_t
#include <ctype.h>    // tolower
#include <sys/wait.h> // wait

#include "lab4_utils.h"
#define STOP -1


void print(char *str, size_t size)
{
	for (size_t i = 0; i < size; ++i) {
		printf("%c", str[i]);
	}
	printf("\n");
}

int main(int argc, char const *argv[])
{
	char *shmpath = "/shmlab4_12";
	int fd = shm_open(shmpath, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	check_err(fd, "shm_open error");

	check_err(ftruncate(fd, sizeof(Shared_str)), "ftruncate error");
	Shared_str *shr_str = mmap(NULL, sizeof(*shr_str), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (shr_str == MAP_FAILED) {check_err(-1, "mmap error")};

	check_err(sem_init(&shr_str->p_to_c1, 1, 0), "sem_init error");
	check_err(sem_init(&shr_str->c1_to_c2, 1, 0), "sem_init error");
	check_err(sem_init(&shr_str->c2_to_p, 1, 0), "sem_init error");

	int pid = fork();
	check_err(pid, "fork error");

	printf("forked\n");

	if (pid == 0) { // child1
		size_t end_flag = 0;
		printf("child running\n");
		while(1) {
			check_err(sem_wait(&shr_str->p_to_c1), "sem_wait error");
			if (shr_str->size == STOP) {
				printf("child stopping...\n");
				break;
			}
			printf("child got text %s\n", shr_str->str);
			for (int i = 0; i < shr_str->size; ++i) {
				shr_str->str[i] = tolower(shr_str->str[i]);
			}
			printf("child mapped text %s with size %d\n", shr_str->str, shr_str->size);
			check_err(sem_post(&shr_str->c1_to_c2), "sem_post error");
		}
		shm_unlink(shmpath);
		
	}
	else {

		int pid = fork();
		check_err(pid, "fork error");
		printf("forked\n");

		if (pid == 0) {
			while(1) {
				printf("child2 running\n");
				check_err(sem_wait(&shr_str->c1_to_c2), "sem_wait error");
				if (shr_str->size == STOP) {
					break;
				}
				printf("child2 got text %s\n", shr_str->str);
				for (int i = 0, end = shr_str->size - 1; i < end; i++) {
					if (shr_str->str[i] == ' ' && shr_str->str[i] == shr_str->str[i + 1]) {
						for (int j = i, end = shr_str->size - 1; j < end; j++) {
							shr_str->str[j] = shr_str->str[j + 1];
						}
					shr_str->size--;
					i--;
					}
				}
				printf("child2 mapped text %s with size %d\n", shr_str->str, shr_str->size);
				check_err(sem_post(&shr_str->c2_to_p), "sem_post error");
			}
		} else {
			size_t len = 16;
			int size = 0;
			char* text = NULL;
			size = getline(&text, &len, stdin);
			check_err(size, "getline error");
			text[size - 1] = '\0';
			while(text[0] != '0') {
				printf("got text %s\n", text);
				shr_str->size = size;
				memcpy(&shr_str->str, text, size);
				printf("mapped text %s with size %d\n", shr_str->str, shr_str->size);
				check_err(sem_post(&shr_str->p_to_c1), "sem_post error");
				printf("waiting...\n");
				check_err(sem_wait(&shr_str->c2_to_p), "sem_wait error");
				printf("size of result string is %d\n", shr_str->size);
				print(shr_str->str, shr_str->size);

		        size = getline(&text, &len, stdin);
				check_err(size, "getline error");
				text[size - 1] = '\0';
				printf("zero: %c\n", text[0]);
			}
			shr_str->size = STOP;
			check_err(sem_post(&shr_str->p_to_c1), "sem_post error");
			wait(NULL);
			wait(NULL);
			free(text);
			shm_unlink(shmpath);
		}
	}

	return 0;
}
