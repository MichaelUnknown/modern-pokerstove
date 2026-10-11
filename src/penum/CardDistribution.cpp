/**
 * Copyright (c) 2012 Andrew Prock. All rights reserved.
 */
#include <penum/CardDistribution.h>

#include <charconv>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string_view>
#include <iostream>
#include <vector>
#include <peval/Card.h>
#include <util/combinations.h>

using namespace std;
using namespace pokerstove;

CardDistribution::CardDistribution()
    : _handList(1, CardSet())
    , _weights()
{
    _weights[CardSet()] = 1.0;
}

CardDistribution::CardDistribution(const CardSet& cs)
    : _handList(1, cs)
    , _weights()
{
    _weights[cs] = 1.0;
}

CardDistribution::CardDistribution(const CardDistribution& cd)
{
    *this = cd;
}

CardDistribution& CardDistribution::operator=(const CardDistribution& other)
{
    _handList = other._handList;
    _weights = other._weights;
    return *this;
}

size_t CardDistribution::size() const
{
    return _handList.size();
}

string CardDistribution::str() const
{
    ostringstream stream;
    stream << fixed << setprecision(3);
    for (size_t i = 0; i < _handList.size(); i++) {
        const CardSet& hand = _handList[i];
        stream << (i > 0 ? "," : "") << hand.str() << "=" << weight(hand);
    }
    return stream.str();
}

void CardDistribution::clear()
{
    _handList.clear();
    _weights.clear();
}

CardDistribution CardDistribution::data() const
{
    return *this;
}

void CardDistribution::fill(int n)
{
    CardSet cards;
    cards.fill();
    fill(cards, n);
}

void CardDistribution::fill(const CardSet& cs, int n)
{
    vector<Card> cards = cs.cards();
    int setsize = static_cast<int>(cards.size());
    combinations hands(setsize, n);
    int vsize = choose(setsize, n);
    clear();
    _handList.reserve(vsize);

    do {
        CardSet cs;
        for (int i = 0; i < n; i++)
            cs.insert(cards[hands[i]]);
        _handList.push_back(cs);
        _weights[cs] = 1.0;
    } while (hands.next());
}

const CardSet& CardDistribution::operator[](size_t index) const
{
    if (index >= _handList.size())
        throw std::runtime_error("CardDistribution::operator: bounds error");
    return _handList[index];
}

const double& CardDistribution::operator[](const CardSet& hand) const
{
    static const double kStaticZero = 0.0;  // for hands not in distribution
    map<CardSet, double>::const_iterator it = _weights.find(hand);
    if (it == _weights.end())
        return kStaticZero;
    return it->second;
}

double& CardDistribution::operator[](const CardSet& hand)
{
    return _weights[hand];
}

// this is a simple wrapper to make reading the code easier
const double& CardDistribution::weight(const CardSet& hand) const
{
    return (*this)[hand];
}

bool CardDistribution::parse(const std::string& input)
{
    CardDistribution parsed;
    if (input == ".") {
        _handList.swap(parsed._handList);
        _weights.swap(parsed._weights);
        return true;
    }
    parsed.clear();

    // Parse decimal/scientific weights without whitespace or locale dependence.
    // Preserve the optional leading '+' accepted by lexical_cast.
    std::string_view remaining(input);
    while (true) {
        const auto comma = remaining.find(',');
        const auto entry = remaining.substr(0, comma);
        const auto equals = entry.find('=');
        double weight = 1.0;
        if (equals != std::string_view::npos) {
            auto text = entry.substr(equals + 1);
            if (!text.empty() && text.front() == '+') {
                text.remove_prefix(1);
                if (!text.empty() && text.front() == '-')
                    return false;
            }
            if (text.empty())
                return false;
            const auto result = std::from_chars(text.data(), text.data() + text.size(),
                                                weight, std::chars_format::general);
            if (result.ec != std::errc{} || result.ptr != text.data() + text.size() ||
                !std::isfinite(weight) || weight < 0.0)
                return false;
        }

        const auto handstr = entry.substr(0, equals);
        if (handstr.empty() || handstr.size() % 2 != 0)
            return false;
        CardSet hand;
        for (size_t i = 0; i < handstr.size(); i += 2) {
            Card card;
            if (!card.fromString(std::string(handstr.substr(i, 2))) || hand.contains(card))
                return false;
            hand.insert(card);
        }
        parsed._weights[hand] = weight;
        parsed._handList.push_back(hand);
        if (comma == std::string_view::npos)
            break;
        remaining.remove_prefix(comma + 1);
    }
    _handList.swap(parsed._handList);
    _weights.swap(parsed._weights);
    return true;
}

std::string CardDistribution::display() const
{
    return str();
}

void CardDistribution::removeCards(const CardSet& dead)
{
    for (size_t i = 0; i < _handList.size(); i++)
        if (_handList[i].intersects(dead))
            _weights[_handList[i]] = 0.0;
}

double CardDistribution::weight() const
{
    double total = 0.0;
    for (std::pair<CardSet, double> w : _weights) {
        total += w.second;
    }
    return total;
}
