#pragma once
#include <functional>
#include <string>
#include <assert.h>

namespace Sunder
{
    //For the current purpose we want to hide template so it cant be specified
    class CallEvent
    {
    private:
        struct WrappingInfo
        {
            int     ArgSize     = 0;
            char*   Data        = nullptr;

            inline bool IsWrapped() const { return Data != nullptr; }
        };

        char*                                                           myReturnValue       = nullptr;
        int                                                             myReturnValueSize   = 0;
        WrappingInfo*                                                   myWrappingInfo      = nullptr;
        int                                                             myArgAmount         = 0;
        std::string                                                     myArgs              = "";
        std::function<void(char** someData, char* outReturnValue)>      myCall              = std::function<void(char**, char*)>();

    public:
        CallEvent() = default;

        template<typename T>
        CallEvent(const std::function<T()>& aFunction)
        {
            Clear();

            myCall = [aFunction](char** someData, char* outReturnValue)
                {
                    someData;
                    outReturnValue;
                    if constexpr (std::is_same_v<T, void>)
                    {
                        aFunction();
                    }
                    else
                    {
                        //Capture return value and store it
                        new(outReturnValue) T(std::move(aFunction()));
                    }
                };
        }


        template <typename T, typename... Args>
        CallEvent(const std::function<T(Args...)>& aFunction)
        {
            Clear();

            int argAmount       = sizeof...(Args);
            myArgAmount         = argAmount;

            myWrappingInfo      = reinterpret_cast<WrappingInfo*>(std::malloc(myArgAmount * sizeof(WrappingInfo)));

            //Check if Failed to allocate memory
            if (myWrappingInfo == nullptr)  throw;

            if constexpr (!std::is_same_v<T, void>)
            {
                myReturnValueSize   = sizeof(T);
                myReturnValue       = reinterpret_cast<char*>(std::malloc(myReturnValueSize));

                if (myReturnValue == nullptr)   throw;
            }


            for (size_t i = 0; i < myArgAmount; i++)
            {
                myWrappingInfo[i] = WrappingInfo();
            }

            myArgs = ((std::string(typeid(Args).name()) + ", ") + ...);
            myArgs = myArgs.substr(0, myArgs.size() - 2);

            myCall = [aFunction, argAmount](char** someData, char* outReturnValue)
                {
                    char** data = someData + argAmount;
                    if constexpr (std::is_same_v<T, void>)
                    {
                        outReturnValue;
                        aFunction(InterpretData<Args>(data)...);
                    }
                    else
                    {
                        //Capture return value and store it
                        new(outReturnValue) T(std::move( aFunction(InterpretData<Args>(data)...) ));
                    }
                    
                };
        }

        CallEvent(const CallEvent& aOther)
        {
            *this = aOther;
        }

        ~CallEvent()
        {
            Clear();
        }

        void operator =(const CallEvent& aOther) 
        {
            Clear();

            myArgAmount         = aOther.myArgAmount;
            myReturnValueSize   = aOther.myReturnValueSize;
            myArgs              = aOther.myArgs;
            myCall              = aOther.myCall;


            if (0 < aOther.myReturnValueSize)
            {
                myReturnValue = reinterpret_cast<char*>(std::malloc(aOther.myReturnValueSize));
                assert(myReturnValue != NULL && aOther.myReturnValue != NULL);
            }

            //Check if arguments are empty
            if (myArgAmount < 1) return;

            int memorySize      = WrapListTotalSize(aOther.myWrappingInfo, aOther.myArgAmount);
            myWrappingInfo      = reinterpret_cast<WrappingInfo*>(std::malloc(myArgAmount * sizeof(WrappingInfo)));




            //Check if Failed to allocate memory
            if (myWrappingInfo == NULL   || aOther.myWrappingInfo == NULL)    throw;

            std::memcpy(myWrappingInfo, aOther.myWrappingInfo, memorySize);

            char* data = reinterpret_cast<char*>(myWrappingInfo) + myArgAmount * sizeof(WrappingInfo);
            for (int i = 0; i < myArgAmount; ++i)
            {
                if (myWrappingInfo[i].IsWrapped() == false || myWrappingInfo[i].ArgSize == 0) continue;

                myWrappingInfo[i].Data = data;
                data += myWrappingInfo[i].ArgSize;
            }
        }

        int GetArgAmount() const { return myArgAmount; }
        std::string GetArgsName() const { return myArgs; }


        template <typename T = void, typename... Args>
        T Invoke(const Args... aArgs) const
        {
            if (myCall == false)
            {
                if constexpr (std::is_pointer_v<T>)
                {
                    return nullptr;
                }
                else if constexpr (std::is_reference_v<T>)
                {
                    return &myReturnValue;
                }
                else if constexpr (!std::is_same_v<T, void>)
                {
                    return {}; //*reinterpret_cast<T*>(myReturnValue);
                }
                else
                {
                    return;
                }
            }

            if (myArgAmount == 0) 
            {
                myCall(nullptr, myReturnValue);

                if constexpr (std::is_pointer_v<T>)
                {
                    reinterpret_cast<T*>(myReturnValue);
                }
                else if constexpr (std::is_reference_v<T>)
                {
                    reinterpret_cast<T>(*myReturnValue);
                }
                else if constexpr (!std::is_same_v<T, void>)
                {
                    *reinterpret_cast<T*>(myReturnValue);
                }
                else
                {
                    return;
                }
                
                //return;
            }

            int iterateIndex = 0;
            char** argumentList = NULL;
            if (0 < myArgAmount)
            {
                argumentList = reinterpret_cast<char**>(_alloca(myArgAmount * sizeof(char*)));
                assert(argumentList != NULL);
            }
            

            for (size_t i = 0; i < myArgAmount; i++)
            {
                argumentList[i] = myWrappingInfo[i].Data;
            }

            //Find the first unwrapped data
            while (iterateIndex < myArgAmount && myWrappingInfo[iterateIndex].IsWrapped())
            {
                iterateIndex++;
            }

            (StorArgument(argumentList, iterateIndex, aArgs), ...);
            
            assert(iterateIndex <= myArgAmount      && "Too many Arguments");
            assert(iterateIndex == myArgAmount      && "Missing Arguments");

            myCall(argumentList, myReturnValue);

            if constexpr (std::is_pointer_v<T>)
            {
                return reinterpret_cast<T>(myReturnValue);
            }
            else if constexpr (std::is_reference_v<T>)
            {
                return reinterpret_cast<T>(*myReturnValue);
            }
            else if constexpr (!std::is_same_v<T, void>)
            {
                return *reinterpret_cast<T*>(myReturnValue);
            }
            else
            {
                return;
            }
        }

        template <typename T>
        void Wrap(size_t aIndex, const T& aValue)
        {
            if (myArgAmount < 1)
            {
                return;
            }

            int memorySize = 0;
            WrappingInfo* newWrapInfo = nullptr;
            {
                memorySize  = WrapListTotalSize(myWrappingInfo, myArgAmount);
                memorySize += sizeof(T) - myWrappingInfo[aIndex].ArgSize;

                newWrapInfo = reinterpret_cast<WrappingInfo*>(malloc(memorySize));
                std::memcpy(newWrapInfo, myWrappingInfo, myArgAmount * sizeof(WrappingInfo));
            }

            if constexpr (!std::is_reference<T>::value && !std::is_pointer<T>::value)
            {
                newWrapInfo[aIndex].ArgSize = sizeof(T);
            }
            else 
            {
                newWrapInfo[aIndex].ArgSize = 0;
            }

            using baseType = std::remove_const_t< std::remove_reference_t<decltype(aValue)> >;

            char* iterateLocation = reinterpret_cast<char*>(&newWrapInfo[myArgAmount]);
            for (size_t i = 0; i < myArgAmount; i++)
            {
                WrappingInfo& wrapper = newWrapInfo[i];
                if (i != aIndex && wrapper.IsWrapped() == false) continue;

                if (i == aIndex) 
                {
                    if constexpr (std::is_reference<T>::value)
                    {
                        //Make pointer
                        char* location = ConvertPtr(&aValue);
                        wrapper.Data = location;
                    }
                    else if constexpr (std::is_pointer<T>::value)
                    {
                        char* location = ConvertPtr(aValue);
                        wrapper.Data = location;
                    }
                    else
                    {
                        //Fill memory
                        std::memcpy(iterateLocation, &aValue, wrapper.ArgSize);
                        wrapper.Data = iterateLocation;
                    }
                }
                else if(0 < wrapper.ArgSize)
                {
                    std::memcpy(iterateLocation, wrapper.Data, wrapper.ArgSize);
                    wrapper.Data = iterateLocation;
                }

                iterateLocation += wrapper.ArgSize;
            }

            WrapArgString(newWrapInfo);

            std::free(myWrappingInfo);
            myWrappingInfo = newWrapInfo;
        }

    private:


        static int WrapListTotalSize(WrappingInfo* aList, int aSize)
        {
            int size = aSize * sizeof( WrappingInfo );
            for (size_t i = 0; i < aSize; i++)
            {
                size += aList[i].ArgSize;
            }

            return size;
        }

        inline void Clear() 
        {
            if (myWrappingInfo)     free(myWrappingInfo);
            if (myReturnValue)      free(myReturnValue);

            myWrappingInfo      = nullptr;
            myReturnValue       = nullptr;
            myArgAmount         = 0;
            myReturnValueSize   = 0;
            myArgs              = "No Argument";
            myCall              = std::function<void(char**, char*)>();
        }

        static char* ConvertPtr(const void* aValue) 
        {
            void* voidPtr = const_cast<void*>(aValue);
            return reinterpret_cast<char*>(voidPtr);
        }


        template <typename T>
        static T& InterpretData(char**& aPointer)
        {
            aPointer -= 1;  //Reverse since arguments are read backwards for some reason

            if constexpr (std::is_reference<T>::value)
            {
                return reinterpret_cast<T>(**aPointer);
            }
            else if constexpr (std::is_pointer<T>::value)
            {
                return reinterpret_cast<T&>(*aPointer);
            }
            else
            {
                return *reinterpret_cast<T*>(*aPointer);
            }
        }

        template <typename T>
        void StorArgument(char**& aList, int& aIndex, const T& aArgument) const
        {
            //Check whats wrapped
            assert(aIndex <= myArgAmount && "Too many Arguments");

            using baseType = std::remove_const_t< std::remove_reference_t<decltype(aArgument)> >;

            if constexpr (std::is_reference<T>::value)
            {
                baseType* ptr = const_cast<baseType*>(&aArgument);
                aList[aIndex] = reinterpret_cast<char*>(ptr);
            }
            else if constexpr (std::is_pointer<T>::value)
            {
                baseType ptr = const_cast<baseType>(aArgument);
                aList[aIndex] = reinterpret_cast<char*>(ptr);
            }
            else
            {
                baseType* ptr = const_cast<baseType*>(&aArgument);
                aList[aIndex] = reinterpret_cast<char*>(ptr);
            }

            aIndex++;
            while (aIndex < myArgAmount && myWrappingInfo[aIndex].IsWrapped())
            {
                aIndex++;
            }
        }

        void WrapArgString(WrappingInfo* newWrapList)
        {
            std::string someArguments = myArgs;

            size_t iterateIndex = 0;

            //Insert "Wrapped(" and ")" at the next ',' or end of string
            for (int i = 0; i < myArgAmount; i++)
            {
                iterateIndex = 0 < i ? someArguments.find(',', iterateIndex) + 2 : 0;

                //Skip if not wrapping or already wrapped 
                if (newWrapList[i].IsWrapped() == false || myWrappingInfo[i].IsWrapped()) continue;

                someArguments.insert(iterateIndex, "Wrapped(");

                iterateIndex = i < myArgAmount - 1 ? someArguments.find(',', iterateIndex + 8) : someArguments.size();

                someArguments.insert(iterateIndex, ")");
            }

            myArgs = someArguments;
        }
    };
}

