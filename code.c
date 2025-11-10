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

void *faculty_thread(void *arg){
    while(1){
        sem_wait(&faculty_sem);
        pthread_mutex_lock(&chair_mutex);
        if(waiting_students==0){
            pthread_mutex_unlock(&chair_mutex);
            log_event("Faculty", "No students waiting. Goes back to sleep.", -1);
            continue;
        }

        int sid=chairs[next_help];
        next_help=(next_help + 1)%MAX_CHAIRS;
        waiting_students--;
        
        char msg[80];
        sprintf(msg, "Starts helping Student %d. Waiting left: %d", sid, waiting_students);
        log_event("Faculty",msg,-1);
        print_queue();

        pthread_mutex_unlock(&chair_mutex);

        simulate_work();

        sprintf(msg,"Finished helping Student %d",sid);
        log_event("Faculty",msg,-1);

        sem_post(&student_sem[sid]);
    }
    return NULL;
}

void *student_thread(void *arg){
    int id=*(int * )arg;
    while(1){
        simulate_work();
        log_event("Student","Needs help.",id);
        pthread_mutex_lock(&chair_mutex);
        if(waiting_students<MAX_CHAIRS){
            chairs[next_seat]=id;
            next_seat=(next_seat+1)%MAX_CHAIRS;
            waiting_students++;

            char msg[80];
            sprintf(msg,"Sits on chair. Waiting students : %d",waiting_students);
            log_event("Student",msg,id);

            sem_post(&faculty_sem);
            pthread_mutex_unlock(&chair_mutex);

            sem_wait(&student_sem[id]);
            log_event("Student","Got help. Back to programming.",id);
        }
        else{
            log_event("Student","NO chairs available. Goes back to programming.",id);
            pthread_mutex_unlock(&chair_mutex);
        }
    }
    return NULL;
}
