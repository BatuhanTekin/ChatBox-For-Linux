# deu-chat-BatuhanTekin
deu-chat-BatuhanTekin created by GitHub Classroom
Chat Box 

Goal

In this assignment, you will implement server side and client side socket programming.

Implementation Details & Requirements

People want to talk with others about specific subjects or different areas in the digital rooms
which are named as chat rooms. We will develop chat system to meet this need. In this
system, we must have a chat server and we may have more than a client.
The server must be up always for the system works. It executes commands which are taken
by clients and controls and manages the system. The server will be hosted on localhost
and its port is 3205. When the client connects successfully to the server, it will enter its
nickname initially. Then it is located in the common area such as lobby.

The commands are described in below can be used in the system:

● -list: Lists the currently available rooms with the name of the customers in it. If the
room is private, no details should be given.

● -create room_name: Creates a new specified room. Not more than one room with
the same name.

● -pcreate room_name: Creates a new specified private room. This type of room has
been protected with password. The system will ask to define a password.

● -enter room_name: Enter to the specified room. If the room is private, the client
must know the password for enter.

● -quit room_name: Quit from the room that you are in. You come back to the
common area.

● -msg message_body: Sends a message to room that you are in. Your and the
others' messages should look different in the room.

● -whoami: Shows your own nickname information.

● -exit: Exit the program.

Client can create room and it cannot be in more than one room at a time, but it can enter and
exit different rooms at any time. Clients in the same room can see all posts.
