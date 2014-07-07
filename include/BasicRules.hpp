#ifndef BASIC_RULES
#define BASIC_RULES

#include <string>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include "Types.hpp"


struct NumberRule
{
   static Result fromString(int& destination, const std::string& text, size_t& read)
   {
      const char *start = text.c_str();
      char *end;
      destination = strtol(start, &end, 10);
      read = end - start;
      return ResultOk;
   }
};


struct HexRule
{
   static Result fromString(unsigned& destination, const std::string& text, size_t& read)
   {
      const char *start = text.c_str();
      char *end;
      destination = strtol(start, &end, 16);
      read = end - start;
      return ResultOk;
   }
};


template <class ContentType, class ContentRule>
struct OptionalRule
{
   static Result fromString(Optional<ContentType>& destination, const std::string& text, size_t& read)
   {
      destination.present = true;
      if (text.size() >= 4 && text.substr(0, 4) == "none")
      {
         read = 4;
         destination.present = false;
         return ResultOk;
      }
      else return ContentRule::fromString(destination.content, text, read);
   }
};


template<const std::string& StringValue>
struct StaticStringRule
{
   static Result fromString(const std::string& text, size_t& read)
   {
      if (text.size() >= StringValue.length())
      {
         if (StringValue == text.substr(0, StringValue.length()))
         {
            read = StringValue.length();
            return ResultOk;
         }
      }
      return ResultError;
   }
};


template<class Type, class TypeRule>
struct ArrayRule
{
   static Result fromString(std::vector<Type>& destination, const std::string& text, size_t& read)
   {
      bool match = true;
      read = 0;
      while (match)
      {
         Type next;

         size_t memberRead = 0;
         std::string memberText = text.substr(read);
         match = (ResultOk == TypeRule::fromString(next, memberText, memberRead));
         if (match)
         {
            read += memberRead;
            destination.push_back(next);
            if (text.length() > read + 2 && text[read] == ',' && text[read+1] == ' ')
            {
               read += 2;
            }
            else
            {
               match = false;
            }
         }
      }

      return ResultOk;
   }
};


template<typename Enum, const std::string StringList[], size_t StringListSize>
struct EnumRule
{
   static Result fromString(Enum& destination, const std::string& text, size_t& read)
   {
      Result result = ResultError;
      for (size_t i = 0; result != ResultOk && i < StringListSize; i++)
      {
         if (text.substr(0, StringList[i].length()) == StringList[i])
         {
            destination = static_cast<Enum>(i);
            result = ResultOk;
            read = StringList[i].size();
         }
      }
      return result;
   }
};

#endif
