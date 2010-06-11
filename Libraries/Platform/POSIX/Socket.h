#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netex/net.h>
#include <netex/ifctl.h>
#include <netex/errno.h>
#include <arpa/inet.h>

namespace Platform
{
  typedef int Socket;
}
