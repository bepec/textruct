#ifndef TYPES
#define TYPES


typedef enum {
   ResultOk,
   ResultError
} Result;


template <class ContentType>
struct Optional
{
   bool present;
   ContentType content;
};


#endif
