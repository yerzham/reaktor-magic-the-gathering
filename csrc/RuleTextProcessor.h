#ifndef RULE_TEXT_PROCESSOR
#define RULE_TEXT_PROCESSOR
#include "RuleNode.h"
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <stdlib.h>

// for mmap:
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

namespace ruletxt
{
    class RuleTextProcessor
    {
    private:
        /**
         * Exits the program with a file access related error.
         */
        void handle_file_error(const char *msg);

        /**
         * Maps file content to a given char pointer.
         */
        char *map_file(const char *fname, size_t &length);

        /**
         * Adds "\n" and "00" to the section id to create an initial chapter id. For example: "2" -> "\n200".
         */
        char *create_initial_chapter(const char *section, size_t &length);

        /**
         * Adds "\n" and ". " to the section id to create a serchable id. For example: "2" -> "\n2. ".
         */
        char *create_searchable_section(const char *section);

        /**
         * Adds "\n"  and ".{`number`}" to the chapter id to create a rule id. For example: "200" -> "\n200.1".
         */
        char *create_rule_id(std::string chapter, int number, size_t &length);

        /**
         * Adds "`alphabet[number]`" to the rule id to create a subrule id. For example: "\n200.1" -> "\n200.1a".
         */
        char *create_subrule_id(char *rule, int number, size_t &length);

        /**
         * Copies character array content to a given string.
         */
        void to_string_cpy(std::string &str, char *begin, char *end);
        void to_string_cpy(std::string &str, char *that);
        void to_string_cpy(std::string &str, const char *that);

        /**
         * Returns a section ruleNode containing chapters for a given section.
         */
        RuleNode find_section_chapters(const char *section);

        /**
         * Returns a vector of section chapters for sections from "1" up to "15".
         */
        std::vector<RuleNode> find_chapters();

        /**
         * Returns a list of rule nodes with subrules for a given chapter.
         */
        std::vector<RuleNode> find_chapter_rules(std::string chapter);

        /**
         * Reads the rules text file.
         *
         * Reads the text file `rules_file` and writes contents to `contents_txt` and rules to `rules_txt`.
         */
        void read_rules_text();

        bool read_fail;
        const char *fail_msg;
        const char *rules_file;
        char *contents_txt;
        char *rules_txt_initial;
        char *rules_txt;
        size_t rules_txt_length;

    public:
        // Constructor.
        RuleTextProcessor(const char *rules_file_name);

        // Destructor.
        ~RuleTextProcessor();

        /**
         * Returns a vector of sections containing all chapters, rules, and subrules.
         */
        std::vector<RuleNode> find_rules();

        /**
         * Returns whether file read failed.
         */
        bool failed();

        /**
         * Returns failure message.
         */
        const char *failed_msg();
    };
}
#endif // RULE_TEXT_PROCESSOR