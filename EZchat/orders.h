#ifndef __ORDERS_H
#define __ORDERS_H

#ifndef __CLIENT_C
#define __CLIENT_C
#endif // !__CLIENT_C
#include "global.h"

int AnalyseOrder(char* buf, int Socket);
int JudgeOrder(char*buf, int flag1, int flag2, int Socket);
void Help(void);
void SignupC(int Socket);
void SigninC(int Socket);
void SignoutC(int Socket);
void Exit(int Socket);
void MyfriendsC(int Socket);
void PrivateChatC(int Socket);
void RecvDatabaseMsg(int Socket);
void CheckFriendC(int Socket);
void FindPasswordC(int Socket);
void ChangePasswordC(int Socket);
void AddFriendC(int Socket);
void DelFriendC(int Socket);
void BlockFriendC(int Socket);
void SpecialCareFriendC(int Socket);
void ChatHistoryC(int Socket);
void CreateGroupC(int Socket);
void DelGroupC(int Socket);
void JoinGroupC(int Socket);
void ExitGroupC(int Socket);
void SetAdminC(int Socket);
void KickOffC(int Socket);
void MyGroupsC(int Socket);
void MyGroupC(int Socket);
void GroupChatHistoryC(int Socket);
void* Recv(void* tempSocket);
void* Send(void* tempSocket);
void GroupChatC(int Socket);
void AcceptFileC(int Socket);
void TransmitFileC(int Socket);




#endif // !__ORDERS_H