#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <atomic>
#include <mutex>

#include <omh.hpp>
#include <glob.hpp>
#include <omh_compare_sketches.hpp>

// Dirty trick to print the content of a ostringstream
std::ostream& operator<<(std::ostream& os, std::ostringstream& ss) {
  struct dumpbuf : std::streambuf {
    inline void dump(std::ostream& os) {
      os.write(pbase(), pptr() - pbase());
    }
  };

 if(std::ostringstream::sentry(ss)) {
    auto b = ss.rdbuf();
    ((dumpbuf*)b)->dump(os);
    b->pubseekpos(0);
  }
  return os;
}

void read_sketches(const char* pattern, std::vector<sketch>& sketches) {
  glob_files sketch_files(pattern);
  for(const auto file : sketch_files) {
    std::ifstream is(file);
    if(!is.good())
      std::cerr << "Failed to open file: '" << file << "'\n";
    int c = is.peek();
    for( ; c == '>'; c = is.peek())
      sketches.push_back(sketch::from_stream(is));
    if(c != EOF)
      std::cerr << "Invalid sketch file '" << file << "'\n";
  }
}

void comparison_th(const std::vector<sketch>& sketches1, const std::vector<sketch>& sketches2, const ssize_t m, const bool circular,
                   std::atomic<size_t>& left_sketch, std::mutex& output_mtx) {
  static constexpr size_t buffsize = 4096;
  std::ostringstream buff; // Output buffer
  while(true) {
    // Atomically get a sketch from sketches1. If done, break out of loop
    const size_t sketch_index = left_sketch++;
    if(sketch_index >= sketches1.size()) break;
    const auto& s1 = sketches1[sketch_index];

    for(const auto& s2 : sketches2) {
      buff << s1.name << ' ' << s2.name << ' ' << compare_sketches(s1, s2, m, circular) << '\n';
      if(buff.tellp() > buffsize) {
        std::lock_guard<std::mutex> lock(output_mtx);
        std::cout << buff;
      }
    }
  }
  if(buff.tellp() > 0) {
    std::lock_guard<std::mutex> lock(output_mtx);
    std::cout << buff;
  }
}

int main(int argc, char *argv[]) {
  omh_compare_sketches args(argc, argv);

  std::vector<sketch> sketches1, sketches2;
  if(args.threads_arg > 1) {
    std::vector<std::thread> threads;
    threads.push_back(std::thread(read_sketches, args.sk1_arg, std::ref(sketches1)));
    threads.push_back(std::thread(read_sketches, args.sk2_arg, std::ref(sketches2)));
    for(auto& th : threads) th.join();
  } else {
    read_sketches(args.sk1_arg, sketches1);
    read_sketches(args.sk2_arg, sketches2);
  }

  const ssize_t m = args.m_given ? args.m_arg : -1;
  if(args.threads_arg != 1) {
    const unsigned nb_threads = args.threads_arg == 0 ? std::thread::hardware_concurrency() : args.threads_arg;
    std::atomic<size_t> left_sketch(0);
    std::mutex          output_mtx;
    std::vector<std::thread> threads;
    for(unsigned i = 0; i < nb_threads; ++i)
      threads.push_back(std::thread(comparison_th, sketches1, sketches2, m, args.circular_flag, std::ref(left_sketch), std::ref(output_mtx)));
    for(auto& th : threads) th.join();
  } else {
    for(const auto& s1 : sketches1) {
      for(const auto& s2 : sketches2)
        std::cout << s1.name << ' ' << s2.name << ' ' << compare_sketches(s1, s2, m, args.circular_flag) << '\n';
    }
  }

  return 0;
}
