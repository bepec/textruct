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

   BOOST_CHECK_EQUAL(SRule::fromString(s, "1 13", size), ResultOk);
   BOOST_CHECK_EQUAL(s.n1, 1);
   BOOST_CHECK_EQUAL(s.n2, 13);
   BOOST_CHECK_EQUAL(size, 4);
}

BOOST_AUTO_TEST_CASE( testOptionalNumber )
{
   Optional<int> on;
   typedef OptionalRule<int, NumberRule> OptionalNumberRule;
   size_t read;

   BOOST_CHECK_EQUAL(OptionalNumberRule::fromString(on, "13", read), ResultOk);
   BOOST_CHECK_EQUAL(on.present, true);
   BOOST_CHECK_EQUAL(on.content, 13);
   BOOST_CHECK_EQUAL(read, 2);

   BOOST_CHECK_EQUAL(OptionalNumberRule::fromString(on, "none", read), ResultOk);
   BOOST_CHECK_EQUAL(on.present, false);
   BOOST_CHECK_EQUAL(read, 4);
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

   BOOST_CHECK_EQUAL(StructWithOptionalRule::fromString(s, "19 86", read), ResultOk);
   BOOST_CHECK_EQUAL(s.n1, 19);
   BOOST_CHECK_EQUAL(s.n2.present, true);
   BOOST_CHECK_EQUAL(s.n2.content, 86);
   BOOST_CHECK_EQUAL(read, 5);

   BOOST_CHECK_EQUAL(StructWithOptionalRule::fromString(s, "19 none", read), ResultOk);
   BOOST_CHECK_EQUAL(s.n1, 19);
   BOOST_CHECK_EQUAL(s.n2.present, false);
   BOOST_CHECK_EQUAL(read, 7);
}

extern const std::string Hello("hello");

BOOST_AUTO_TEST_CASE( testStaticString )
{
   size_t read;
   typedef StaticStringRule<Hello> StaticStringHello;
   
   BOOST_CHECK_EQUAL(StaticStringHello::fromString("hello", read), ResultOk);
   BOOST_CHECK_EQUAL(read, 5);
   BOOST_CHECK_EQUAL(StaticStringHello::fromString("hell", read), ResultError);
}

BOOST_AUTO_TEST_CASE( testStructWithStaticText )
{
   typedef MemberSequenceRule<S,
      VoidMemberRule<S, StaticStringRule<Hello> >,
      MemberRule<S, int, &S::n1, NumberRule>
   > SRule;

   size_t read;
   S s;

   BOOST_CHECK_EQUAL(SRule::fromString(s, "hello 2", read), ResultOk);
   BOOST_CHECK_EQUAL(read, 7);
   BOOST_CHECK_EQUAL(s.n1, 2);

   BOOST_CHECK_EQUAL(SRule::fromString(s, "hello", read), ResultError);
   BOOST_CHECK_EQUAL(SRule::fromString(s, "hell 1", read), ResultError);
}

BOOST_AUTO_TEST_CASE( testArrayRule )
{
   size_t read;
   std::vector<int> array;
   typedef ArrayRule<int, NumberRule> NumberArrayRule;

   BOOST_CHECK_EQUAL(NumberArrayRule::fromString(array, "1, 2, 3", read), ResultOk);
   BOOST_CHECK_EQUAL(read, 7);
   BOOST_CHECK_EQUAL(array.size(), 3);
   BOOST_CHECK_EQUAL(array[0], 1);
   BOOST_CHECK_EQUAL(array[2], 3);
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

   BOOST_CHECK_EQUAL(StructWithArrayRule::fromString(s, "1 2, 3, 4", read), ResultOk);
   BOOST_CHECK_EQUAL(read, 9);
   BOOST_CHECK_EQUAL(s.n, 1);
   BOOST_CHECK_EQUAL(s.an.size(), 3);
   BOOST_CHECK_EQUAL(s.an[2], 4);
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

   BOOST_CHECK_EQUAL(NestedStructRule::fromString(s, "3 2 1", read), ResultOk);
   BOOST_CHECK_EQUAL(read, 5);
   BOOST_CHECK_EQUAL(s.n, 3);
   BOOST_CHECK_EQUAL(s.s.n1, 2);
   BOOST_CHECK_EQUAL(s.s.n2, 1);
}

typedef enum { One, Two, Three } E;
extern const std::string se[] = { "One", "Two", "Three" };

BOOST_AUTO_TEST_CASE( testEnum )
{
   typedef EnumRule<E, se, 3> ERule;

   E e;
   size_t read;

   BOOST_CHECK_EQUAL(ERule::fromString(e, "Two", read), ResultOk);
   BOOST_CHECK_EQUAL(read, 3);
   BOOST_CHECK_EQUAL(e, Two);

   BOOST_CHECK_EQUAL(ERule::fromString(e, "Three", read), ResultOk);
   BOOST_CHECK_EQUAL(read, 5);
   BOOST_CHECK_EQUAL(e, Three);

   BOOST_CHECK_EQUAL(ERule::fromString(e, "Four", read), ResultError);
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

   BOOST_CHECK_EQUAL(RtspAudioCodecsRule::fromString(codecs, "LPCM 00000003 01", read), ResultOk);
   BOOST_CHECK_EQUAL(read, 16);
   BOOST_CHECK_EQUAL(codecs.codecs.present, true);
   BOOST_CHECK_EQUAL(codecs.codecs.content.size(), 1);
   BOOST_CHECK_EQUAL(codecs.codecs.content[0].type, RtspAudioCodecs::LPCM);
   BOOST_CHECK_EQUAL(codecs.codecs.content[0].mask, 3);
   BOOST_CHECK_EQUAL(codecs.codecs.content[0].latency, 1);
}

