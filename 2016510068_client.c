#include<stdio.h>
#include<string.h>    // for strlen
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h> // for inet_addr
#include<unistd.h>    // for write
#include<pthread.h>   // for threading, link with lpthread
#include <sys/types.h>

#define MAXCHAR  140    // message Max lenght

int sockfd = 0 ;
char nickname[20];
int exitp = 1;
//Prevent to send space
void message_handler(char *arr,int length ){
    for (int i = 0; i < length; i++)
    {
        if(arr[i]=='\n'){
            arr[i]='\0';
            break;
        }
    }
    
}
//----------------------------------------------
//User Take Message from Server
void message_reciever(){
    char msg[MAXCHAR];
    while (1)
    {
        int recieve = recv(sockfd,msg,MAXCHAR,0);
        if (recieve>0)
        {
            printf("%s",msg);
        }
        else if (recieve==0)
        {
            break;
        }
        memset(msg,'\0',sizeof(msg));
        
    }
    
}
//--------------------------------------------------------------
//User Send Message
void message_sender(){
    char msg[MAXCHAR];
    while (1)
    {
        fgets(msg,MAXCHAR,stdin);
        message_handler(msg,sizeof(msg));
        if(strstr(msg,"-exit")!=NULL){
            send(sockfd,"-quit",6,0);
            exitp=0;
            break;
        }
        else
        {
            send(sockfd,msg,strlen(msg),0);
        }
        memset(msg,0,sizeof(msg));
    }
}
//-----------------------------------------------------------
int main(){
    struct sockaddr_in serverAdr;
    int socket_desc;
    //server and socket settings 
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    serverAdr.sin_family=AF_INET;
    serverAdr.sin_addr.s_addr=INADDR_ANY;
    serverAdr.sin_port=htons(3205);
    inet_pton(AF_INET,"127.0.0.1",&serverAdr.sin_addr);
    //-------------------------------------------------------
    int connection ; 

          if((connection =connect(sockfd,(struct sockaddr *)&serverAdr,sizeof(serverAdr)))== -1){
            printf("Error Connection Fail \n");
            return 1 ;
        }
    
    printf("Welcome To DEUChatBox \n");
    //thread creations
    pthread_t message_sender_thread;
    pthread_t message_reciever_thread;

    if (pthread_create(&message_sender_thread,NULL,(void *)message_sender,NULL)!=0)
    {
        printf("Thread Error");
        return 1;
    }

    if (pthread_create(&message_reciever_thread,NULL,(void *)message_reciever,NULL)!=0)
    {
        printf("Thread Error");
        return 1;
    }
    //------------------------------------------------------------------------------------------
    //works until user exit
    while (1)
    {
        if(exitp == 0){
            break;
        }
    }
    //-------------------------------------------------------------------------------------------
    printf("See You \n");
    close(sockfd);
    return 1;
}