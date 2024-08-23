#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include <cassert>
#include <cctype>
#include <algorithm>

namespace JsonParser
{
    using text_it = std::string::iterator;

    union JsonValue
    {
        int i;
        std::string *s;
    };

    void ReadFile(const std::string &filepath, std::string &output)
    {
        std::ifstream file(filepath);
        std::string line;

        while (std::getline(file, line))
        {
            output.append(line); // Append line by line to the output string
        }
    }

    JsonValue ParsePrimitive(const std::string &text, text_it start, text_it end)
    {
        std::string substr = text.substr(start - text.begin(), end - start);
        JsonValue v;
        // Check if it's an integer
        if (std::all_of(substr.begin(), substr.end(), ::isdigit))
        {
            v.i = std::stoi(substr);
        }
        else
        { // Otherwise, treat it as a string
            v.s = new std::string(substr);
        }
        return v;
    }

    std::pair<std::string, JsonValue> RetrieveKeyValuePair(
        const std::string &text,
        text_it &it)
    {
        assert(it != text.end());

        // Ignore white spaces and line breaks
        while (*it == ' ' || *it == '\n')
        {
            it++;
        }

        text_it curr_it;
        std::string key;
        JsonValue value;

        // Parse the key
        if (*it == '\"')
        {
            curr_it = ++it;
            while (*it != '\"')
            {
                it++;
            }
            key = text.substr(curr_it - text.begin(), it - curr_it);
            assert(*(++it) == ':'); // Assert that we have a key-value separator ':'
            it++;
        }

        // Ignore white spaces and line breaks
        while (*it == ' ' || *it == '\n')
        {
            it++;
        }

        // Parse the value (primitive: int or string)
        if (*it == '\"')
        { // Handle string value
            curr_it = ++it;
            while (*it != '\"')
            {
                it++;
            }
            value.s = new std::string(text.substr(curr_it - text.begin(), it - curr_it));
            it++;
        }
        else
        { // Handle integer value
            curr_it = it;
            while (isdigit(*it))
            {
                it++;
            }
            value = ParsePrimitive(text, curr_it, it);
        }

        // After parsing the value, check whether the current iterator points to a comma
        if (*it == ',')
        {
            it++;
        }

        return std::make_pair(key, value);
    }

    std::vector<std::pair<std::string, JsonValue>> ParseJsonObject(
        const std::string &text,
        text_it &it)
    {
        std::vector<std::pair<std::string, JsonValue>> object;

        assert(*it == '{'); // Must start with the left curly bracket
        it++;

        while (*it != '}')
        {
            std::pair<std::string, JsonValue> a = RetrieveKeyValuePair(text, it);
            object.push_back(a);

            // Skip white spaces and line breaks
            while (*it == ' ' || *it == '\n')
            {
                it++;
            }

            if (*it == ',')
            {
                it++;
            }
        }

        it++; // Move past the closing brace '}'
        return object;
    }

    std::vector<std::vector<std::pair<std::string, JsonValue>>> ParseJsonArray(
        const std::string &text,
        text_it &it)
    {
        std::vector<std::vector<std::pair<std::string, JsonValue>>> array;

        assert(*it == '['); // Must start with the left square bracket
        it++;

        while (*it != ']')
        {
            // Ignore white spaces and line breaks
            while (*it == ' ' || *it == '\n')
            {
                it++;
            }

            if (*it == '{')
            {
                auto object = ParseJsonObject(text, it);
                array.push_back(object);

                // Skip white spaces and line breaks
                while (*it == ' ' || *it == '\n')
                {
                    it++;
                }

                if (*it == ',')
                {
                    it++;
                }
            }
        }

        it++; // Move past the closing bracket ']'
        return array;
    }

    std::vector<std::vector<std::pair<std::string, JsonValue>>> ParseJson(const std::string &filepath)
    {
        // Read the text data from the given file
        std::string text;
        ReadFile(filepath, text);

        text_it it = text.begin();
        std::vector<std::vector<std::pair<std::string, JsonValue>>> array;
        if (*it == '[')
        {
            array = ParseJsonArray(text, it);
        }
        return array;
    }
}
