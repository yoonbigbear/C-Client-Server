#include "user.h"

bool User::Initialize(Shared<ClientSession> session)
{
    tcp_ = session;

    return true;
}
