#ifndef FALCONVIEW_SHP_STRINGUTILS_H_
#define FALCONVIEW_SHP_STRINGUTILS_H_

#include <cctype>
// great trimming from
// stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// modified for our coding style
// locale independent space search (not as smart)

// trim from start
static inline std::string& ltrim(std::string *s)
{
   s->erase(s->begin(), std::find_if(s->begin(), s->end(),
      [](char& c)
   {
         return c != ' ';
   }));
   return *s;
}

// trim from end
static inline std::string& rtrim(std::string *s)
{
   s->erase(std::find_if(s->rbegin(), s->rend(),
      [](char& c)
   {
         return c != ' ';
   }).base(), s->end());
   return *s;
}

// trim from both ends
static inline std::string& trim(std::string *s)
{
   return ltrim(&rtrim(s));
}

#endif  // FALCONVIEW_SHP_STRINGUTILS_H_
