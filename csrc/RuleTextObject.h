/* TODO: Develop an organized wrapped C++ object for better code readibility and data conversion efficiency */

#ifndef RULE_TEXT_OBJECT_H
#define RULE_TEXT_OBJECT_H

#include <node.h>
#include <node_object_wrap.h>

namespace ruletxt
{

    class RuleTextObject : public node::ObjectWrap
    {
    public:
        static void Init(v8::Local<v8::Object> exports);

    private:
        explicit MyObject(double value = 0);
        ~MyObject();

        static void New(const v8::FunctionCallbackInfo<v8::Value> &args);
        static void PlusOne(const v8::FunctionCallbackInfo<v8::Value> &args);

        double value_;
    };

} // namespace demo

#endif