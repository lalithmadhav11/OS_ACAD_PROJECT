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

void *faculty_thread(void *arg);
void *student_thread(void *arg);
void  simulate_work();
void  print_queue();
void  log_event(const char *role,const char *msg,int id);

void log_event(const char *role,const char *msg,int id){
    time_t now=time(NULL);
    struct tm *t=localtime(&now);
    if(id==-1){
        printf("[%02d:%02d:%02d] [%s] %s\n", t->tm_hour, t->tm_min, t->tm_sec, role, msg);
    }
    else{
        printf("[%02d:%02d:%02d] [%s %d] %s\n", t->tm_hour, t->tm_min, t->tm_sec, role, id, msg);
    }
}

void simulate_work(){
    int t = rand()%3 +1 ;// -> 1 to 3 seconds
    sleep(t); 
}

void print_queue(){
    printf(" Chairs: ");
    if(waiting_students==0){
        printf("[ empty ]\n");
        return;
    }
    printf("[ ");
    for(int i=0;i<waiting_students;i++){
        int index=(next_help + i)% MAX_CHAIRS ;
        printf("S%d ",chairs[index]);
    }
    printf("]\n");
}
