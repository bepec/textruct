#include <cassert>
#include <cstdio>
#include <vector>
#include "ObjectParser.hpp"

struct S
{
   int n1;
   int n2;
};

void testStructNumber()
{
   typedef MemberSequenceRule<S,
      MemberRule<S, int, &S::n1, NumberRule>,
      MemberRule<S, int, &S::n2, NumberRule>
   > SRule;

   S s;
   size_t size;

   assert(ResultOk == SRule::fromString(s, "1 13", size));
   assert(1 == s.n1);
   assert(13 == s.n2);
   assert(4 == size);
}

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

void testStructWithOptionalNumber()
{
   typedef MemberSequenceRule<StructWithOptional,
      MemberRule<StructWithOptional, int, &StructWithOptional::n1, NumberRule>,
      MemberRule<StructWithOptional, Optional<int>, &StructWithOptional::n2, OptionalRule<int, NumberRule> >
   > StructWithOptionalRule;

   StructWithOptional s;
   size_t read;

   assert(ResultOk == StructWithOptionalRule::fromString(s, "19 86", read));
   assert(19 == s.n1);
   assert(true == s.n2.present);
   assert(86 == s.n2.content);
   assert(5 == read);

   assert(ResultOk == StructWithOptionalRule::fromString(s, "19 none", read));
   assert(19 == s.n1);
   assert(false == s.n2.present);
   assert(7 == read);
}

extern const std::string Hello("hello");

void testStaticString()
{
   size_t read;
   typedef StaticStringRule<Hello> StaticStringHello;
   
   assert(ResultOk == StaticStringHello::fromString("hello", read));
   assert(5 == read);
   assert(ResultError == StaticStringHello::fromString("hell", read));
}

void testStructWithStaticText()
{
   typedef MemberSequenceRule<S,
      VoidMemberRule<S, StaticStringRule<Hello> >,
      MemberRule<S, int, &S::n1, NumberRule>
   > SRule;

   size_t read;
   S s;

   assert(ResultOk == SRule::fromString(s, "hello 2", read));
   assert(7 == read);
   assert(2 == s.n1);

   assert(ResultError == SRule::fromString(s, "hello", read));
   assert(ResultError == SRule::fromString(s, "hell 1", read));
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

struct StructWithArray
{
   int n;
   std::vector<int> an;
};

void testStructWithArrayParser()
{
   typedef MemberSequenceRule<StructWithArray,
      MemberRule<StructWithArray, int, &StructWithArray::n, NumberRule>,
      MemberRule<StructWithArray, std::vector<int>, &StructWithArray::an, ArrayRule<int, NumberRule> >
   > StructWithArrayRule;

   size_t read;
   StructWithArray s;

   assert(ResultOk == StructWithArrayRule::fromString(s, "1 2, 3, 4", read));
   assert(9 == read);
   assert(1 == s.n);
   assert(3 == s.an.size());
   assert(4 == s.an[2]);
}

struct NestedStruct
{
   int n;
   S s;
};

void testNestedStructParser()
{
   typedef MemberSequenceRule<NestedStruct,
      MemberRule<NestedStruct, int, &NestedStruct::n, NumberRule>,
      MemberRule<NestedStruct, S, &NestedStruct::s, MemberSequenceRule<S,
         MemberRule<S, int, &S::n1, NumberRule>,
         MemberRule<S, int, &S::n2, NumberRule>
   > > > NestedStructRule;

   NestedStruct s;
   size_t read;

   assert(ResultOk == NestedStructRule::fromString(s, "3 2 1", read));
   assert(5 == read);
   assert(3 == s.n);
   assert(2 == s.s.n1);
   assert(1 == s.s.n2);
}

typedef enum { One, Two, Three } E;
extern const std::string se[] = { "One", "Two", "Three" };

void testEnum()
{
   typedef EnumRule<E, se, 3> ERule;

   E e;
   size_t read;

   assert(ResultOk == ERule::fromString(e, "Two", read));
   assert(3 == read);
   assert(Two == e);

   assert(ResultOk == ERule::fromString(e, "Three", read));
   assert(5 == read);
   assert(Three == e);

   assert(ResultError == ERule::fromString(e, "Four", read));
};

struct RtspAudioCodecs
{
   typedef enum { AAC, AC3, LPCM } AudioCodecType;
   static const std::string AudioCodecList[];

   struct Codec
   {
      AudioCodecType type;
      unsigned int mask;
      unsigned int latency;
   };

   Optional<std::vector<Codec> > codecs;
};

const std::string RtspAudioCodecs::AudioCodecList[] = { "AAC", "AC3", "LPCM" };

void testRtspAudioCodecs()
{
   typedef MemberSequenceRule<RtspAudioCodecs::Codec,
      MemberRule<RtspAudioCodecs::Codec,
                 RtspAudioCodecs::AudioCodecType,
                 &RtspAudioCodecs::Codec::type,
                 EnumRule<RtspAudioCodecs::AudioCodecType,
                          RtspAudioCodecs::AudioCodecList, 3> >,
      MemberSequenceRule<RtspAudioCodecs::Codec,
         MemberRule<RtspAudioCodecs::Codec, unsigned int, &RtspAudioCodecs::Codec::mask, HexRule>,
         MemberRule<RtspAudioCodecs::Codec, unsigned int, &RtspAudioCodecs::Codec::latency, HexRule>
   > > RtspAudioCodecsCodecRule; 

   typedef MemberRule<RtspAudioCodecs,
                      Optional<std::vector<RtspAudioCodecs::Codec> >,
                      &RtspAudioCodecs::codecs,
                      OptionalRule<std::vector<RtspAudioCodecs::Codec>,
                                   ArrayRule<RtspAudioCodecs::Codec,
                                             RtspAudioCodecsCodecRule>
   > > RtspAudioCodecsRule;
                        
   RtspAudioCodecs codecs;
   size_t read;

   assert(ResultOk == RtspAudioCodecsRule::fromString(codecs, "LPCM 00000003 01", read));
   assert(16 == read);
   assert(true == codecs.codecs.present);
   assert(1 == codecs.codecs.content.size());
   assert(RtspAudioCodecs::LPCM == codecs.codecs.content[0].type);
   assert(3 == codecs.codecs.content[0].mask);
   assert(1 == codecs.codecs.content[0].latency);
}

int main(int argc, char* argv[])
{
   testStructNumber();
   testOptionalNumber();
   testStructWithOptionalNumber();
   testStaticString();
   testStructWithStaticText();
   testArrayRule();
   testStructWithArrayParser();
   testNestedStructParser();
   testEnum();
   testRtspAudioCodecs();

   puts("ok");
}
