#ifndef RTSP_PARAMETERS
#define RTSP_PARAMETERS

#include <string>
#include <cstdlib>
#include <cstdio>
#include <vector>

typedef enum {
   ResultOk,
   ResultError
} Result;

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


template <class ContentType>
struct Optional
{
   bool present;
   ContentType content;
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

template<class Struct>
struct SequenceElementParser
{
   struct IMemberElement
   {
      virtual Result fromString(Struct& parent, const std::string& text, size_t& read)=0;
   };

   template<class Rule>
   struct BaseElement: public IMemberElement
   {
      BaseElement(SequenceElementParser& parent)
      {
         parent.registerMember(this);
      }

      Result fromString(Struct&, const std::string& text, size_t& read)
      {
         return Rule::fromString(text, read);
      }
   };

   template<typename Type, class Rule>
   struct MemberElement: public IMemberElement
   {
      Type Struct::* mpReference;

      MemberElement(SequenceElementParser& parent, Type Struct::* reference):
         mpReference(reference)
      {
         parent.registerMember(this);
      }

      Result fromString(Struct& parent, const std::string& text, size_t& read)
      {
         return Rule::fromString(parent.*mpReference, text, read);
      }
   };

   template<class Type, class TypeRule>
   struct OptionalMemberElement: public MemberElement<Optional<Type>, OptionalRule<Type, TypeRule> > 
   {
      OptionalMemberElement(SequenceElementParser& parent, Optional<Type> Struct::* reference):
         MemberElement<Optional<Type>, OptionalRule<Type, TypeRule> >(parent, reference) {}
   };

   std::vector< IMemberElement* > mMembers;
   
   void registerMember(IMemberElement* m)
   {
      mMembers.push_back(m);
   }

   SequenceElementParser() {}
   
   Result fromString(Struct& destination, const std::string& text, size_t& read)
   {
      read = 0;
      for (size_t i = 0; i < mMembers.size(); ++i)
      {
         size_t memberRead = 0;
         if (i > 0) read++;
         if (read > text.length())
            return ResultError;
         std::string memberText = text.substr(read);
         if (ResultOk != mMembers[i]->fromString(destination, memberText, memberRead))
            return ResultError;
         read += memberRead;
      }
      return ResultOk;
   }
};


#endif
