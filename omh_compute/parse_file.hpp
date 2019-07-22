#ifndef __PARSE_FILE_H__
#define __PARSE_FILE_H__

#include <istream>
#include <string>

bool parse_entry(std::istream& is, std::string& name, std::string& seq, bool upper = true);

#endif /* __PARSE_FILE_H__ */
