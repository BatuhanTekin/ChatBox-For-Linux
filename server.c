#include<stdio.h>
#include<string.h>    // for strlen
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h> // for inet_addr
#include<unistd.h>    // for write
#include<pthread.h>   // for threading, link with lpthread
#include <sys/types.h>

#define MAXROOM  20     // Number of rooms can be changed
#define CAPACITY  4     // Each room capacity
#define MAXCLIENT  100  // Server capacity
#define MAXCHAR  140    // message Max lenght
#define CloseRoom -1    //defines room is closed
#define OpenRoom 0      //defines room is opened
#define PRoom 1         //defines room is  private


int count=0;           // client count

//For information of cliet
typedef struct{
    struct sockaddr_in address;
    int sockfd;
    int uid;
    char nickname[20];
}client_inf;   

client_inf *clients[MAXCLIENT];
//------------------------------

//For information of Room
int rooms[MAXROOM][CAPACITY]; //rooms
char roomsPass[MAXROOM][10]; //rooms passwords max 10 char
char roomsName[MAXROOM][20]; //rooms passwords max 20 char
int roomC[MAXROOM]; // if -1 room closed if 0 room public else room private
//-----------------------------------------------------
//Add Client in array
void add_user(void *arg){
    for (int i = 0; i < MAXCLIENT; i++)
    {
        client_inf *usr = (client_inf *) arg;
        if(!clients[i]){
            usr->uid = i;
            clients[i] = usr;
            break;
        }
    }
    
}
//----------------------------------------------------
//Send message
void send_message(char *msg,int id){
    write(clients[id]->sockfd,msg,strlen(msg));
}
//----------------------------------------------------
//Send message to room
void send_message_room(char *msg,int room,int id){
    for (int i = 0; i < CAPACITY; i++)
    {
        if(rooms[room][i]!=-1 && rooms[room][i]!=id){
            send_message(msg,rooms[room][i]);
            send_message("\n",rooms[room][i]);
        }
    }
    
}
//----------------------------------------------------

//add users to rooms
void add_user_room(int room , void *arg){
    client_inf *usr = (client_inf *) arg;
    int placed = 0;
    char message[MAXCHAR];
    char msgWnick[MAXCHAR+20];
    if(roomC[room]!=CloseRoom){
        
    for (int i = 0; i < CAPACITY; i++)
    {
     if(rooms[room][i]==-1){
         rooms[room][i]=usr->uid;
         placed = 1 ;
         sprintf(message,"%s:You Placed Room:%s\n",usr->nickname,roomsName[room]);
         send_message(message,usr->uid);
         strcat(msgWnick,usr->nickname);
         strcat(msgWnick,": Has been joined \n");
         send_message_room(msgWnick,room,usr->uid);
         break;
     }   
    }
    }
    if(placed==0){
        send_message("ROOM IS FULL \n",usr->uid);
    }
}
//----------------------------------

//Room exit
void quit_room(int room , int id){

    for (int i = 0; i < CAPACITY; i++)
    {
       if(rooms[room][i]==id){
           rooms[room][i]=-1;
       }
    }
      
}
//----------------------------------

//Empty room
void empty_room(int room){
   roomC[room]=CloseRoom;
   memset(roomsName[room],'\0',sizeof(roomsName[room]));
   memset(roomsPass[room],'\0',sizeof(roomsPass[room]));
}
//----------------------------------

//Room Control if empty close
void check_rooms(){
    int roomcount = 0;
    for (int i = 0; i < MAXROOM; i++)
    {
        roomcount=0;
        for (int j = 0; j < CAPACITY; j++)
        {
            if(rooms[i][j]==-1){
                roomcount++;
            }
        }
        if (roomcount==CAPACITY){
            empty_room(i);
        }
    }
      
}
//----------------------------------

//Find room which is not created
int find_empty_room(){
    int emptyRoom = -1;
    for (int i = 0; i < MAXROOM; i++)
    {
        if(roomC[i]==CloseRoom){
            emptyRoom=i;
        }
    }
    return emptyRoom;
    
}
//----------------------------------
//Find user room if it is exist
int find_users_room(int id){
    int user_in_room = -1;
    for (int i = 0; i < MAXROOM; i++)
    {
        for (int j = 0; j < CAPACITY; j++)
        {
            if(rooms[i][j]==id){
            user_in_room=i;
            }
        }
        
    }
    return user_in_room;
    
}
//----------------------------------
//check room name if it is exist
int check_room_name(char *name){
    int exist = -1;
    for (int i = 0; i < MAXROOM; i++)
    {
        if(strcmp(name,roomsName[i])!=0){

        }
        else{
            exist = i;
        }
    }
    return exist;
    
}
//----------------------------------
//list all public rooms
void list_all_rooms(int id){
    for (int i = 0; i < MAXROOM ; i++)
    {
        if (roomC[i]==OpenRoom){
            send_message("Room Name :",id);
            send_message(roomsName[i],id);
            send_message(" Users:",id);
            for (int j = 0; j < CAPACITY; j++)
            {
                if(rooms[i][j]!=-1){
                    send_message(clients[rooms[i][j]]->nickname,id);
                    send_message("-",id);
                }
            }
            send_message("\n",id);
        }
    }
        
}
//----------------------------------


//client handling
void *client_connect(void *arg)
{
    char *ptr;
    char *sysmsg;
    char message[MAXCHAR];         // for  message
    char msgWnick[MAXCHAR+20];
    char name[20];
     int room;
     char delim[]=" ";
    count++;
    client_inf *usr = (client_inf *) arg;
        //Take User Nickname
        sysmsg="Please enter Nickname (2-20 char) \n";
        write(usr->sockfd,sysmsg,strlen(sysmsg));
        while(recv(usr->sockfd,name,20,0) <= 0 || strlen(name)<2||strlen(name)>=19){
            sysmsg="Fail Please enter again \n";
            write(usr->sockfd,sysmsg,strlen(sysmsg));
        }
        strcpy(usr->nickname,name);
        puts(usr->nickname);
        add_user(usr);
        //------------------------------------------------------------
        
    while (1)
    {   
        if(!(recv(usr->sockfd,message,MAXCHAR,0) <= 0 || strlen(message)<2||strlen(message)>=MAXCHAR)){
            if(strstr(message,"-create")){ //Create Room
                if((room = find_users_room(usr->uid))!=-1){
                  send_message("You are already in a room \n",usr->uid);
                 }
                else if((room = find_empty_room())==-1){
                    sysmsg="All rooms are created already \n";
                    write(usr->sockfd,sysmsg,strlen(sysmsg));
                }
                else{
                    
                    ptr = strtok(message,delim);
                    ptr = strtok(NULL,delim);
                    if(check_room_name(ptr)==-1){
                    roomC[room] = OpenRoom;
                    strcpy(roomsName[room],ptr);
                    add_user_room(room,usr);}
                    else{
                    sysmsg="Room name must be unique \n";
                    write(usr->sockfd,sysmsg,strlen(sysmsg));  
                    }
                }
            }
            else if(strstr(message,"-pcreate")){ //Create private room
                 if((room = find_users_room(usr->uid))!=-1){
                  send_message("You are already in a room \n",usr->uid);
                 }
                 else if((room = find_empty_room())==-1){
                    sysmsg="All rooms are created already \n";
                    write(usr->sockfd,sysmsg,strlen(sysmsg));
                }
                else{
                    ptr = strtok(message,delim);
                    ptr = strtok(NULL,delim);
                    if(check_room_name(ptr)==-1){
                    roomC[room] = PRoom;
                    strcpy(roomsName[room],ptr);
                    ptr = strtok(NULL,delim);
                    if(ptr==NULL){
                        roomC[room]=CloseRoom;
                        memset(roomsName[room],'\0',sizeof(roomsName[room]));
                        sysmsg="-pcreate (room name) (room password) \n";
                        write(usr->sockfd,sysmsg,strlen(sysmsg)); 
                    }
                    else{
                    strcpy(roomsPass[room],ptr);
                    add_user_room(room,usr);}}
                    else{
                    sysmsg="Room name must be unique \n";
                    write(usr->sockfd,sysmsg,strlen(sysmsg));  
                    }
                }
            }
            else if(strstr(message,"-whoami")){ // Send User Nickname
                sprintf(message,"%s: is Your Nickname \n",usr->nickname,roomsName[room]);
                write(usr->sockfd,message,strlen(message));
            }
            else if(strstr(message,"-list")){ //List Rooms
                if((room = find_users_room(usr->uid))!=-1){
                  send_message("You are already in a room \n",usr->uid);
                 }
                 else{
                list_all_rooms(usr->uid);}
            }
            else if(strstr(message,"-enter")){ //Enter Room
                    ptr = strtok(message,delim);
                    ptr = strtok(NULL,delim);
                    room = find_users_room(usr->uid);
                    if(ptr!=NULL){
                    if(room!=-1 && check_room_name(ptr)!=-1){ //if in a room exit then enter
                    quit_room(room,usr->uid);
                    }
                    room = check_room_name(ptr);
                    if(room!=-1){
                        if(roomC[room]==OpenRoom){
                            add_user_room(room,usr);
                               }
                        else if (roomC[room]==PRoom)
                        {
                            ptr = strtok(NULL,delim);
                            if(ptr == NULL){
                                send_message("it is private room need password \n",usr->uid);
                            }
                            else if(strcmp(ptr,roomsPass[room])!=0 ){
                                send_message("Wrong Pasword \n",usr->uid);
                            }
                            else {
                                add_user_room(room,usr);
                            }
                        }
                    }
                    else{
                        send_message("Room is not exist \n",usr->uid);
                    }
                    }
                    else{
                        send_message("Enter room name \n",usr->uid);
                    }
            }
            else if(strstr(message,"-quit")){ //Quit Room
              room = find_users_room(usr->uid);
              if(room==-1){
                  send_message("You are not in a room \n",usr->uid);
              }
              else
              {
                  quit_room(room,usr->uid);
                  strcat(msgWnick,name);
                  strcat(msgWnick,": left from your channel \n");
                  send_message_room(msgWnick,room,usr->uid);
                  send_message("You quit from your room \n",usr->uid);
              }
              
            }
            else if(strstr(message,"-msg")){ //Send message
                room = find_users_room(usr->uid);
                if(room==-1){
                  send_message("You are not in a room \n",usr->uid);
                }
                else{
                    for (int k = 0; k < sizeof(message)-4; k++)
                    {
                        message[k]=message[k+4];
                    }
                    strcat(msgWnick,name);
                    strcat(msgWnick,":");
                    strcat(msgWnick,message);
                    strcat(msgWnick,"\n");
                    send_message_room(msgWnick,room,usr->uid);
                }
            }
            else{
                send_message("There is no command like that \n",usr->uid);
            }
        }
        check_rooms();
        memset(message,'\0',sizeof(message));
        memset(message,'\0',sizeof(message));
    }
    

}
//-----------------------------------

int main (){
    int enterence = 0;
    int sock;
    pthread_t  tid;
    //create room and close them
    for (int i = 0; i < MAXROOM; i++)
    {
        roomC[i]=-1; 
    }
    //-------------------------------
    for (int i = 0; i < MAXROOM; i++)
    {
        for (int j = 0; j < CAPACITY; j++)
        {
            rooms[i][j]=-1;
        }
        
    }
    
    struct sockaddr_in serverAdr,clientAdr;
    int socket_desc;

    //create socket and check
    socket_desc = socket(AF_INET,SOCK_STREAM,0);
    if(socket_desc==-1){
        puts("Couldn't create socket!!");
        return 1;
    }
    //--------------------------------------------

    //server settings
    serverAdr.sin_family=AF_INET;
    serverAdr.sin_addr.s_addr=INADDR_ANY;
    serverAdr.sin_port=htons(3205);
    //-----------------------------------------

    //create server and check
    if(bind(socket_desc,(struct sockaddr*)&serverAdr,sizeof(serverAdr))<0){
        puts("Binding fail");
        return 1;
    }
    listen(socket_desc,MAXCLIENT);
    puts("Waiting for incomming connection...\n");
    //----------------------------------------------------------------------------
    //User Connection
        while(1)
        {
        socklen_t clilen = sizeof(clientAdr);
        enterence=accept(socket_desc,(struct sockaddr*)&clientAdr,&clilen);
        if((count+1==MAXCLIENT)){
            puts("A connection failed");
            close(enterence);
            }
        else{
            client_inf *user = ((client_inf*)malloc(sizeof(client_inf)));
            user->sockfd = enterence;
            user->address = clientAdr;
            
            pthread_create(&tid,NULL,&client_connect,(void*)user);
        }

        }
    //-----------------------------------------------------------------------------
        return 0;
}

 
