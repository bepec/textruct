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


template<typename Struct, typename... Args>
struct MemberSequenceRule;


template <typename Struct, typename FirstRule>
struct MemberSequenceRule<Struct, FirstRule>
{
   static Result fromString(Struct& destination,
                            const std::string& text,
                            size_t& read)
   {
      read = 0;

      if (ResultOk != FirstRule::fromString(destination, text, read))
         return ResultError;
      return ResultOk;
   }
};


template <typename Struct, typename FirstRule, typename... NextRules>
struct MemberSequenceRule<Struct, FirstRule, NextRules...>
{
   static Result fromString(Struct& destination,
                            const std::string& text,
                            size_t& read)
   {
      read = 0;

      if (ResultOk != FirstRule::fromString(destination, text, read))
         return ResultError;

      else if (text.length() > read && text[read] == ' ')
      {
         read++;
         size_t elementRead = 0; 
         std::string textRemain = text.substr(read);
         if (ResultOk == MemberSequenceRule<Struct, NextRules...>::fromString(destination, textRemain, elementRead))
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
