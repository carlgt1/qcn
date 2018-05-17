#ifndef _CSERIALIZE_H_
#define _CSERIALIZE_H_

class CSerialize
{
 public:
   CSerialize();
   ~CSerialize();  

   bool serialize(void* ptr, long len, const char* lpcszFile);
   bool deserialize(void* ptr, long len, const char* lpcszFile);
};

#endif

