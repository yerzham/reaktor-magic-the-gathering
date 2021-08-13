#ifndef RULE_NODE
#define RULE_NODE
#include <cstring>
#include <string>
#include <vector>

typedef struct ruleNode RuleNode;
struct ruleNode
{
    std::string name;
    std::string type;
    std::string contents;
    std::vector<RuleNode> children;
    bool operator<(const RuleNode &pt) const
    {
        return name.compare(pt.name) == -1;
    }
    bool operator==(const RuleNode &pt) const
    {
        return name.compare(pt.name) == 0;
    }
    size_t h(const RuleNode &toHash) const noexcept
    {
        size_t h = 0;
        for (size_t i = 0; i < toHash.name.length(); i++)
        {
            h = 31 * h + toHash.name.at(i);
        }
        return h;
    };
};

namespace std
{
    template <>
    struct hash<RuleNode>
    {
        std::size_t operator()(const RuleNode &p) const noexcept
        {
            return p.h(p);
        }
    };
}

#endif // RULE_NODE