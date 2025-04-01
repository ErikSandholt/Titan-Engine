
#  Reflection

Uses Clang to reflect files and Generate reflection data with Shared Reflection Data.

### Process

## 1. Find files

By searching a folder at a given path for .h and .hpp files.

## 2. Reflect

Each file is reflected with clang llvm to generate data such as: ReflectionClass, ReflectionVariable, ReflectionFunction and ReflectionType.

The data is stored inside the reflector.

## 3. Generate code

A "ReflectComponentData.cpp" will be generated that defines the function "InitReflection". Inside all the generated code will be created. 

#### See "ReflectComponentData.cpp" for an example
