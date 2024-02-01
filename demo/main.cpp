#include "../src/Json.h"
#include <sstream>
#include <fstream>
#include <bitset>

struct vec2 {
	float x = 0, y = 0;
};
JSON_C(vec2, JSON_M(x), JSON_M(y))

struct Leg {
	vec2 position;
};
JSON_C(Leg, JSON_M(position))

enum class SomeEnum { ONE = 1, TWO, THREE, FOUR, _COUNT };

struct Person {
	double age;
	std::string name;
	Leg legs[2];
	SomeEnum pEnum = SomeEnum::FOUR;

	bool operator==(Person p) { return name == p.name && age == p.age; }
};
JSON_C(Person
	, JSON_M(age)
	, JSON_M(name)
	, JSON_M(legs)
	, JSON_M(pEnum)
)

struct UserDefined {
	int value;
};

template <>
Json& Json::operator=(UserDefined v) {
	*this = Json::CreateEmptyObject<UserDefined>();
	(*this)["v"] = v.value;
	return *this;
}

template <>
Json::operator UserDefined() const {
	return UserDefined{ (*this)["v"] };
}

// TODO: document the usage of this
struct UserDefinedV2 {
	int value;

	operator Json() { return (Json)value; }
	UserDefinedV2(const Json& json) : value(json.number) { }
};

int main(int argc, char** argv) {
	Person person{ .age = 63, .name = "Bob", .legs = { Leg{{1,2}}, Leg{{3,4}} } };

	// Create a Json object from a value to which reflection data was provided (JSON_C, JSON_M)
	Json json(person);
	// Output Json object to an ostream
	std::cout << "[1.0] JSON Object parsed from the Person instance:\n" << json << "\n\n";
	// Parse Json object
	Person parsedPerson = json;
	std::cout << "[1.1] Does the parsed object match the original: " << std::boolalpha << (person == parsedPerson) << "\n\n";

	// Access member of the Person type by a string
	std::cout << "[2.0] Age of the person: " << json["age"] << "\n\n";

	// Create Json object from data without type information
	unsigned char* untyped = (unsigned char*)new vec2{ 2, 4 };
	// Get pointer to the type appropriate converter function
	Json::converter_function_t* anyToJsonVec2Fnc = Json::GetConverter<vec2>();
	std::cout << "[3.0] JSON Object created from data without type information:\n" << anyToJsonVec2Fnc(untyped) << "\n\n";

	// Parse struct from json object without type information
	Json vecToParse(vec2{ 4, 2 });
	unsigned char* parsedVec = (unsigned char*)new vec2();
	// Get parser function with which Json objects can be parsed without type information
	Json::parser_function_t* vec2Parser = Json::GetParser<vec2>();
	vec2Parser(vecToParse, parsedVec);
	std::cout << "[4.0] Parsed vec2: {" << ((vec2*)parsedVec)->x << "," << ((vec2*)parsedVec)->y << "}" << "\n\n";

	// Use conversion defined by the end user
	// To use a user defiend conversion, two methods have to be overriden: 
	//		Json& Json::operator=(T), and Json::operator T() const
	Json userDefinedJson(UserDefined{ .value = 3 });
	std::cout << "[5.0] Use of user-defined conversion: " << userDefinedJson << "\n\n";
	UserDefined userDefined = userDefinedJson;
	std::cout << "[5.1] Value of parsed user-defined object: " << userDefined.value << "\n\n";

	// Use the typeless parsers with the user defined conversions
	auto userDefiendConverter = Json::GetConverter<UserDefined>();
	auto userDefinedParser = Json::GetParser<UserDefined>();
	UserDefined userDefinedToConvert{ .value = 5 };
	Json userDefinedTypelessConverted = userDefiendConverter((unsigned char*)&userDefinedToConvert);
	std::cout << "[5.2] JSON object created with user-defined conversion without type info: " << userDefinedTypelessConverted << "\n\n";
	userDefinedParser(userDefinedTypelessConverted, (unsigned char*)&userDefinedToConvert);
	std::cout << "[5.2] Value of object parsed with user-defined conversion: " << userDefinedToConvert.value << "\n\n";

	// Parse json from input stream
	std::cout << "[6.0] Parsing json from input stream: " << std::endl;
	std::ifstream ifs("person.json");
	Json jsonFrank;
	ifs >> jsonFrank;
	std::cout << jsonFrank << std::endl;
	std::cout << ((Person)jsonFrank).age << std::endl;

	return 0;
}
