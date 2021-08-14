#include "RuleTextProcessor.h"

using namespace ruletxt;

RuleTextProcessor::RuleTextProcessor(const char *rules_file_name)
{
    read_fail = false;
    rules_file = rules_file_name;
    read_rules_text();
}

RuleTextProcessor::~RuleTextProcessor()
{
    delete[] contents_txt;
}

void RuleTextProcessor::read_rules_text()
{
    rules_txt = map_file(rules_file, rules_txt_length);
    if (rules_txt == nullptr)
    {
        contents_txt = new char[1]{'\0'};
        return;
    }

    char *rules_begin = strstr(rules_txt, "\nGlossary");
    char *rules_end;
    if (rules_begin != NULL)
    {
        rules_begin = strstr(rules_begin, "\n");
        rules_end = strstr(rules_begin, "\nGlossary");
    }

    if (rules_begin != NULL && rules_end != NULL)
    {
        contents_txt = new char[rules_begin - rules_txt + 1];
        strncpy(contents_txt, rules_txt, rules_begin - rules_txt);
        contents_txt[rules_begin - rules_txt] = 0;
        memmove(rules_txt, rules_begin, rules_end - rules_begin);
    }
    else
    {
        contents_txt = new char[1]{'\0'};
        rules_txt = nullptr;
    }
    rules_txt_initial = rules_txt;
}

void RuleTextProcessor::handle_file_error(const char *msg)
{
    read_fail = true;
    fail_msg = msg;
}

bool RuleTextProcessor::failed()
{
    return read_fail;
}

const char *RuleTextProcessor::failed_msg()
{
    return read_fail ? fail_msg : "";
}

char *RuleTextProcessor::map_file(const char *fname, size_t &length)
{
    int fd = open(fname, O_RDONLY);
    if (fd == -1)
    {
        handle_file_error("Failed opening the file");
        return nullptr;
    }

    // obtain file size
    struct stat sb;
    if (fstat(fd, &sb) == -1)
    {
        handle_file_error("Failed retrieving file size");
        return nullptr;
    }

    length = sb.st_size;

    char *addr = static_cast<char *>(mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0u));
    if (addr == MAP_FAILED)
    {
        handle_file_error("Failed mapping the file onto a variable.");
        return nullptr;
    }

    int closed = close(fd);
    if (closed == -1)
    {
        handle_file_error("Failed closing the file.");
        return nullptr;
    }
    return addr;
}

char *RuleTextProcessor::create_initial_chapter(const char *section, size_t &length)
{
    size_t section_length = strlen(section);
    length = section_length + 4;
    char *chapter = new char[length];

    const char *endl = "\n";
    const char *chapter_zero_tail = "00\0";

    strcpy(chapter, endl);
    strcat(chapter, section);
    strcat(chapter, chapter_zero_tail);
    return chapter;
}

char *RuleTextProcessor::create_searchable_section(const char *section)
{
    size_t section_length = strlen(section);
    size_t length = section_length + 4;
    char *section_new = new char[length];

    const char *endl = "\n";
    const char *section_tail = ". ";

    strcpy(section_new, endl);
    strcat(section_new, section);
    strcat(section_new, section_tail);
    return section_new;
}

void RuleTextProcessor::to_string_cpy(std::string &str, char *begin, char *end)
{
    char contents[end - begin + 1];
    strncpy(contents, begin, end - begin);
    contents[end - begin] = 0;
    str = std::string(contents);
}

void RuleTextProcessor::to_string_cpy(std::string &str, char *that)
{
    size_t len = strlen(that);
    char contents[len + 1];
    strcpy(contents, that);
    contents[len] = 0;
    str = std::string(contents);
}

void RuleTextProcessor::to_string_cpy(std::string &str, const char *that)
{
    size_t len = strlen(that);
    char contents[len + 1];
    strcpy(contents, that);
    contents[len] = 0;
    str = std::string(contents);
}

RuleNode RuleTextProcessor::find_section_chapters(const char *section)
{
    const char *numbers = "0123456789";
    size_t chapter_length;
    char *chapter = create_initial_chapter(section, chapter_length);
    RuleNode sectionNode;
    char *serchable_section = create_searchable_section(section);
    char *section_start = strstr(contents_txt, serchable_section);
    if (section_start == NULL)
    {
        delete[] serchable_section;
        delete[] chapter;
        return sectionNode;
    }
    section_start++;
    char *section_end = strstr(section_start, "\n");
    to_string_cpy(sectionNode.name, section);
    to_string_cpy(sectionNode.contents, section_start, section_end);

    char *chapter_start = strstr(contents_txt, chapter);

    char *found, *found_end = chapter_start;
    std::vector<RuleNode> result;
    unsigned char rule_number = 0;

    while (found_end != NULL)
    {
        chapter[chapter_length - 3] = numbers[rule_number / 10 % 10];
        chapter[chapter_length - 2] = numbers[rule_number % 10];

        found = strstr(found_end, chapter);
        if (found == NULL)
            break;
        found++;
        found_end = strstr(found, "\n");
        if (found_end == NULL)
            break;

        RuleNode s;
        to_string_cpy(s.name, chapter + 1);
        to_string_cpy(s.contents, found, found_end);
        s.type = "chapter";

        result.push_back(s);
        rule_number++;
    }
    delete[] serchable_section;
    delete[] chapter;
    sectionNode.children = result;
    sectionNode.type = "section";

    return sectionNode;
}

std::vector<RuleNode> RuleTextProcessor::find_chapters()
{
    std::vector<const char *> sections = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"};
    std::vector<RuleNode> chapters = {};
    for (auto it = sections.begin(); it != sections.end(); it++)
    {
        RuleNode res = find_section_chapters(*it);
        if (res.children.empty())
            break;
        chapters.push_back(res);
    }
    return chapters;
}

char *RuleTextProcessor::create_rule_id(std::string chapter, int number, size_t &length)
{
    const char *endl = "\n";
    const char *delimiter = ".";
    std::string rule_number = std::to_string(number);

    length = 3 + chapter.length() + rule_number.length();
    char *rule = new char[length];

    strcpy(rule, endl);
    strcat(rule, chapter.c_str());
    strcat(rule, delimiter);
    strcat(rule, rule_number.c_str());
    return rule;
}

char *RuleTextProcessor::create_subrule_id(char *rule, int number, size_t &length)
{
    const char *endl = "\n";
    char subrule_char = 'a';
    subrule_char += number - 1;

    length = strlen(rule) + 3;
    char *subrule = new char[length];

    strcpy(subrule, endl);
    strcat(subrule, rule);
    subrule[length - 2] = subrule_char;
    subrule[length - 1] = 0;
    return subrule;
}

std::vector<RuleNode> RuleTextProcessor::find_chapter_rules(std::string chapter)
{
    char *found, *found_end = rules_txt;
    std::vector<RuleNode> result;
    for (int i = 1; i < 1000; i++)
    {
        size_t rule_length;
        char *rule_id = create_rule_id(chapter, i, rule_length);
        found = strstr(found_end, rule_id);
        if (found == NULL)
        {
            delete[] rule_id;
            break;
        }
        found++;
        found_end = strstr(found, "\n");
        if (found_end == NULL)
        {
            delete[] rule_id;
            break;
        }

        rules_txt = found;
        RuleNode s;
        to_string_cpy(s.name, rule_id + 1);
        to_string_cpy(s.contents, found, found_end);
        s.type = "rule";

        for (int j = 1; j <= 26; j++)
        {
            if (j == 12 || j == 15)
                continue;
            size_t subrule_length;
            char *subrule_id = create_subrule_id(rule_id + 1, j, subrule_length);
            found = strstr(found_end, subrule_id);
            if (found == NULL)
            {
                delete[] subrule_id;
                break;
            }

            found++;
            found_end = strstr(found, "\n");
            if (found_end == NULL)
            {
                delete[] subrule_id;
                break;
            }

            rules_txt = found;
            RuleNode c;
            to_string_cpy(c.name, subrule_id + 1);
            to_string_cpy(c.contents, found, found_end);
            c.type = "rule";
            s.children.push_back(c);
            delete[] subrule_id;
        }
        delete[] rule_id;

        result.push_back(s);
    }
    return result;
}

std::vector<RuleNode> RuleTextProcessor::find_rules()
{
    std::vector<RuleNode> sections;
    if (rules_txt == nullptr)
        return sections;
    sections = find_chapters();
    for (auto section = sections.begin(); section != sections.end(); section++)
    {
        for (auto chapter = section->children.begin(); chapter != section->children.end(); chapter++)
        {
            std::vector<RuleNode> rules = find_chapter_rules(chapter->name);
            chapter->children = rules;
        }
    }
    return sections;
}