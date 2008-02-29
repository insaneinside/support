#include <support/net.hh>

using namespace spt;



Address::Address()
  : _M_addr(),
    _M_sr(),
    _M_set(false)
{}

Address::Address(const string& s)
  : _M_addr(),
    _M_sr(),
    _M_set(false)

{
  set(s);
}
  
// Address::Address(const string& s, const int p)
//   : _M_addr()
// {
//   set(s,p);
// }

bool
Address::set(const string& str)
{
  struct hostent* he;
  std::string s(str);

  // Ignore any auth information
  auto_ptr<std::vector<std::string>> tokm = explode(s, "@");

  if ( tokm->size() > 2 || tokm->size() < 1 )
    return false;

  if ( tokm->size() > 1 )
    s = tokm->at(1);

  _M_sr = s;

  auto_ptr<std::vector<std::string>> tokl = explode(s, ":");

  if ( tokl->size() > 2 )
    return false;
  else if ( tokl->size() == 2 )
    {
      _M_addr.in.sa_family = AF_INET;
      _M_addr.in.sa_port = strtoul(tokl->at(1));
      s = tokl->at(0);
    }

  he = gethostbyname(s);
  if ( !he )
    {
      iray::log(V_ERR, "Unknown host " + s);
      return false;
    }
  

}


/************************************************/

Socket::Socket()
  : _M_addr(), _M_fd(-1)
{}

Socket::Socket(const string& s)
  : _M_addr(parse_addr(s)), _M_fd(-1)
{}

Socket::Socket(const string& s, const unsigned int)
  : _M_addr(), _M_fd(-1)
{
  
}

