#pragma once
#include <typeinfo>

class BitMask
{
private:
	unsigned int myMask;

public:
	BitMask() = default;
	BitMask(unsigned int aMask) : myMask(aMask) {}

	unsigned int GetMask() const			{ return myMask; }

	//Bit
	bool GetBit(int aBitIndex) const 		{ return GetBit(myMask, aBitIndex); }
	bool SetBit(int aBitIndex, bool aValue)	{ return SetBit(myMask, aBitIndex, aValue); }

	/// Change mask bit to !value
	bool FlipBit(int aBitIndex)				{ return FlipBit(myMask, aBitIndex); }

	template<typename T>
	bool GetBit(T aEnumValue) requires std::is_enum_v<T>
	{ return GetBit(myMask, aEnumValue); }

	template<typename T>
	bool SetBit(T aEnumValue, bool aValue) requires std::is_enum_v<T>
	{ return SetBit(myMask, aEnumValue, aValue); }

	template<typename T>
	bool FlipBit(T aEnumValue) requires std::is_enum_v<T>
	{ return FlipBit(myMask, aEnumValue); }


	inline static bool GetBit( unsigned int aMask, int aBitIndex)
	{
		return aMask & (1 << aBitIndex);
	}

	inline static bool SetBit(unsigned int& outMask, int aBitIndex, bool aValue)
	{
		if (aValue)
		{
			outMask |= (1 << aBitIndex);
		}
		else
		{
			outMask &= ~(1 << aBitIndex);
		}

		return GetBit(outMask, aBitIndex);
	}

	inline static bool FlipBit(unsigned int& outMask, int aBitIndex)
	{
		outMask ^= (1 << aBitIndex);
		return GetBit(outMask, aBitIndex);
	}

	inline static unsigned int FlipMask(
		unsigned int aRhsMask,
		unsigned int aLhsMask)
	{
		return aRhsMask ^ aLhsMask;
	}

	template<typename T>
	inline static bool GetBit(unsigned int aMask, T aEnumValue)						requires std::is_enum_v<T>;

	template<typename T>
	inline static bool SetBit(unsigned int& outMask, T aEnumValue, bool aValue)		requires std::is_enum_v<T>;

	template<typename T>
	inline static bool FlipBit(unsigned int& outMask, T aEnumValue)					requires std::is_enum_v<T>;

	///Clears Mask to 0
	static void ClearMask(unsigned int& outMask) { outMask = 0; };

#pragma warning( push )
#pragma warning( disable : 4146 )
	///Set every bit to true 
	static void FillMask(unsigned int& outMask)  { outMask = -1u; };

#pragma warning( pop )
	
	//Digit
	int GetDigits(int aDigitOffsetIndex, int aDigitAmount) const								{ return GetDigits(myMask, aDigitOffsetIndex, aDigitAmount); }
	unsigned int SetDigits(int aDigitIndex, int aDigitAmount, int aValue)						{ return SetDigits(myMask, aDigitIndex, aDigitAmount, aValue); }
	
	
	inline static unsigned int GetDigits(unsigned int outMask, int aDigitOffsetIndex, int aDigitAmount);

	inline static unsigned int SetDigits(unsigned int& outMask, int aDigitOffsetIndex, int aDigitAmount, int aValue);

	inline static int CountDigits(unsigned int number);

};

template<typename T>
inline bool BitMask::GetBit(unsigned int aMask, T aEnumValue)					requires std::is_enum_v<T>
{
	return GetBit(aMask, static_cast<int>(aEnumValue));
}

template<typename T>
inline bool BitMask::SetBit(unsigned int& outMask, T aEnumValue, bool aValue)	requires std::is_enum_v<T>
{
	return SetBit(outMask, static_cast<int>(aEnumValue), aValue);
}

template<typename T>
inline bool BitMask::FlipBit(unsigned int& outMask, T aEnumValue)				requires std::is_enum_v<T>
{
	return FlipBit(outMask, static_cast<int>(aEnumValue));
}


inline int BitMask::CountDigits(unsigned int number)
{
	int digits = 0;
	while (number > 0) {
		number /= 10;
		++digits;
	}
	return digits > 0 ? digits : 1; // Handles the case where `number` is 0
}


inline unsigned int BitMask::GetDigits(unsigned int outMask, int aDigitOffsetIndex, int aDigitAmount)
{
	if (aDigitAmount < 1)		return 0;
	if (aDigitOffsetIndex < 0)	return 0;

	auto powBy10 = [](int aValue, int aPower)
		{
			int result = aValue;
			for (int i = 0; i < aPower; ++i)
			{
				result *= 10;
			}

			return result;
		};


	unsigned int higherPartIndex = powBy10(1, aDigitOffsetIndex + aDigitAmount);
	unsigned int lowerPartIndex = powBy10(1, aDigitOffsetIndex);

	unsigned int higherPart = outMask - outMask % higherPartIndex;
	unsigned int lowerPart = outMask % lowerPartIndex;

	outMask -= (higherPart + lowerPart);

	for (int i = 0; i < aDigitOffsetIndex; ++i)
	{
		outMask /= 10;
	}

	return outMask;
}


inline unsigned int BitMask::SetDigits(unsigned int& outMask, int aDigitOffsetIndex, int aDigitAmount, int aValue)
{
	if (aDigitAmount < 1)		return outMask;
	if (aDigitOffsetIndex < 0)	return outMask;

	auto powBy10 = [](int aValue, int aPower)
		{
			int result = aValue;
			for (int i = 0; i < aPower; ++i)
			{
				result *= 10;
			}

			return result;
		};


	unsigned int higherPartIndex	= powBy10(1, aDigitOffsetIndex + aDigitAmount);
	unsigned int lowerPartIndex		= powBy10(1, aDigitOffsetIndex);

	unsigned int higherPart			= outMask - outMask % higherPartIndex;
	unsigned int lowerPart			= outMask % lowerPartIndex;

	aValue							%= powBy10(1, aDigitAmount);

	unsigned int result				= higherPart + powBy10(aValue, aDigitOffsetIndex) + lowerPart;
	outMask = result;

	return outMask;
}