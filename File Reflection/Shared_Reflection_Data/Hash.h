#pragma once
#include <string>


std::string TextToLower(const std::string& aFuncName);
uint32_t Fnv1a_Hash(const char* aString);
inline uint32_t Fnv1a_Hash(const std::string& aString) { return Fnv1a_Hash(aString.c_str()); }
inline uint32_t Fnv1a_Hash_Lower(const std::string& aString) { return Fnv1a_Hash(TextToLower(aString).c_str()); }
