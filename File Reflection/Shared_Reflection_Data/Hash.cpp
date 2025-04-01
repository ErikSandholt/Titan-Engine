#include "Hash.h"

constexpr const uint32_t FNV_OFFSET_BASIS	= 2166136261u;
constexpr const uint32_t FNV_PRIME			= 16777619u;

uint32_t Fnv1a_Hash(const char* aString)
{
    uint32_t hash = FNV_OFFSET_BASIS;
    size_t size = strlen(aString);

    for (int i = 0; i < size; ++i)
    {
        hash ^= static_cast<uint8_t>(aString[i]);
        hash *= FNV_PRIME;
    }

    return hash;
}

std::string TextToLower(const std::string& aFuncName)
{
	std::string result;
	for (size_t i = 0; i < aFuncName.length(); ++i) {
		result += (char)std::tolower(aFuncName[i]);

		/* Check if current character is lowercase and the next */
		/* character is uppercase */
		if (std::islower(aFuncName[i]) &&
			(i + 1 < aFuncName.length()) &&
			std::isupper(aFuncName[i + 1]))
		{
			result += '_'; /*Add underscore*/
		}
	}

	return result;
}
