#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<time.h>

#define MAX_STUDENTS 5
#define MAX_CHAIRS 3

sem_t faculty_sem;
sem_t student_sem[MAX_STUDENTS];
pthread_mutex_t chair_mutex;

int waiting_students=0;
int chairs[MAX_STUDENTS];
int next_seat=0,next_help=0;