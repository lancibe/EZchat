#ifndef __REFLECTS_H
#define __REFLECTS_H

#include "global.h"

int Analyse(char* buf, int ClientSocket);
int Reflect(char*buf, int flag1, int flag2, int ClientSocket);
int Signup(int ClientSocket);
void Signin(int ClientSocket);
void Signout(int ClientSocket);
void Myfriends(int ClientSocket);
void PrivateChat(int ClientSocket);
void SendDatabaseMsg(int ClientSocket);
void CheckFriend(int ClientSocket);
void FindPassword(int ClientSocket);
void ChangePassword(int ClientSocket);
void ChangeFriend(int ClientSocket, char kind);
void ChatHistory(int ClientSocket);
void CreateGroup(int ClientSocket);
void DelGroup(int ClientSocket);
void JoinGroup(int ClientSocket);
void ExitGroup(int ClientSocket);
void SetAdmin(int ClientSocket);
int JudgeMember(int ClientSocket, MYSQL mysql, char* membercount, char* groupcount);
void KickOff(int ClientSocket);
void MyGroups(int ClientSocket);
void MyGroup(int ClientSocket);
void GroupChatHistory(int ClientSocket);
void * Send(int ClientSocket);
void * Recv(int ClientSocket);
void TransmitFile(int ClientSocket);
void AcceptFile(int ClientSocket);
void GroupChat(int ClientSocket);





#endif // !__REFLECTS_H