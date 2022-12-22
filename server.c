#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#define MAX 1024
  
struct message {
    long type;
    char text[MAX];
    char reciever[1024];
    char sender[1024];
} message;

void signalHandler(int dummy);  //when ctrl+c typed, queue will be destroyed
long hash(char* str);   //to hash the recievers id
void* recieveMessage(void* arg);    //worker threads' method
char * addAChar(char * str);

int msgid;
int main()
{
    key_t key = ftok(".", -1);
    msgid = msgget(key, 0666 | IPC_CREAT);
    signal(SIGINT, signalHandler);
    struct message recieved;
    
    while(1){
        msgrcv(msgid, &recieved, sizeof(message), 1, 0);    //1 is a special type of message
                                                        //it will be recieved when a new client has been created
        printf("New client detected: %s, worker thread is created\n" , recieved.sender);
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, recieveMessage, (void *)&(recieved.sender));
    }
    
    return 0;
}

void* recieveMessage(void* arg){
    char* type = (char *)arg;
    char* serverReciever = addAChar(type);  //client will send addedAchar versioned of id
    long hashed = hash(serverReciever);
    while(1){
        struct message recieved;
        if(msgrcv(msgid, &recieved, sizeof(message), hashed, 0)!=-1){
            printf("\nMessage recieved from %s: %s\n", recieved.sender,recieved.text);
            printf("Message will be sent to %s\n", recieved.reciever);
        }

        recieved.type = hash(recieved.reciever);
        msgsnd(msgid, &recieved, sizeof(recieved), 0);
    }
    return 0;
}

long hash(char* str){
    long returnValue=0;
    for(int i=0;i<strlen(str);i++){
        if(str[i]!='\n')
            returnValue += (str[i]-'0'+1)*pow(2,i);
    }
    return returnValue;
}

char * addAChar(char * str){
    char temp[strlen(str)+1];
    strcpy(temp,str);
    char a[2] = "0";
    return strcat(temp, a);
}

void signalHandler(int dummy){
    msgctl(msgid, IPC_RMID, NULL);  //destroys message queue
    exit(0);                        // and closes the program
}