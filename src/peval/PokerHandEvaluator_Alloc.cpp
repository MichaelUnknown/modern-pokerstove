/**
 * Copyright (c) 2012 Andrew Prock. All rights reserved.
 * $Id: PokerHandEvaluator_Alloc.cpp 2649 2012-06-30 04:53:24Z prock $
 */
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <locale>
#include <peval/HoldemHandEvaluator.h>
#include <peval/StudHandEvaluator.h>
#include <peval/RazzHandEvaluator.h>
#include <peval/StudEightHandEvaluator.h>
#include <peval/OmahaHighHandEvaluator.h>
#include <peval/OmahaEightHandEvaluator.h>
#include <peval/DeuceToSevenHandEvaluator.h>
#include <peval/DrawHighHandEvaluator.h>
#include <peval/BadugiHandEvaluator.h>
//#include "LowballA5HandEvaluator.h"
//#include "ThreeCardPokerHandEvaluator.h"

#include <peval/UniversalHandEvaluator.h>

using namespace std;
using namespace pokerstove;

std::shared_ptr<PokerHandEvaluator> PokerHandEvaluator::alloc (const string & input)
{
  if (input.empty())
    return nullptr;
  string strid = input;
  const std::locale locale;
  std::transform(strid.begin(), strid.end(), strid.begin(),
                 [&locale](char c) { return std::tolower(c, locale); });
  std::shared_ptr<PokerHandEvaluator> ret = nullptr;
  switch (strid[0])
    {
    case 'h':		//     hold'em
      //ret = new UniversalHandEvaluator (2,2,3,5,0,&CardSet::evaluateHigh, NULL);
      ret = std::make_shared<HoldemHandEvaluator>();
      break;

    case 'k':		//     Kansas City lowball (2-7)
      ret = std::make_shared<UniversalHandEvaluator>(1,5,0,0,0,&CardSet::evaluateLow2to7, nullptr);
      break;

    case 'l':		//     lowball (A-5)
      ret = std::make_shared<UniversalHandEvaluator>(1,5,0,0,0,&CardSet::evaluateLowA5, nullptr);
      break;

    case '3':		//     three card poker
      ret = std::make_shared<UniversalHandEvaluator>(1,3,0,0,0,&CardSet::evaluate3CP, nullptr);
      break;

    case 'o':		//     omaha
    {
      if (strid == "o6" || strid == "o5") {
        ret = std::make_shared<UniversalHandEvaluator>(4,4,3,5,2,&CardSet::evaluateHigh, nullptr);
      } else if (strid == "o5/8" || strid == "o6/8") {
        ret = std::make_shared<UniversalHandEvaluator>(4,4,3,5,2, &CardSet::evaluateHigh, &CardSet::evaluate8LowA5);
      } else if (strid == "o") {
        ret = std::make_shared<OmahaHighHandEvaluator>();
      } else if (strid == "o/8") {
        ret = std::make_shared<OmahaEightHandEvaluator>();
      } else if (strid == "o8") {
        ret = std::make_shared<OmahaEightHandEvaluator>();
      } else if (strid == "omaha") {
        ret = std::make_shared<OmahaHighHandEvaluator>();
      } else if (strid == "omaha/8") {
        ret = std::make_shared<OmahaEightHandEvaluator>();
      }
    }
    break;

      //
      // For the draw games, we need to be able to evaluate 1 card to determine the
      // bring in on 3rd street

    case 'r':		//     razz
      //ret.reset (new UniversalHandEvaluator (1,7,0,0,0,&CardSet::evaluateLowA5, NULL));
      ret = std::make_shared<RazzHandEvaluator>();
      break;

    case 's':		//     stud
      //ret.reset (new UniversalHandEvaluator (1,7,0,0,0,&CardSet::evaluateHigh, NULL));
      ret = std::make_shared<StudHandEvaluator>();
      break;

    case 'q':		//     stud high/low no qualifier
      ret = std::make_shared<UniversalHandEvaluator>(1,7,0,0,0, &CardSet::evaluateHigh, &CardSet::evaluateLowA5);
      break;

    case 'd':		//     draw high
    case 'D':		//     draw high
      //ret.reset (new UniversalHandEvaluator (1,5,0,0,0,&CardSet::evaluateHigh, NULL));
      ret = std::make_shared<DrawHighHandEvaluator>();
      break;

    case 't':		//     triple draw lowball (2-7)
      //ret.reset (new UniversalHandEvaluator (1,5,0,0,0,&CardSet::evaluateLow2to7, NULL));
      ret = std::make_shared<DeuceToSevenHandEvaluator>();
      break;

    case 'T':		//     triple draw lowball (A-5)
      ret = std::make_shared<UniversalHandEvaluator>(1,5,0,0,0,&CardSet::evaluateLowA5, nullptr);
      break;

    case 'e':		//     stud/8
      //ret.reset (new UniversalHandEvaluator (1,7,0,0,0,&CardSet::evaluateHigh, &CardSet::evaluate8LowA5));
      ret = std::make_shared<StudEightHandEvaluator>();
      break;

    case 'b':		//     badugi
      //ret.reset (new UniversalHandEvaluator (0,52,0,0,0,&CardSet::evaluateBadugi,NULL));
      ret = std::make_shared<BadugiHandEvaluator>();
      break;

    default:
      return ret;
    }

  if (ret)
    ret->_subclassID = strid;

  return ret;
}
