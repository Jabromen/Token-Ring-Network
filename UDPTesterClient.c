#include "UDPLib.h"


int main(int argc, char** argv)
{
    char buffer[512];
    sendMessage(argv[1],(u_short)60002,buffer);
 
    
}

