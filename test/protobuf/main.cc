#include <iostream>
#include "test.pb.h"
using namespace fixbug;

int main(){
    LoginRequest req;
    req.set_name("lee");
    req.set_pwd("123456");

    return 0;
}