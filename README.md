# Usage

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
