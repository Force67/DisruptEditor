#pragma once

#include "Serialization.h"

template <typename A1, typename A2>
struct Pair {
	A1 first;
	A2 second;
	void registerMembers(MemberStructure &ms);
};

template<typename A1, typename A2>
inline void Pair<A1, A2>::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(first);
	REGISTER_MEMBER(second);
}
