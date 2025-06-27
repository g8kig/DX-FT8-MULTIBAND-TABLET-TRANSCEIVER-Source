
#include <string.h>
#include <stdbool.h>

#include "ini.h"

#define min(a, b) ((a) < (b)) ? (a) : (b)

// Basic function to check if a character is whitespace (space or tab)
static bool is_whitespace(char c)
{
    return ((c == ' ') || (c == '\t') || (c == '\r'));
}

// Function to trim leading and trailing whitespace from a "string"
static void str_trim(char *str)
{
    char *start = str;
    while (is_whitespace(*start))
    {
        start++;
    }

    char *end = str + strlen(str) - 1;
    while (end > start && is_whitespace(*end))
    {
        end--;
    }

    char *p = str;
    while (start <= end)
    {
        *p++ = *start++;
    }
    *p = 0;
}

static void copy_and_trim(char *dest, const char *src, size_t len)
{
    memcpy(dest, src, len);
    dest[len] = 0;
    str_trim(dest);
}

void parse_ini(const char *buffer, size_t buffer_len, ini_data_t *ini_data)
{
    ini_data->num_sections = 0;
    int current_section = -1;
    const char *current_pos = buffer;
    const char *buffer_end = buffer + buffer_len;

    while (current_pos < buffer_end)
    {
        const char *line_start = current_pos;
        while (current_pos < buffer_end && *current_pos != '\n')
        {
            current_pos++;
        }

        char line[MAX_LINE_LENGTH];
        copy_and_trim(line, line_start, min((size_t)(current_pos - line_start), sizeof(line) - 1));

        // Handle comments
        if (*line == ';' || *line == '#' || strlen(line) == 0)
        {
            // Skip comment or empty line
        }
        else if (*line == '[' && line[strlen(line) - 1] == ']')
        {
            // Handle section
            if (ini_data->num_sections < MAX_SECTIONS)
            {
                current_section = ini_data->num_sections;
                ini_section_t *section = &ini_data->sections[current_section];

                section->num_entries = 0;
                section->present = true;

                copy_and_trim(section->name, line + 1, min(strlen(line) - 2, sizeof(section->name) - 1));
                ini_data->num_sections++;
            }
            else
            {
                current_section = -1; // Too many sections
            }
        }
        else if (current_section != -1)
        {
            // Handle key-value pair
            const char *equals_pos = NULL;
            for (size_t i = 0; i < strlen(line); i++)
            {
                if (line[i] == '=')
                {
                    equals_pos = line + i;
                    break;
                }
            }

            ini_section_t *current_section_ptr = &ini_data->sections[current_section];
            if (equals_pos != NULL && current_section_ptr->num_entries < MAX_KEYS_PER_SECTION)
            {
                ini_entry_t *entry = &current_section_ptr->entries[ini_data->sections[current_section].num_entries++];

                copy_and_trim(entry->key, line, min((size_t)(equals_pos - line), sizeof(entry->key) - 1));
                copy_and_trim(entry->value, equals_pos + 1, min((size_t)(buffer_end - equals_pos + 1), sizeof(entry->value) - 1));

                entry->present = true;
            }
        }

        // Move to the next line
        if (current_pos < buffer_end && *current_pos == '\n')
        {
            current_pos++;
        }
    }
}

const char *get_ini_value(const ini_data_t *ini_data, const char *section_name, const char *entry_key)
{
    for (unsigned int i = 0; i < ini_data->num_sections; i++)
    {
        const ini_section_t *section = ini_data->sections + i;
        if (section->present && strcmp(section->name, section_name) == 0)
        {
			return get_ini_value_from_section(section, entry_key);
        }
    }
    return NULL;
}

const ini_section_t *get_ini_section(const ini_data_t *ini_data, const char *section_name)
{
    for (unsigned int i = 0; i < ini_data->num_sections; i++)
    {
        const ini_section_t *section = ini_data->sections + i;
        if (section->present && strcmp(section->name, section_name) == 0)
        {
            return section;
        }
    }
    return NULL;
}

const char *get_ini_value_from_section(const ini_section_t *section, const char *entry_key)
{
    for (unsigned int j = 0; j < section->num_entries; j++)
    {
        const ini_entry_t *entry = section->entries + j;
        if (entry->present && strcmp(entry->key, entry_key) == 0)
            return entry->value;
    }
    return NULL;
}
