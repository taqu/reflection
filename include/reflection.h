#ifndef INC_REFLECTION_REFLECTION_H_
#define INC_REFLECTION_REFLECTION_H_
#include <string>
#include <vector>

namespace reflection
{
struct PropertyInfo
{
    std::string type_;
    std::string name_;
};

struct ClassInfo
{
    std::string className_;
    std::vector<PropertyInfo> properties_;
};

class Reflection
{
public:
    static void parse(const char* filepath, const char* compile_commands);
private:
    Reflection() = delete;
    ~Reflection() = delete;
    Reflection(const Reflection&) = delete;
    Reflection& operator=(const Reflection&) = delete;
};
} // namespace reflection
#endif // reflection
