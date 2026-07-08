// Copyright (C) Benjamín Gajardo (also known as +KZ)

#include <engine/storage.h>

#include "emoji_kz.h"
#include <base/log.h>
#include <engine/external/json-parser/json.h>
#include <base/str.h>

void CEmojiKZ::Init(IKernel *pKernel)
{
    if(!pKernel)
        return;

    IStorage *pStorage = pKernel->RequestInterface<IStorage>();
    if(!pStorage)
        return;

    // used code from text.cpp
    // read file data into buffer
    const char *pFilename = "kz/emojis.json";
    void *pFileData;
    unsigned JsonFileSize;
    if(!pStorage->ReadFile(pFilename, IStorage::TYPE_ALL, &pFileData, &JsonFileSize))
    {
        log_error("emoji_kz", "Failed to open/read file '%s'", pFilename);
        return;
    }

    // parse json data
    json_settings JsonSettings{};
    char aError[256];
    json_value *pJsonData = json_parse_ex(&JsonSettings, static_cast<const json_char *>(pFileData), JsonFileSize, aError);
    free(pFileData);
    if(pJsonData == nullptr)
    {
        log_error("emoji_kz", "Failed to parse file '%s': %s", pFilename, aError);
        return;
    }
    if(pJsonData->type != json_object)
    {
        log_error("emoji_kz", "Emoji file malformed: root must be an object in file '%s'", pFilename);
        return;
    }

    for(auto &value : (*pJsonData).u.object)
    {
        if(!value.value)
        {
            log_error("emoji_kz", "Emoji file malformed: value '%s' is empty", value.name);
            continue;
        }

        if(value.value->type != json_string)
        {
            log_error("emoji_kz", "Emoji file malformed: value '%s' must be a string", value.name);
            continue;
        }

        m_EmojiList[value.name] = value.value->u.string.ptr;
    }

    json_value_free(pJsonData);
}

std::string CEmojiKZ::Emojize(std::string emoticontext) //this could be better
{
    bool enough_semicolons = false; //dont keep checking if there are not enough semicolons in text
    for(size_t i = 0; i < emoticontext.size(); ++i)
    {
        if(emoticontext[i] == ':') //emoticon start
        {
            size_t start = i;
            size_t end = 0;
            for(size_t j = start + 1; j < emoticontext.size(); j++)
            {
                //if(emoticontext[j] == ' ') //no spaces allowed
                //    break;

                if(emoticontext[j] == ':') //emoticon end
                {
                    end = j;
                    enough_semicolons = true;
                    break;
                }
            }

            if(end != 0)
            {
                std::string rest = emoticontext.substr(start + 1, (end - start) - 1);

                if(m_EmojiList.find(rest) != m_EmojiList.end())
                {
                    emoticontext.replace(start, (end - start) + 1, m_EmojiList[rest]);
                }
                else
                {
                    //Start from the last ':', fixes detection issues for messages like "+KZ: :thumbsup:" where "thumbsup" would be ignored otherwise
                    i = end - 1;
                }
                continue;
            }
            else
            {
                if(!enough_semicolons)
                    return emoticontext; //not enough semicolons to check for emojis
                break; //there is no need to keep checking if there will be no more ':'
            }
        }
    }

	return emoticontext;
}
