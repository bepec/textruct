#ifndef TYPES_H_
#define TYPES_H_


// Copyright 2014 Alex Vedmidsky


typedef enum {
  ResultOk,
  ResultError
} Result;


template <class ContentType>
struct Optional {
  bool present;
  ContentType content;
};


#endif  // TYPES_H_
