#pragma once

#ifdef HAVE_FSYNC
# undef HAVE_FSYNC
#endif

#ifndef P4CLIENTAPI_H
# define P4CLIENTAPI_H
# pragma warning (disable : 4267 4244)
# include "p4/clientapi.h"
# pragma warning (default : 4267 4244)
# pragma comment (lib, "libclient.lib")
# pragma comment (lib, "libsupp.lib")
# pragma comment (lib, "librpc.lib")
#endif

#ifndef HAVE_FSYNC
# define HAVE_FSYNC
#endif
