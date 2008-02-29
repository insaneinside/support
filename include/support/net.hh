#ifndef SUPPORT_NET_HH
#define SUPPORT_NET_HH

#include <sys/socket.h>
#include <sys/un.h>
/* #include <sys/in.h> */
#include <string>

namespace spt
{
  namespace net
  {
    class Address
    {
    public:
      Address();
      Address(const std::string&);
      //Address(const std::string&, const int = -1);
      virtual ~Address() {}

      bool
      set(const std::string&);
      //bool set(const std::string&, const int = -1);

      std::string to_s() const;

    private:
      struct sockaddr* _M_sockaddr;
      socklen_t _M_socklen;
      std::string _M_sr;
      bool _M_set;
    };


    /** Interface to the low-level networking functions.
     */
    class Socket
    {
    public:
      Socket();
      Socket(const std::string&);
      Socket(const std::string&, const int = -1);
      ~Socket();

      enum
      ProtocolFamily
	{
	  LOCAL = PF_LOCAL,
	  INET = PF_INET
	};

      enum
      CommunicationStyle
	{
	  STREAM = SOCK_STREAM,
	  DGRAM = SOCK_DGRAM,
	  RAW = SOCK_RAW
	};
    

      typedef unsigned char sockdata_t;

      bool has_addr();

      bool connected();

      bool bind(const std::string&);

      bool bind(const std::string&, const int = 0);
    

    protected:
      Address _M_addr;
      int _M_fd;
    };
  }
}

#endif	/* SUPPORT_NET_HH */
