
# File Reflection

This project uses Clang LLVM to reflect files and generate code to set up reflection data that exposes objects, variables and functions. This allows a program to dynamically read and call these functions. This is useful for scripting as data containers such as classes and structs can now be read and modified.
Macros
#define REFLECT_CLASS

Tell the reflector to reflect this class. Atm the reflector reflects all classes it finds.
#define REFLECT_FUNCTION

Tell the reflector to reflect this function. Atm the reflector reflects all public functions.
#define REFLECT_VARIABLE

Tell the reflector to reflect this variable. Atm the reflector reflects all public variables.
#define REFLECT_EXCLUDE_INVOKER

To save memory by defining this, Invoke won't be included inside ReflectionVariable and ReflectionFunctions.
Reflection

Uses Clang to reflect files and Generate reflection data with Shared Reflection Data.
Shared_Reflection_Data

Contains code for Reflection data.
Improvements
Main takes in Arguments
Reflect Code to constexpr
Variable and Functions to pointers
