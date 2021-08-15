#include <nan.h>
#include <vector>
#include <unordered_set>

#include "RuleNode.h"
#include "RuleTextProcessor.h"

using namespace ruletxt;

class AddonData
{
private:
    v8::Isolate *isolate;

public:
    explicit AddonData(v8::Isolate *isolatep)
    {
        // Ensure this per-addon-instance data is deleted at environment cleanup.
        isolate = isolatep;
        node::AddEnvironmentCleanupHook(isolate, DeleteInstance, this);
    }
    /**
     * Sructure of "std::vector<RuleNode> rules"
     * rules: // contains sections
     * - list<RuleNode> (chpaters) -> list<RuleNode> (rules) -> list<RuleNode> (subrules) // rules for chapter at section 1
     * - list<RuleNode> -> list<RuleNode> -> list<RuleNode>
     * ...
     * - list<RuleNode> -> list<RuleNode> -> list<RuleNode> // rules for chapter at the last section
     * 
    **/
    std::vector<RuleNode> rules;

    v8::Local<v8::Array> rulesJS;

    std::unordered_set<RuleNode> rulesHash;
    std::unordered_set<RuleNode> chaptersHash;

    static void DeleteInstance(void *data)
    {
        delete static_cast<AddonData *>(data);
    }

    v8::Local<v8::Object> object_from_rule_node(RuleNode r, std::string filter)
    {
        v8::Local<v8::Object> chapterObj = Nan::New<v8::Object>();

        v8::Local<v8::String> nameKey = Nan::New("name").ToLocalChecked();
        v8::Local<v8::String> contentsKey = Nan::New("contents").ToLocalChecked();
        v8::Local<v8::String> childrenKey = Nan::New("children").ToLocalChecked();

        v8::Local<v8::Value> nameValue = Nan::New(r.name).ToLocalChecked();
        v8::Local<v8::Value> contentsValue = Nan::New(r.contents).ToLocalChecked();
        v8::Local<v8::Array> childrenValue = v8::Array::New(isolate);

        int i = 0;
        if (r.type == "rule" || r.type == "chapter")
        { // Rules and Subrules processing from chapter
            if (filter.length() > 0)
            {
                for (auto it = r.children.begin(); it != r.children.end(); it++)
                {
                    std::string contents = it->contents;
                    std::for_each(contents.begin(), contents.end(), [](char &c)
                                  { c = ::tolower(c); });
                    if (r.type != "chapter" && contents.find(filter) != std::string::npos)
                    {
                        v8::Local<v8::Object> chapterObj = object_from_rule_node(*it, filter);
                        auto result = childrenValue->Set(isolate->GetCurrentContext(), i, chapterObj);
                        result.Check();
                        i++;
                    }
                    else if (r.type == "chapter")
                    {
                        v8::Local<v8::Object> chapterObj = object_from_rule_node(*it, filter);
                        v8::Local<v8::Array> children = v8::Local<v8::Array>::Cast(chapterObj->Get(isolate->GetCurrentContext(), Nan::New("children").ToLocalChecked()).ToLocalChecked());
                        if (children->Length() != 0)
                        {
                            auto result = childrenValue->Set(isolate->GetCurrentContext(), i, chapterObj);
                            result.Check();
                            i++;
                        }
                    }
                }
            }
            else
            {
                if (r.type == "chapter")
                {
                    if (chaptersHash.find(r) == chaptersHash.end())
                        chaptersHash.insert(r);
                }
                else
                {
                    if (rulesHash.find(r) == rulesHash.end())
                        rulesHash.insert(r);
                }
                for (auto it = r.children.begin(); it != r.children.end(); it++)
                {
                    v8::Local<v8::Object> chapterObj = object_from_rule_node(*it, filter);
                    auto result = childrenValue->Set(isolate->GetCurrentContext(), i, chapterObj);
                    result.Check();
                    i++;
                }
            }
        }
        else
        { // Chapter processing from section
            for (auto it = r.children.begin(); it != r.children.end(); it++)
            {
                v8::Local<v8::Object> chapterObj = object_from_rule_node(*it, filter);
                v8::Local<v8::Array> children = v8::Local<v8::Array>::Cast(chapterObj->Get(isolate->GetCurrentContext(), Nan::New("children").ToLocalChecked()).ToLocalChecked());
                if (children->Length() > 0)
                {
                    auto result = childrenValue->Set(isolate->GetCurrentContext(), i, chapterObj);
                    result.Check();
                    i++;
                }
            }
        }

        Nan::Set(chapterObj, nameKey, nameValue);
        Nan::Set(chapterObj, contentsKey, contentsValue);
        Nan::Set(chapterObj, childrenKey, childrenValue);

        return chapterObj;
    }

    v8::Local<v8::Object> object_from_rule_node(RuleNode r)
    {
        return object_from_rule_node(r, "");
    }
};

static void open(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    v8::Isolate *isolate = info.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    if (info.Length() != 1)
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong number of arguments. Expected one string argument.")
                .ToLocalChecked()));
        return;
    }

    if (!info[0]->IsString())
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong argument. Expected one string argument.").ToLocalChecked()));
        return;
    }
    // Retrieve the per-addon-instance data.
    AddonData *data =
        reinterpret_cast<AddonData *>(info.Data().As<v8::External>()->Value());

    v8::String::Utf8Value file_path(isolate, info[0]);

    RuleTextProcessor p(*file_path);
    if (p.failed())
    {
        isolate->ThrowException(v8::Exception::Error(
            v8::String::NewFromUtf8(isolate, p.failed_msg()).ToLocalChecked()));
        return;
    }
    data->rules = p.find_rules();

    data->rulesJS = v8::Array::New(isolate);

    if (!data->rulesHash.empty())
        data->rulesHash.clear();

    int i = 0;
    for (auto it = data->rules.begin(); it != data->rules.end(); it++)
    {
        v8::Local<v8::Object> chapterObj = data->object_from_rule_node(*it);
        auto result = data->rulesJS->Set(context, i, chapterObj);
        result.Check();
        i++;
    }

    info.GetReturnValue().Set(data->rulesJS);
}

static void rules_containing(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    v8::Isolate *isolate = info.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    if (info.Length() != 1)
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong number of arguments. Expected one string argument.")
                .ToLocalChecked()));
        return;
    }

    if (!info[0]->IsString())
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong argument. Expected one string argument.").ToLocalChecked()));
        return;
    }

    // Retrieve the per-addon-instance data.
    AddonData *data =
        reinterpret_cast<AddonData *>(info.Data().As<v8::External>()->Value());

    v8::String::Utf8Value str(isolate, info[0]);
    std::string filter(*str);
    std::for_each(filter.begin(), filter.end(), [](char &c)
                  { c = ::tolower(c); });

    v8::Local<v8::Array> filtered_rules = v8::Array::New(isolate);

    int i = 0;
    for (auto it = data->rules.begin(); it != data->rules.end(); it++)
    {
        v8::Local<v8::Object> chapterObj = data->object_from_rule_node(*it, filter);
        v8::Local<v8::Array> children = v8::Local<v8::Array>::Cast(chapterObj->Get(isolate->GetCurrentContext(), Nan::New("children").ToLocalChecked()).ToLocalChecked());
        if (children->Length() > 0)
        {
            auto result = filtered_rules->Set(context, i, chapterObj);
            result.Check();
            i++;
        }
    }

    info.GetReturnValue().Set(filtered_rules);
}

static void rule(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    v8::Isolate *isolate = info.GetIsolate();

    if (info.Length() != 1)
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong number of arguments. Expected one string argument.")
                .ToLocalChecked()));
        return;
    }

    if (!info[0]->IsString())
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong argument. Expected one string argument.").ToLocalChecked()));
        return;
    }
    // Retrieve the per-addon-instance data.
    AddonData *data =
        reinterpret_cast<AddonData *>(info.Data().As<v8::External>()->Value());

    v8::String::Utf8Value str(isolate, info[0]);

    std::string rule_id(*str);

    RuleNode r;
    r.name = rule_id;

    auto it = data->rulesHash.find(r);

    if (it != data->rulesHash.end())
        info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, it->contents.c_str()).ToLocalChecked());
    else
        info.GetReturnValue().Set(Nan::Undefined());
}

static void chapter(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    v8::Isolate *isolate = info.GetIsolate();

    if (info.Length() != 1)
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong number of arguments. Expected one string argument.")
                .ToLocalChecked()));
        return;
    }

    if (!info[0]->IsString())
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong argument. Expected one string argument.").ToLocalChecked()));
        return;
    }
    // Retrieve the per-addon-instance data.
    AddonData *data =
        reinterpret_cast<AddonData *>(info.Data().As<v8::External>()->Value());

    v8::String::Utf8Value str(isolate, info[0]);

    std::string chapter_id(*str);

    RuleNode r;
    r.name = chapter_id;

    auto it = data->chaptersHash.find(r);

    if (it != data->chaptersHash.end())
    {
        v8::Local<v8::Object> chapterObj = data->object_from_rule_node(*it);
        info.GetReturnValue().Set(chapterObj);
    }
    else
    {
        info.GetReturnValue().Set(Nan::Undefined());
    }
}

static void chapter_rules_containing(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    v8::Isolate *isolate = info.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    if (info.Length() != 2)
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong number of arguments. Expected two string arguments.")
                .ToLocalChecked()));
        return;
    }

    if (!info[0]->IsString() && !info[1]->IsString())
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong argument. Expected two string arguments.").ToLocalChecked()));
        return;
    }

    // Retrieve the per-addon-instance data.
    AddonData *data =
        reinterpret_cast<AddonData *>(info.Data().As<v8::External>()->Value());

    v8::String::Utf8Value str1(isolate, info[0]);
    v8::String::Utf8Value str2(isolate, info[1]);
    std::string chapter_id(*str1);
    std::string filter(*str2);
    std::for_each(filter.begin(), filter.end(), [](char &c)
                  { c = ::tolower(c); });

    RuleNode r;
    r.name = chapter_id;

    auto it = data->chaptersHash.find(r);

    if (it != data->chaptersHash.end())
    {
        v8::Local<v8::Object> chapterObj = data->object_from_rule_node(*it, filter);
        info.GetReturnValue().Set(chapterObj);
    }
    else
    {
        info.GetReturnValue().Set(Nan::Undefined());
    }
}

// Initialize this addon to be context-aware.
extern "C" NODE_MODULE_EXPORT void
NODE_MODULE_INITIALIZER(v8::Local<v8::Object> exports,
                        v8::Local<v8::Object> module,
                        v8::Local<v8::Context> context)
{
    v8::Isolate *isolate = context->GetIsolate();

    // Create a new instance of `AddonData` for this instance of the addon and
    // tie its life cycle to that of the Node.js environment.
    AddonData *data = new AddonData(isolate);

    // Wrap the data in a `v8::External` so we can pass it to the method we
    // expose.
    v8::Local<v8::External> external = v8::External::New(isolate, data);

    // Expose the method `Method` to JavaScript, and make sure it receives the
    // per-addon-instance data we created above by passing `external` as the
    // third parameter to the `FunctionTemplate` constructor.
    exports->Set(context,
                 v8::String::NewFromUtf8(isolate, "open").ToLocalChecked(),
                 v8::FunctionTemplate::New(isolate, open, external)
                     ->GetFunction(context)
                     .ToLocalChecked())
        .FromJust();

    exports->Set(context,
                 v8::String::NewFromUtf8(isolate, "rule").ToLocalChecked(),
                 v8::FunctionTemplate::New(isolate, rule, external)
                     ->GetFunction(context)
                     .ToLocalChecked())
        .FromJust();
    exports->Set(context,
                 v8::String::NewFromUtf8(isolate, "chapter").ToLocalChecked(),
                 v8::FunctionTemplate::New(isolate, chapter, external)
                     ->GetFunction(context)
                     .ToLocalChecked())
        .FromJust();

    exports->Set(context,
                 v8::String::NewFromUtf8(isolate, "rules_containing").ToLocalChecked(),
                 v8::FunctionTemplate::New(isolate, rules_containing, external)
                     ->GetFunction(context)
                     .ToLocalChecked())
        .FromJust();

    exports->Set(context,
                 v8::String::NewFromUtf8(isolate, "chapter_rules_containing").ToLocalChecked(),
                 v8::FunctionTemplate::New(isolate, chapter_rules_containing, external)
                     ->GetFunction(context)
                     .ToLocalChecked())
        .FromJust();
}