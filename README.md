# Usage

## Create Reflection Data
```c++
struct Person {
  std::string name;
  int age;
  std::vector<std::string> addresses;
}
JSON_C(Person
  , JSON_M(name)
  , JSON_M(age)
  , JSON_M(addresses)
)
```

## Convert to and from JSON
```c++
Person bob{ "Bob", 42, {"hello", "world"} };
Json json(bob);

std::cout << json << std::endl; // { "name": "Bob", "age": 42, "addresses": ["hello", "world"]}
```
