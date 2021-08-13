#include <nan.h>
#include <algorithm>
#include <vector>
#include <utility>
#include <iostream>
#include <fstream>
#include <cstring>

// for mmap:
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

namespace demo
{
    using v8::ArrayBuffer;
    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::Local;
    using v8::Number;
    using v8::Object;
    using v8::Value;

    void handle_error(const char *msg)
    {
        perror(msg);
        exit(255);
    }

    char *map_file(const char *fname, size_t &length)
    {
        int fd = open(fname, O_RDONLY);
        if (fd == -1)
            handle_error("open");

        // obtain file size
        struct stat sb;
        if (fstat(fd, &sb) == -1)
            handle_error("fstat");

        length = sb.st_size;

        char *addr = static_cast<char *>(mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0u));
        if (addr == MAP_FAILED)
            handle_error("mmap");

        int closed = close(fd);
        if (closed == -1)
            handle_error("close");
        return addr;
    }

    std::vector<std::pair<char *, char *>> find_chapters_for_section(const char *section)
    {
        size_t rules_txt_length;
        const char *numbers = "0123456789";
        char *rules_txt = map_file("text.txt", rules_txt_length);
        char *rules_l = rules_txt + rules_txt_length;

        char *chapter = (char *)malloc(5);
        strcpy(chapter, "\n000");
        chapter[1] = section[0];

        char *chapter_start = strstr(rules_txt, chapter);
        chapter_start--;

        char *found, *found_end = chapter_start, *rule_index, *rule_text, *found_index_end;
        std::vector<std::pair<char *, char *>> result = {};
        unsigned char rule_number = 0;

        while (found_end != NULL)
        {
            chapter[2] = numbers[rule_number / 10 % 10];
            chapter[3] = numbers[rule_number % 10];

            found = strstr(found_end, chapter);
            if (found == NULL)
                break;
            found++;
            found_index_end = strstr(found, " ");
            if (found_index_end == NULL)
                break;
            found_end = strstr(found, "\n") - 1;
            if (found_end == NULL)
                break;

            rule_index = new char[found_index_end - found + 1];
            strncpy(rule_index, found, found_index_end - found);
            rule_index[found_index_end - found] = 0;

            rule_text = new char[found_end - found + 1];
            strncpy(rule_text, found, found_end - found);
            rule_text[found_end - found] = 0;
            result.push_back(std::make_pair(rule_index, rule_text));
            rule_number++;
        }

        return result;
    }

    void Method(const FunctionCallbackInfo<Value> &args)
    {
        Isolate *isolate = args.GetIsolate();
        // auto res = find_chapters_for_section("1");
        // for (int i = 0; i < res.size(); i++) {
        //     char* rule = res.at(i).second;
        //     auto bufferobj = Nan::NewBuffer(
        //         (char *)(res.at(i).second),
        //         strlen(res.at(i).second)+1
        //     ).ToLocalChecked();
        //     elements[i] = Nan::New(5);
        // }
        v8::Local<v8::Array> jsRes = v8::Array::New(isolate, 6);
        jsRes->Set(1, v8::Number::New(isolate, 5));
        args.GetReturnValue().Set(jsRes);
    }

    void Initialize(Local<Object> exports)
    {
        NODE_SET_METHOD(exports, "test", Method);
    }

    NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)

}