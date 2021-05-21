#ifndef UUID_H
#define UUID_H

namespace uuid 
{
  typedef int uuid;

  namespace 
  {
    uuid next_id = 0;
  }

  static uuid generate()
  {
    return next_id++;
  }
};

#endif 