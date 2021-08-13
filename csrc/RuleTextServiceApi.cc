// TODO: investigate potential migration to Node-API for compatibility between v8 versions.

#include <node_api.h>
#include <vector>
#include <unordered_set>

#include "RuleNode.h"
#include "RuleTextProcessor.h"

using namespace ruletxt;

class AddonData
{
private:
    napi_env env;

public:
    explicit AddonData(napi_env envp)
    {
        env = envp;
    }
    /**
     * Sructure of "std::vector<RuleNode> rules"
     * rules:
     * - list<RuleNode> (rules) -> list<RuleNode> (subrules) // rules for chapter at index 0
     * - list<RuleNode> -> list<RuleNode>
     * ...
     * - list<RuleNode> -> list<RuleNode> // rules for chapter at the last index
     * 
    **/
    std::vector<RuleNode> rules;

    std::unordered_set<RuleNode> rulesHash;

    static void DeleteInstance(void *data)
    {
        delete static_cast<AddonData *>(data);
    }

    napi_value object_from_rule_node(RuleNode r)
    {
        napi_value chapterObj;
        napi_create_object(env, &chapterObj);

        napi_value name;
        napi_create_string_utf8(env, "name", NAPI_AUTO_LENGTH, &name);
        napi_value contents;
        napi_create_string_utf8(env, "contents", NAPI_AUTO_LENGTH, &contents);
        napi_value children;
        napi_create_string_utf8(env, "children", NAPI_AUTO_LENGTH, &children);

        napi_value nameValue;
        napi_create_string_utf8(env, r.name.c_str(), NAPI_AUTO_LENGTH, &nameValue);
        napi_value contentsValue;
        napi_create_string_utf8(env, r.contents.c_str(), NAPI_AUTO_LENGTH, &contentsValue);
        napi_value childrenValue;
        napi_create_array_with_length(env, r.children.size(), &childrenValue);

        uint32_t i = 0;
        for (auto it = r.children.begin(); it != r.children.end(); it++)
        {
            napi_value InnerChapterObj = object_from_rule_node(*it);
            napi_set_element(env, childrenValue, i, InnerChapterObj);
            i++;
        }

        napi_set_property(env, chapterObj, name, nameValue);
        napi_set_property(env, chapterObj, contents, contentsValue);
        napi_set_property(env, chapterObj, children, childrenValue);

        return chapterObj;
    }
};

napi_value Method(napi_env env, napi_callback_info args)
{
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, NULL, NULL);
    AddonData *data = ;

    RuleTextProcessor p("text.txt");
    data->rules = p.find_rules();

    v8::Local<v8::Array> fullRules = v8::Array::New(isolate);

    int i = 0;
    for (auto it = data->rules.begin(); it != data->rules.end(); it++)
    {
        v8::Local<v8::Object> chapterObj = data->object_from_rule_node(*it);
        auto result = fullRules->Set(context, i, chapterObj);
        result.Check();
        i++;
    }

    info.GetReturnValue().Set(fullRules);
}

// Initialize this addon to be context-aware.
napi_value init(napi_env env, napi_value exports)
{
    napi_status status;
    napi_value fn;

    AddonData *data = new AddonData(env);

    status = napi_create_function(env, nullptr, 0, Method, data, &fn);
    if (status != napi_ok)
        return nullptr;

    status = napi_set_named_property(env, exports, "method", fn);
    if (status != napi_ok)
        return nullptr;
    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init)
}