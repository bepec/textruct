#ifndef OBJECT_PARSERS
#define OBJECT_PARSERS

#include <string>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include "BasicRules.hpp"


template<class Struct, class Type, Type Struct::* Ref, class Rule>
struct MemberRule
{
   static Result fromString(Struct& destination,
                            const std::string& text,
                            size_t& read)
   {
      return Rule::fromString(destination.*Ref, text, read);
   }
};

template<class Struct, class Rule>
struct VoidMemberRule
{
   static Result fromString(Struct& ,
                            const std::string& text,
                            size_t& read)
   {
      return Rule::fromString(text, read);
   }
};

template<class Struct, class Rule, class NextRule>
struct MemberSequenceRule
{
   static Result fromString(Struct& destination,
                            const std::string& text,
                            size_t& read)
   {
      read = 0;
      if (ResultOk != Rule::fromString(destination, text, read))
         return ResultError;
      if (text.length() > read && text[read] == ' ')
      {
         read++;
         size_t elementRead = 0; 
         std::string textRemain = text.substr(read);
         if (ResultOk == NextRule::fromString(destination, textRemain, elementRead))
         {
            read += elementRead;
         }
         else return ResultError;
      }
      else return ResultError;
      return ResultOk;
   }
};

#endif
