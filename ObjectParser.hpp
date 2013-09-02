#ifndef OBJECT_PARSERS
#define OBJECT_PARSERS

#include <string>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include "BasicRules.hpp"


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

   template<class Type, class TypeRule>
   struct ArrayMemberElement: public MemberElement<std::vector<Type>, ArrayRule<Type, TypeRule> > 
   {
      ArrayMemberElement(SequenceElementParser& parent, std::vector<Type> Struct::* reference):
         MemberElement<std::vector<Type>, ArrayRule<Type, TypeRule> >(parent, reference) {}
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
