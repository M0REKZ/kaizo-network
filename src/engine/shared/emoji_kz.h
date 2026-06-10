// Copyright (C) Benjamín Gajardo (also known as +KZ)

#ifndef ENGINE_CLIENT_EMOJI_KZ_H
#define ENGINE_CLIENT_EMOJI_KZ_H

#include <unordered_map>

class IKernel;

class CEmojiKZ
{
public:
    void Init(IKernel *pKernel);
    std::string Emojize(std::string emoticontext);

private:
    std::unordered_map<std::string, std::string> m_EmojiList;
};

#endif
