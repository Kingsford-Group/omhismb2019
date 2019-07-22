#include <parse_file.hpp>
#include <limits>
#include <stdexcept>
#include <cctype>


// Parse one sequence of fasta. Assume (and don't check) that is currently points to '>'
void parse_fasta_entry(std::istream& is, std::string& name, std::string& seq) {
  is.get();
  is >> name;
  is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  std::string line;
  seq.clear();
  for(int c = is.peek(); c != '>' && c != EOF; c = is.peek()) {
    std::getline(is, line);
    seq += line;
  }
}

// Parse one sequence of fastq. Assume (and don't check) that is currently points to '@'
void parse_fastq_entry(std::istream& is, std::string& name, std::string& seq) {
  is.get();
  is >> name;
  is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  std::getline(is, seq);
  is.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Quality header
  is.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Quality scores
}

bool parse_entry(std::istream& is, std::string& name, std::string& seq, bool upper) {
  switch(is.peek()) {
  case '>': parse_fasta_entry(is, name, seq); break;
  case '@': parse_fastq_entry(is, name, seq); break;
  case EOF: return false;
  default:
    throw std::runtime_error("Unknown header");
  }

  if(upper) {
    for(auto it = seq.begin(); it != seq.end(); ++it)
      *it = toupper(*it);
  }
  return true;
}
