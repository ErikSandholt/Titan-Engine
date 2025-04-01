
# Shared_Reflection_Data
Contains the code for Reflection Data and the system that manages them.

## Reflection Data Types
**BitMask** _Decal_: 

Uses a contains a UInt were each digit represents a enum. The enum represents keywords such as public, static, const, pointer, reference.

**R_Type** _type_:    

Is an enum that represents the type this reflection is. It can be a base type, class, function or variable.

**ReflectionBase*** _data_:    

Points to additional data such as another ReflectionBase.

### ReflectionBase

### ReflectionType

### ReflectionVariable

### ReflectionFunction

### ReflectionClass

Contains code for Reflection data.
