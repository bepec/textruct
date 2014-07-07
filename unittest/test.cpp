#define BOOST_TEST_MODULE TestRules

#include <boost/test/unit_test.hpp>
#include "ObjectParser.hpp"

struct S
{
   int n1;
   int n2;
};

BOOST_AUTO_TEST_CASE( testStructNumber )
{
   typedef MemberSequenceRule<S,
      MemberRule<S, int, &S::n1, NumberRule>,
      MemberRule<S, int, &S::n2, NumberRule>
   > SRule;

   S s;
   size_t size;

   BOOST_CHECK(ResultOk == SRule::fromString(s, "1 13", size));
   BOOST_CHECK(1 == s.n1);
   BOOST_CHECK(13 == s.n2);
   BOOST_CHECK(4 == size);
}

BOOST_AUTO_TEST_CASE( testOptionalNumber )
{
   Optional<int> on;
   typedef OptionalRule<int, NumberRule> OptionalNumberRule;
   size_t read;

   BOOST_CHECK(ResultOk == OptionalNumberRule::fromString(on, "13", read));
   BOOST_CHECK(true == on.present);
   BOOST_CHECK(13 == on.content);
   BOOST_CHECK(2 == read);

   BOOST_CHECK(ResultOk == OptionalNumberRule::fromString(on, "none", read));
   BOOST_CHECK(false == on.present);
   BOOST_CHECK(4 == read);
}

struct StructWithOptional
{
   int n1;
   Optional<int> n2;
};

BOOST_AUTO_TEST_CASE( testStructWithOptionalNumber )
{
   typedef MemberSequenceRule<StructWithOptional,
      MemberRule<StructWithOptional, int, &StructWithOptional::n1, NumberRule>,
      MemberRule<StructWithOptional, Optional<int>, &StructWithOptional::n2, OptionalRule<int, NumberRule> >
   > StructWithOptionalRule;

   StructWithOptional s;
   size_t read;

   BOOST_CHECK(ResultOk == StructWithOptionalRule::fromString(s, "19 86", read));
   BOOST_CHECK(19 == s.n1);
   BOOST_CHECK(true == s.n2.present);
   BOOST_CHECK(86 == s.n2.content);
   BOOST_CHECK(5 == read);

   BOOST_CHECK(ResultOk == StructWithOptionalRule::fromString(s, "19 none", read));
   BOOST_CHECK(19 == s.n1);
   BOOST_CHECK(false == s.n2.present);
   BOOST_CHECK(7 == read);
}

extern const std::string Hello("hello");

BOOST_AUTO_TEST_CASE( testStaticString )
{
   size_t read;
   typedef StaticStringRule<Hello> StaticStringHello;
   
   BOOST_CHECK(ResultOk == StaticStringHello::fromString("hello", read));
   BOOST_CHECK(5 == read);
   BOOST_CHECK(ResultError == StaticStringHello::fromString("hell", read));
}

BOOST_AUTO_TEST_CASE( testStructWithStaticText )
{
   typedef MemberSequenceRule<S,
      VoidMemberRule<S, StaticStringRule<Hello> >,
      MemberRule<S, int, &S::n1, NumberRule>
   > SRule;

   size_t read;
   S s;

   BOOST_CHECK(ResultOk == SRule::fromString(s, "hello 2", read));
   BOOST_CHECK(7 == read);
   BOOST_CHECK(2 == s.n1);

   BOOST_CHECK(ResultError == SRule::fromString(s, "hello", read));
   BOOST_CHECK(ResultError == SRule::fromString(s, "hell 1", read));
}

BOOST_AUTO_TEST_CASE( testArrayRule )
{
   size_t read;
   std::vector<int> array;
   typedef ArrayRule<int, NumberRule> NumberArrayRule;

   BOOST_CHECK(ResultOk == NumberArrayRule::fromString(array, "1, 2, 3", read));
   BOOST_CHECK(7 == read);
   BOOST_CHECK(3 == array.size());
   BOOST_CHECK(1 == array[0]);
   BOOST_CHECK(3 == array[2]);
}

struct StructWithArray
{
   int n;
   std::vector<int> an;
};

BOOST_AUTO_TEST_CASE( testStructWithArrayParser )
{
   typedef MemberSequenceRule<StructWithArray,
      MemberRule<StructWithArray, int, &StructWithArray::n, NumberRule>,
      MemberRule<StructWithArray, std::vector<int>, &StructWithArray::an, ArrayRule<int, NumberRule> >
   > StructWithArrayRule;

   size_t read;
   StructWithArray s;

   BOOST_CHECK(ResultOk == StructWithArrayRule::fromString(s, "1 2, 3, 4", read));
   BOOST_CHECK(9 == read);
   BOOST_CHECK(1 == s.n);
   BOOST_CHECK(3 == s.an.size());
   BOOST_CHECK(4 == s.an[2]);
}

struct NestedStruct
{
   int n;
   S s;
};

BOOST_AUTO_TEST_CASE( testNestedStructParser )
{
   typedef MemberSequenceRule<NestedStruct,
      MemberRule<NestedStruct, int, &NestedStruct::n, NumberRule>,
      MemberRule<NestedStruct, S, &NestedStruct::s, MemberSequenceRule<S,
         MemberRule<S, int, &S::n1, NumberRule>,
         MemberRule<S, int, &S::n2, NumberRule>
   > > > NestedStructRule;

   NestedStruct s;
   size_t read;

   BOOST_CHECK(ResultOk == NestedStructRule::fromString(s, "3 2 1", read));
   BOOST_CHECK(5 == read);
   BOOST_CHECK(3 == s.n);
   BOOST_CHECK(2 == s.s.n1);
   BOOST_CHECK(1 == s.s.n2);
}

typedef enum { One, Two, Three } E;
extern const std::string se[] = { "One", "Two", "Three" };

BOOST_AUTO_TEST_CASE( testEnum )
{
   typedef EnumRule<E, se, 3> ERule;

   E e;
   size_t read;

   BOOST_CHECK(ResultOk == ERule::fromString(e, "Two", read));
   BOOST_CHECK(3 == read);
   BOOST_CHECK(Two == e);

   BOOST_CHECK(ResultOk == ERule::fromString(e, "Three", read));
   BOOST_CHECK(5 == read);
   BOOST_CHECK(Three == e);

   BOOST_CHECK(ResultError == ERule::fromString(e, "Four", read));
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

BOOST_AUTO_TEST_CASE( testRtspAudioCodecs )
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

   BOOST_CHECK(ResultOk == RtspAudioCodecsRule::fromString(codecs, "LPCM 00000003 01", read));
   BOOST_CHECK(16 == read);
   BOOST_CHECK(true == codecs.codecs.present);
   BOOST_CHECK(1 == codecs.codecs.content.size());
   BOOST_CHECK(RtspAudioCodecs::LPCM == codecs.codecs.content[0].type);
   BOOST_CHECK(3 == codecs.codecs.content[0].mask);
   BOOST_CHECK(1 == codecs.codecs.content[0].latency);
}

