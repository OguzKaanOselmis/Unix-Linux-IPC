#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#define MAX 1024

struct message {
    long type;
    char text[MAX];
    char reciever[1024];    //reciever's client id
    char sender[1024];      //sender's client id
} message;

long hash(char* str);   //to hash the id of sender and give it to type
void sendMessage(int msgid);
void* recieveMessage(void *arguments);  //threads function to recieve a message at any time
char * addAChar(char * str);    //to change the id to send the message to server    

long hashedType;
int msgid;
struct message firstMsg;
struct message msg;

int main(int argc, char *argv[])
{
    key_t key;
    char input[10];
    if(argc==1){        // if no arguments given to client
        printf("No arguments given! Closing client.");
        return 1;
    }
    hashedType = hash(argv[1]);
    strcpy(firstMsg.sender,argv[1]);
    strcpy(msg.sender,argv[1]);

    key = ftok(".", -1);        //to create unique key
    msgid = msgget(key, 0666 | IPC_CREAT);
    firstMsg.type = 1;  //first message's type is always 1
                        // to let the server know that new client has been created
    msgsnd(msgid, &firstMsg, sizeof(firstMsg), 0);

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, recieveMessage, NULL);

    while(1){
        printf("Waiting for new messages\n");
        printf("To send message enter w\nTo exit enter q\n");
        fgets(input,10,stdin);
        input[strlen(input)-1] = '\0';
        if(strcmp(input,"w")==0){
            sendMessage(msgid);
        }else if(strcmp(input,"q")==0){
            return 0;
        }else{
            printf("Wrong Input!\n");
        }
    }
  
    return 0;
}

void sendMessage(int msgid){
    printf("Which client do you want to send: ");
    fgets(msg.reciever,1024,stdin);
    msg.reciever[strlen(msg.reciever)-1] = '\0';
    char* serverReciever = addAChar(msg.reciever);  // server will be listening addedAChar version of id
    msg.type = hash(serverReciever);    //after first message type will be static

    printf("Write the message: ");
    fgets(msg.text,MAX,stdin);
    msg.text[strlen(msg.text)-1] = '\0';

    msgsnd(msgid, &msg, sizeof(msg), 0);
}

void* recieveMessage(void* arguments){
    struct message recieved;
    while(1){
        if(msgrcv(msgid, &recieved, sizeof(message),hashedType, 0)!=-1)
            printf("Message recieved from %s: %s\n",recieved.sender,recieved.text);
    }
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