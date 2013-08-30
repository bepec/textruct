#include "RtspParameters.hpp"
#include <cassert>
#include <cstdio>
#include <vector>

struct S
{
   int n1;
   int n2;
};

NumberRule numberElementParser;

struct SParser: public SequenceElementParser<S>
{
   MemberElement<int, NumberRule> n1;
   MemberElement<int, NumberRule> n2;

   SParser(): n1(*this, &S::n1),
              n2(*this, &S::n2)
   {
   }
};

void testStructNumber()
{
   S s;
   SParser parser;

   size_t size;

   assert(ResultOk == parser.fromString(s, "1 13", size));
   assert(1 == s.n1);
   assert(13 == s.n2);
   assert(4 == size);
}

struct OptionalNumber
{
   bool present;
   int content;
};

void testOptionalNumber()
{
   Optional<int> on;
   typedef OptionalRule<int, NumberRule> OptionalNumberRule;
   size_t read;

   assert(ResultOk == OptionalNumberRule::fromString(on, "13", read));
   assert(true == on.present);
   assert(13 == on.content);
   assert(2 == read);

   assert(ResultOk == OptionalNumberRule::fromString(on, "none", read));
   assert(false == on.present);
   assert(4 == read);
}

struct StructWithOptional
{
   int n1;
   Optional<int> n2;
};

struct StructWithOptionalParser: public SequenceElementParser<StructWithOptional>
{
   MemberElement<int, NumberRule> n1;
   OptionalMemberElement<int, NumberRule> n2;
   
   StructWithOptionalParser():
      n1(*this, &StructWithOptional::n1),
      n2(*this, &StructWithOptional::n2)
   {
   }
};

void testStructWithOptionalNumber()
{
   StructWithOptionalParser parser;
   StructWithOptional s;
   size_t read;

   assert(ResultOk == parser.fromString(s, "19 86", read));
   assert(19 == s.n1);
   assert(true == s.n2.present);
   assert(86 == s.n2.content);
   assert(5 == read);

   assert(ResultOk == parser.fromString(s, "19 none", read));
   assert(19 == s.n1);
   assert(false == s.n2.present);
   assert(7 == read);
}

extern const std::string Hello("hello");
std::string NullString;

void testStaticString()
{
   size_t read;
   typedef StaticStringRule<Hello> StaticStringHello;
   
   assert(ResultOk == StaticStringHello::fromString("hello", read));
   assert(5 == read);
   assert(ResultError == StaticStringHello::fromString("hell", read));
}

struct ParserWithStaticText: public SequenceElementParser<S>
{
   BaseElement<StaticStringRule<Hello> > s;
   MemberElement<int, NumberRule> n;

   ParserWithStaticText():
      s(*this), n(*this, &S::n1) {}

};

void testStructWithStaticText()
{
   size_t read;
   S s;
   ParserWithStaticText parser;

   assert(ResultOk == parser.fromString(s, "hello 2", read));
   assert(7 == read);
   assert(2 == s.n1);

   assert(ResultError == parser.fromString(s, "hello", read));
   assert(ResultError == parser.fromString(s, "hell 1", read));
}

void testArrayRule()
{
   size_t read;
   std::vector<int> array;
   typedef ArrayRule<int, NumberRule> NumberArrayRule;

   assert(ResultOk == NumberArrayRule::fromString(array, "1, 2, 3", read));
   assert(7 == read);
   assert(3 == array.size());
   assert(1 == array[0]);
   assert(3 == array[2]);
}

int main(int argc, char* argv[])
{
   testStructNumber();
   testOptionalNumber();
   testStructWithOptionalNumber();
   testStaticString();
   testStructWithStaticText();
   testArrayRule();

   puts("ok");
}