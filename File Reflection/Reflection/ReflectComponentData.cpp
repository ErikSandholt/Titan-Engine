#include "../../Shared_Reflection_Data/ReflectionManager.h"

#include "C:/Program Files/Game/source/Special/Components/AbstractComponent.h"
#include "C:/Program Files/Game/source/Special/Components/Enemy.h"
#include "C:/Program Files/Game/source/Special/Components/EnvironmentObject.h"
#include "C:/Program Files/Game/source/Special/Components/Player.h"


void ReflectionManager::InitReflection()
{
	//	AbstractComponent
	//Globals
		//Functions

		//Variables

	//Class
	//exception_ptr
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 3699228615;
			rClass.globalID = 334812194;
			rClass.decal = 0;
			rClass.name = "exception_ptr";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[334812194] = std::move(rClass);
		myReflections[334812194] = &myClasses.at(334812194);
	}
	//nested_exception
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 2580103832;
			rClass.globalID = 3770116403;
			rClass.decal = 0;
			rClass.name = "nested_exception";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[3770116403] = std::move(rClass);
		myReflections[3770116403] = &myClasses.at(3770116403);
	}
	//AbstractComponent
	//Functions
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 338486351;
			function.globalID = 3048171889;
			function.name = "Talk";
			function.nameSpace.push_back("AbstractComponent");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&AbstractComponent::Talk);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[3048171889] = std::move(function);
		myReflections[3048171889] = &myFunctions.at(3048171889);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 794563358;
			function.globalID = 1964446384;
			function.name = "GetTypeName";
			function.nameSpace.push_back("AbstractComponent");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&AbstractComponent::GetTypeName);
			{
				ReflectionType type;
				type.decal = 8;
				type.type = static_cast<R_Type>(3);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[1964446384] = std::move(function);
		myReflections[1964446384] = &myFunctions.at(1964446384);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 545705456;
			function.globalID = 2508522266;
			function.name = "Revive";
			function.nameSpace.push_back("AbstractComponent");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&AbstractComponent::Revive);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[2508522266] = std::move(function);
		myReflections[2508522266] = &myFunctions.at(2508522266);
	}

	//Variables
	{
		//Variable
		ReflectionVariable variable = {};
		{
			variable.id = 926444256;
			variable.globalID = 3049552958;
			variable.decal = 1;
			variable.name = "id";
			variable.nameSpace.push_back("AbstractComponent");
			variable.invoke = ReflectionManager::Create_Variable(&AbstractComponent::id);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(13);
				variable.type = std::move(type);
			}
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(5);
				variable.valueType = std::move(type);
			}
		}
		myVariables[3049552958] = std::move(variable);
		myReflections[3049552958] = &myVariables.at(3049552958);
	}
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 363204579;
			rClass.globalID = 1926074251;
			rClass.decal = 0;
			rClass.name = "AbstractComponent";
			//Functions 
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(3048171889))); //AbstractComponent::Talk
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(1964446384))); //AbstractComponent::GetTypeName
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(2508522266))); //AbstractComponent::Revive
			//Variables 
			rClass.variables.push_back(static_cast<ReflectionVariable*>(myReflections.at(3049552958))); //AbstractComponent::id
		}
		myClasses[1926074251] = std::move(rClass);
		myReflections[1926074251] = &myClasses.at(1926074251);
	}
	//exception
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 626768054;
			rClass.globalID = 2926128558;
			rClass.decal = 0;
			rClass.name = "exception";
			//Functions 
			//Variables 
		}
		myClasses[2926128558] = std::move(rClass);
		myReflections[2926128558] = &myClasses.at(2926128558);
	}
	//bad_variant_access
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 1284580817;
			rClass.globalID = 877894598;
			rClass.decal = 0;
			rClass.name = "bad_variant_access";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[877894598] = std::move(rClass);
		myReflections[877894598] = &myClasses.at(877894598);
	}
	// bad_variant_access
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(877894598));
		rClass;
	}

	//	ComponentList
	//Globals
		//Functions

		//Variables

	//Class
	//numeric_limits<signed char>
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 1845926185;
			rClass.globalID = 4136283356;
			rClass.decal = 0;
			rClass.name = "numeric_limits<signed char>";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[4136283356] = std::move(rClass);
		myReflections[4136283356] = &myClasses.at(4136283356);
	}
	//numeric_limits<char>
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 2762296935;
			rClass.globalID = 958846636;
			rClass.decal = 0;
			rClass.name = "numeric_limits<char>";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[958846636] = std::move(rClass);
		myReflections[958846636] = &myClasses.at(958846636);
	}
	//numeric_limits<bool>
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 1870693603;
			rClass.globalID = 4286491148;
			rClass.decal = 0;
			rClass.name = "numeric_limits<bool>";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[4286491148] = std::move(rClass);
		myReflections[4286491148] = &myClasses.at(4286491148);
	}
	//_Num_int_base
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 3372008860;
			rClass.globalID = 66567972;
			rClass.decal = 0;
			rClass.name = "_Num_int_base";
			//Functions 
			//Variables 
		}
		myClasses[66567972] = std::move(rClass);
		myReflections[66567972] = &myClasses.at(66567972);
	}
	//numeric_limits<unsigned char>
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 221674914;
			rClass.globalID = 1331447515;
			rClass.decal = 0;
			rClass.name = "numeric_limits<unsigned char>";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[1331447515] = std::move(rClass);
		myReflections[1331447515] = &myClasses.at(1331447515);
	}
	//numeric_limits<char16_t>
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 3593727039;
			rClass.globalID = 340759984;
			rClass.decal = 0;
			rClass.name = "numeric_limits<char16_t>";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[340759984] = std::move(rClass);
		myReflections[340759984] = &myClasses.at(340759984);
	}
	//numeric_limits<wchar_t>
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 3710150815;
			rClass.globalID = 1004250594;
			rClass.decal = 0;
			rClass.name = "numeric_limits<wchar_t>";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[1004250594] = std::move(rClass);
		myReflections[1004250594] = &myClasses.at(1004250594);
	}
	//numeric_limits<char32_t>
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 942385497;
			rClass.globalID = 2438145602;
			rClass.decal = 0;
			rClass.name = "numeric_limits<char32_t>";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[2438145602] = std::move(rClass);
		myReflections[2438145602] = &myClasses.at(2438145602);
	}
	//numeric_limits<short>
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 1344462639;
			rClass.globalID = 2880366106;
			rClass.decal = 0;
			rClass.name = "numeric_limits<short>";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[2880366106] = std::move(rClass);
		myReflections[2880366106] = &myClasses.at(2880366106);
	}
	//numeric_limits<int>
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 227335918;
			rClass.globalID = 1847847371;
			rClass.decal = 0;
			rClass.name = "numeric_limits<int>";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[1847847371] = std::move(rClass);
		myReflections[1847847371] = &myClasses.at(1847847371);
	}
	//numeric_limits<long>
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 281762001;
			rClass.globalID = 3241376442;
			rClass.decal = 0;
			rClass.name = "numeric_limits<long>";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[3241376442] = std::move(rClass);
		myReflections[3241376442] = &myClasses.at(3241376442);
	}
	//numeric_limits<long long>
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 2407660197;
			rClass.globalID = 2225504608;
			rClass.decal = 0;
			rClass.name = "numeric_limits<long long>";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[2225504608] = std::move(rClass);
		myReflections[2225504608] = &myClasses.at(2225504608);
	}
	//numeric_limits<unsigned short>
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 701055992;
			rClass.globalID = 2717342763;
			rClass.decal = 0;
			rClass.name = "numeric_limits<unsigned short>";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[2717342763] = std::move(rClass);
		myReflections[2717342763] = &myClasses.at(2717342763);
	}
	//numeric_limits<unsigned int>
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 2311892497;
			rClass.globalID = 156770026;
			rClass.decal = 0;
			rClass.name = "numeric_limits<unsigned int>";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[156770026] = std::move(rClass);
		myReflections[156770026] = &myClasses.at(156770026);
	}
	//numeric_limits<unsigned long>
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 1305432348;
			rClass.globalID = 3643917493;
			rClass.decal = 0;
			rClass.name = "numeric_limits<unsigned long>";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[3643917493] = std::move(rClass);
		myReflections[3643917493] = &myClasses.at(3643917493);
	}
	//numeric_limits<unsigned long long>
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 754399506;
			rClass.globalID = 220153905;
			rClass.decal = 0;
			rClass.name = "numeric_limits<unsigned long long>";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[220153905] = std::move(rClass);
		myReflections[220153905] = &myClasses.at(220153905);
	}
	//numeric_limits<float>
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 3587252403;
			rClass.globalID = 77467974;
			rClass.decal = 0;
			rClass.name = "numeric_limits<float>";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[77467974] = std::move(rClass);
		myReflections[77467974] = &myClasses.at(77467974);
	}
	//_Num_float_base
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 1077649045;
			rClass.globalID = 1872797821;
			rClass.decal = 0;
			rClass.name = "_Num_float_base";
			//Functions 
			//Variables 
		}
		myClasses[1872797821] = std::move(rClass);
		myReflections[1872797821] = &myClasses.at(1872797821);
	}
	//numeric_limits<double>
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 463956600;
			rClass.globalID = 3583334591;
			rClass.decal = 0;
			rClass.name = "numeric_limits<double>";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[3583334591] = std::move(rClass);
		myReflections[3583334591] = &myClasses.at(3583334591);
	}
	//numeric_limits<long double>
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 1569975028;
			rClass.globalID = 3440566409;
			rClass.decal = 0;
			rClass.name = "numeric_limits<long double>";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[3440566409] = std::move(rClass);
		myReflections[3440566409] = &myClasses.at(3440566409);
	}
	//allocator<void>
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 1905669118;
			rClass.globalID = 3847883111;
			rClass.decal = 0;
			rClass.name = "allocator<void>";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[3847883111] = std::move(rClass);
		myReflections[3847883111] = &myClasses.at(3847883111);
	}
	//tuple<>
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 2073971179;
			rClass.globalID = 1599369878;
			rClass.decal = 0;
			rClass.name = "tuple<>";
			rClass.nameSpace.push_back("std");
			//Functions 
			//Variables 
		}
		myClasses[1599369878] = std::move(rClass);
		myReflections[1599369878] = &myClasses.at(1599369878);
	}
	//memory_resource
	//Functions

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 1455044015;
			rClass.globalID = 1381086127;
			rClass.decal = 0;
			rClass.name = "memory_resource";
			rClass.nameSpace.push_back("std");
			rClass.nameSpace.push_back("pmr");
			//Functions 
			//Variables 
		}
		myClasses[1381086127] = std::move(rClass);
		myReflections[1381086127] = &myClasses.at(1381086127);
	}
	// numeric_limits<signed char>
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(4136283356));
		rClass;
	}
	// numeric_limits<char>
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(958846636));
		rClass;
	}
	// numeric_limits<bool>
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(4286491148));
		rClass;
	}
	// numeric_limits<unsigned char>
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(1331447515));
		rClass;
	}
	// numeric_limits<char16_t>
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(340759984));
		rClass;
	}
	// numeric_limits<wchar_t>
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(1004250594));
		rClass;
	}
	// numeric_limits<char32_t>
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(2438145602));
		rClass;
	}
	// numeric_limits<short>
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(2880366106));
		rClass;
	}
	// numeric_limits<int>
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(1847847371));
		rClass;
	}
	// numeric_limits<long>
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(3241376442));
		rClass;
	}
	// numeric_limits<long long>
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(2225504608));
		rClass;
	}
	// numeric_limits<unsigned short>
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(2717342763));
		rClass;
	}
	// numeric_limits<unsigned int>
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(156770026));
		rClass;
	}
	// numeric_limits<unsigned long>
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(3643917493));
		rClass;
	}
	// numeric_limits<unsigned long long>
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(220153905));
		rClass;
	}
	// numeric_limits<float>
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(77467974));
		rClass;
	}
	// numeric_limits<double>
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(3583334591));
		rClass;
	}
	// numeric_limits<long double>
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(3440566409));
		rClass;
	}

	//	Enemy
	//Globals
		//Functions

		//Variables

	//Class
	//Enemy
	//Functions
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 2361356451;
			function.globalID = 4285342741;
			function.name = "Punch";
			function.nameSpace.push_back("Enemy");
			function.decal = 41;
			function.invoke = ReflectionManager::Create_Function(&Enemy::Punch);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[4285342741] = std::move(function);
		myReflections[4285342741] = &myFunctions.at(4285342741);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 3630339793;
			function.globalID = 647175659;
			function.name = "Die";
			function.nameSpace.push_back("Enemy");
			function.decal = 41;
			function.invoke = ReflectionManager::Create_Function(&Enemy::Die);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[647175659] = std::move(function);
		myReflections[647175659] = &myFunctions.at(647175659);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 3009444011;
			function.globalID = 2656482077;
			function.name = "ChangeImage";
			function.nameSpace.push_back("Enemy");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&Enemy::ChangeImage);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[2656482077] = std::move(function);
		myReflections[2656482077] = &myFunctions.at(2656482077);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 3761822729;
			function.globalID = 1857033891;
			function.name = "MoveRight";
			function.nameSpace.push_back("Enemy");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&Enemy::MoveRight);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[1857033891] = std::move(function);
		myReflections[1857033891] = &myFunctions.at(1857033891);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 1297420860;
			function.globalID = 3345444430;
			function.name = "MoveLeft";
			function.nameSpace.push_back("Enemy");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&Enemy::MoveLeft);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[3345444430] = std::move(function);
		myReflections[3345444430] = &myFunctions.at(3345444430);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 4131112783;
			function.globalID = 720736553;
			function.name = "AttackPlayer";
			function.nameSpace.push_back("Enemy");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&Enemy::AttackPlayer);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[720736553] = std::move(function);
		myReflections[720736553] = &myFunctions.at(720736553);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 2352122139;
			function.globalID = 3669454289;
			function.name = "LevelUp";
			function.nameSpace.push_back("Enemy");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&Enemy::LevelUp);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[3669454289] = std::move(function);
		myReflections[3669454289] = &myFunctions.at(3669454289);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 338486351;
			function.globalID = 1903513561;
			function.name = "Talk";
			function.nameSpace.push_back("Enemy");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&Enemy::Talk);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[1903513561] = std::move(function);
		myReflections[1903513561] = &myFunctions.at(1903513561);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 1599949867;
			function.globalID = 1689412473;
			function.name = "GetPosition";
			function.nameSpace.push_back("Enemy");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&Enemy::GetPosition);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
			{
				//Variable
				ReflectionVariable variable = {};
				{
					variable.id = 2407326440;
					variable.globalID = 333092778;
					variable.decal = 0;
					variable.name = "outX";
					variable.nameSpace.push_back("Enemy");
					{
						ReflectionType type;
						type.decal = 0;
						type.type = static_cast<R_Type>(14);
						variable.type = std::move(type);
					}
					{
						ReflectionType type;
						type.decal = 9;
						type.type = static_cast<R_Type>(7);
						variable.valueType = std::move(type);
					}
				}
				function.arguments.push_back(std::move(variable));
			}
			{
				//Variable
				ReflectionVariable variable = {};
				{
					variable.id = 2424104059;
					variable.globalID = 349870397;
					variable.decal = 0;
					variable.name = "outY";
					variable.nameSpace.push_back("Enemy");
					{
						ReflectionType type;
						type.decal = 0;
						type.type = static_cast<R_Type>(14);
						variable.type = std::move(type);
					}
					{
						ReflectionType type;
						type.decal = 9;
						type.type = static_cast<R_Type>(7);
						variable.valueType = std::move(type);
					}
				}
				function.arguments.push_back(std::move(variable));
			}
		}
		myFunctions[1689412473] = std::move(function);
		myReflections[1689412473] = &myFunctions.at(1689412473);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 2564056209;
			function.globalID = 3482749407;
			function.name = "GetSprite";
			function.nameSpace.push_back("Enemy");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&Enemy::GetSprite);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(5);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[3482749407] = std::move(function);
		myReflections[3482749407] = &myFunctions.at(3482749407);
	}

	//Variables
	{
		//Variable
		ReflectionVariable variable = {};
		{
			variable.id = 1778719668;
			variable.globalID = 2845243482;
			variable.decal = 1;
			variable.name = "enemyType";
			variable.nameSpace.push_back("Enemy");
			variable.invoke = ReflectionManager::Create_Variable(&Enemy::enemyType);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(13);
				variable.type = std::move(type);
			}
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(5);
				variable.valueType = std::move(type);
			}
		}
		myVariables[2845243482] = std::move(variable);
		myReflections[2845243482] = &myVariables.at(2845243482);
	}
	{
		//Variable
		ReflectionVariable variable = {};
		{
			variable.id = 3766098096;
			variable.globalID = 3283800734;
			variable.decal = 1;
			variable.name = "strength";
			variable.nameSpace.push_back("Enemy");
			variable.invoke = ReflectionManager::Create_Variable(&Enemy::strength);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(13);
				variable.type = std::move(type);
			}
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(5);
				variable.valueType = std::move(type);
			}
		}
		myVariables[3283800734] = std::move(variable);
		myReflections[3283800734] = &myVariables.at(3283800734);
	}
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 1776971179;
			rClass.globalID = 3295822931;
			rClass.decal = 0;
			rClass.name = "Enemy";
			//Functions 
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(4285342741))); //Enemy::Punch
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(647175659))); //Enemy::Die
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(2656482077))); //Enemy::ChangeImage
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(1857033891))); //Enemy::MoveRight
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(3345444430))); //Enemy::MoveLeft
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(720736553))); //Enemy::AttackPlayer
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(3669454289))); //Enemy::LevelUp
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(1903513561))); //Enemy::Talk
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(1689412473))); //Enemy::GetPosition
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(3482749407))); //Enemy::GetSprite
			//Variables 
			rClass.variables.push_back(static_cast<ReflectionVariable*>(myReflections.at(2845243482))); //Enemy::enemyType
			rClass.variables.push_back(static_cast<ReflectionVariable*>(myReflections.at(3283800734))); //Enemy::strength
		}
		myClasses[3295822931] = std::move(rClass);
		myReflections[3295822931] = &myClasses.at(3295822931);
	}
	// Enemy
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(3295822931));
		rClass;
		rClass->functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(1964446384))); //AbstractComponent::GetTypeName
		rClass->functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(2508522266))); //AbstractComponent::Revive
		rClass->variables.push_back(static_cast<ReflectionVariable*>(myReflections.at(3049552958))); //AbstractComponent::id
	}

	//	EnvironmentObject
	//Globals
		//Functions

		//Variables

	//Class
	//EnvironmentObject
	//Functions
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 3009444011;
			function.globalID = 330075874;
			function.name = "ChangeImage";
			function.nameSpace.push_back("EnvironmentObject");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&EnvironmentObject::ChangeImage);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[330075874] = std::move(function);
		myReflections[330075874] = &myFunctions.at(330075874);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 4284266772;
			function.globalID = 1332841347;
			function.name = "MoveUp";
			function.nameSpace.push_back("EnvironmentObject");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&EnvironmentObject::MoveUp);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[1332841347] = std::move(function);
		myReflections[1332841347] = &myFunctions.at(1332841347);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 2410144673;
			function.globalID = 1791943958;
			function.name = "MoveDown";
			function.nameSpace.push_back("EnvironmentObject");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&EnvironmentObject::MoveDown);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[1791943958] = std::move(function);
		myReflections[1791943958] = &myFunctions.at(1791943958);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 1599949867;
			function.globalID = 2236000262;
			function.name = "GetPosition";
			function.nameSpace.push_back("EnvironmentObject");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&EnvironmentObject::GetPosition);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
			{
				//Variable
				ReflectionVariable variable = {};
				{
					variable.id = 2407326440;
					variable.globalID = 592981171;
					variable.decal = 0;
					variable.name = "outX";
					variable.nameSpace.push_back("EnvironmentObject");
					{
						ReflectionType type;
						type.decal = 0;
						type.type = static_cast<R_Type>(14);
						variable.type = std::move(type);
					}
					{
						ReflectionType type;
						type.decal = 9;
						type.type = static_cast<R_Type>(7);
						variable.valueType = std::move(type);
					}
				}
				function.arguments.push_back(std::move(variable));
			}
			{
				//Variable
				ReflectionVariable variable = {};
				{
					variable.id = 2424104059;
					variable.globalID = 576203552;
					variable.decal = 0;
					variable.name = "outY";
					variable.nameSpace.push_back("EnvironmentObject");
					{
						ReflectionType type;
						type.decal = 0;
						type.type = static_cast<R_Type>(14);
						variable.type = std::move(type);
					}
					{
						ReflectionType type;
						type.decal = 9;
						type.type = static_cast<R_Type>(7);
						variable.valueType = std::move(type);
					}
				}
				function.arguments.push_back(std::move(variable));
			}
		}
		myFunctions[2236000262] = std::move(function);
		myReflections[2236000262] = &myFunctions.at(2236000262);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 2564056209;
			function.globalID = 2458337176;
			function.name = "GetSprite";
			function.nameSpace.push_back("EnvironmentObject");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&EnvironmentObject::GetSprite);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(5);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[2458337176] = std::move(function);
		myReflections[2458337176] = &myFunctions.at(2458337176);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 338486351;
			function.globalID = 3108868390;
			function.name = "Talk";
			function.nameSpace.push_back("EnvironmentObject");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&EnvironmentObject::Talk);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[3108868390] = std::move(function);
		myReflections[3108868390] = &myFunctions.at(3108868390);
	}

	//Variables
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 574178044;
			rClass.globalID = 1597272340;
			rClass.decal = 0;
			rClass.name = "EnvironmentObject";
			//Functions 
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(330075874))); //EnvironmentObject::ChangeImage
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(1332841347))); //EnvironmentObject::MoveUp
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(1791943958))); //EnvironmentObject::MoveDown
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(2236000262))); //EnvironmentObject::GetPosition
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(2458337176))); //EnvironmentObject::GetSprite
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(3108868390))); //EnvironmentObject::Talk
			//Variables 
		}
		myClasses[1597272340] = std::move(rClass);
		myReflections[1597272340] = &myClasses.at(1597272340);
	}
	// EnvironmentObject
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(1597272340));
		rClass;
		rClass->functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(1964446384))); //AbstractComponent::GetTypeName
		rClass->functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(2508522266))); //AbstractComponent::Revive
		rClass->variables.push_back(static_cast<ReflectionVariable*>(myReflections.at(3049552958))); //AbstractComponent::id
	}

	//	Player
	//Globals
		//Functions

		//Variables

	//Class
	//Player
	//Functions
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 3009444011;
			function.globalID = 1693340654;
			function.name = "ChangeImage";
			function.nameSpace.push_back("Player");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&Player::ChangeImage);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[1693340654] = std::move(function);
		myReflections[1693340654] = &myFunctions.at(1693340654);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 3761822729;
			function.globalID = 1852562592;
			function.name = "MoveRight";
			function.nameSpace.push_back("Player");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&Player::MoveRight);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[1852562592] = std::move(function);
		myReflections[1852562592] = &myFunctions.at(1852562592);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 1297420860;
			function.globalID = 176018351;
			function.name = "MoveLeft";
			function.nameSpace.push_back("Player");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&Player::MoveLeft);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[176018351] = std::move(function);
		myReflections[176018351] = &myFunctions.at(176018351);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 1599949867;
			function.globalID = 3090983162;
			function.name = "GetPosition";
			function.nameSpace.push_back("Player");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&Player::GetPosition);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
			{
				//Variable
				ReflectionVariable variable = {};
				{
					variable.id = 2407326440;
					variable.globalID = 120079087;
					variable.decal = 0;
					variable.name = "outX";
					variable.nameSpace.push_back("Player");
					{
						ReflectionType type;
						type.decal = 0;
						type.type = static_cast<R_Type>(14);
						variable.type = std::move(type);
					}
					{
						ReflectionType type;
						type.decal = 9;
						type.type = static_cast<R_Type>(7);
						variable.valueType = std::move(type);
					}
				}
				function.arguments.push_back(std::move(variable));
			}
			{
				//Variable
				ReflectionVariable variable = {};
				{
					variable.id = 2424104059;
					variable.globalID = 103301468;
					variable.decal = 0;
					variable.name = "outY";
					variable.nameSpace.push_back("Player");
					{
						ReflectionType type;
						type.decal = 0;
						type.type = static_cast<R_Type>(14);
						variable.type = std::move(type);
					}
					{
						ReflectionType type;
						type.decal = 9;
						type.type = static_cast<R_Type>(7);
						variable.valueType = std::move(type);
					}
				}
				function.arguments.push_back(std::move(variable));
			}
		}
		myFunctions[3090983162] = std::move(function);
		myReflections[3090983162] = &myFunctions.at(3090983162);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 2564056209;
			function.globalID = 72064492;
			function.name = "GetSprite";
			function.nameSpace.push_back("Player");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&Player::GetSprite);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(5);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[72064492] = std::move(function);
		myReflections[72064492] = &myFunctions.at(72064492);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 338486351;
			function.globalID = 2663788442;
			function.name = "Talk";
			function.nameSpace.push_back("Player");
			function.decal = 1;
			function.invoke = ReflectionManager::Create_Function(&Player::Talk);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[2663788442] = std::move(function);
		myReflections[2663788442] = &myFunctions.at(2663788442);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 2361356451;
			function.globalID = 110568060;
			function.name = "Punch";
			function.nameSpace.push_back("Player");
			function.decal = 41;
			function.invoke = ReflectionManager::Create_Function(&Player::Punch);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[110568060] = std::move(function);
		myReflections[110568060] = &myFunctions.at(110568060);
	}
	{
		//Function
		ReflectionFunction function = {};
		{
			function.id = 3630339793;
			function.globalID = 1855320642;
			function.name = "Die";
			function.nameSpace.push_back("Player");
			function.decal = 41;
			function.invoke = ReflectionManager::Create_Function(&Player::Die);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(1);
				function.returnValue = std::move(type);
			}
		}
		myFunctions[1855320642] = std::move(function);
		myReflections[1855320642] = &myFunctions.at(1855320642);
	}

	//Variables
	{
		//Variable
		ReflectionVariable variable = {};
		{
			variable.id = 1937375343;
			variable.globalID = 1031447114;
			variable.decal = 1;
			variable.name = "TestValue";
			variable.nameSpace.push_back("Player");
			variable.invoke = ReflectionManager::Create_Variable(&Player::TestValue);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(13);
				variable.type = std::move(type);
			}
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(5);
				variable.valueType = std::move(type);
			}
		}
		myVariables[1031447114] = std::move(variable);
		myReflections[1031447114] = &myVariables.at(1031447114);
	}
	{
		//Variable
		ReflectionVariable variable = {};
		{
			variable.id = 3912097325;
			variable.globalID = 1768061722;
			variable.decal = 1;
			variable.name = "playerData";
			variable.nameSpace.push_back("Player");
			variable.invoke = ReflectionManager::Create_Variable(&Player::playerData);
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(13);
				variable.type = std::move(type);
			}
			{
				ReflectionType type;
				type.decal = 0;
				type.type = static_cast<R_Type>(5);
				variable.valueType = std::move(type);
			}
		}
		myVariables[1768061722] = std::move(variable);
		myReflections[1768061722] = &myVariables.at(1768061722);
	}
	{
		//Function
		ReflectionClass rClass = {};
		{
			rClass.id = 748274432;
			rClass.globalID = 2706737960;
			rClass.decal = 0;
			rClass.name = "Player";
			//Functions 
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(1693340654))); //Player::ChangeImage
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(1852562592))); //Player::MoveRight
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(176018351))); //Player::MoveLeft
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(3090983162))); //Player::GetPosition
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(72064492))); //Player::GetSprite
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(2663788442))); //Player::Talk
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(110568060))); //Player::Punch
			rClass.functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(1855320642))); //Player::Die
			//Variables 
			rClass.variables.push_back(static_cast<ReflectionVariable*>(myReflections.at(1031447114))); //Player::TestValue
			rClass.variables.push_back(static_cast<ReflectionVariable*>(myReflections.at(1768061722))); //Player::playerData
		}
		myClasses[2706737960] = std::move(rClass);
		myReflections[2706737960] = &myClasses.at(2706737960);
	}
	// Player
	{
		ReflectionClass* rClass = static_cast<ReflectionClass*>(myReflections.at(2706737960));
		rClass;
		rClass->functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(1964446384))); //AbstractComponent::GetTypeName
		rClass->functions.push_back(static_cast<ReflectionFunction*>(myReflections.at(2508522266))); //AbstractComponent::Revive
		rClass->variables.push_back(static_cast<ReflectionVariable*>(myReflections.at(3049552958))); //AbstractComponent::id
	}

}