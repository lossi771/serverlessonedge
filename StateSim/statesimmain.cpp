/*
              __ __ __
             |__|__|  | __
             |  |  |  ||__|
  ___ ___ __ |  |  |  |
 |   |   |  ||  |  |  |    Ubiquitous Internet @ IIT-CNR
 |   |   |  ||  |  |  |    C++ edge computing libraries and tools
 |_______|__||__|__|__|    https://github.com/ccicconetti/serverlessonedge

Licensed under the MIT License <http://opensource.org/licenses/MIT>
Copyright (c) 2021 C. Cicconetti <https://ccicconetti.github.io/>

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "StateSim/simulation.h"
#include "Support/chrono.h"
#include "Support/glograii.h"

#include <boost/program_options.hpp>

#include <glog/logging.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

namespace po = boost::program_options;
namespace ss = uiiit::statesim;

int main(int argc, char* argv[]) {
  uiiit::support::GlogRaii myGlogRaii(argv[0]);

  std::string myNodesFile;
  std::string myLinksFile;
  std::string myEdgesFile;
  std::string myTasksFile;
  std::string myOutdir;
  size_t      myNumFunctions;
  size_t      myNumJobs;
  size_t      myStartingSeed;
  size_t      myNumReplications;
  double      myOpsFactor;
  double      myArgFactor;
  double      myStateFactor;
  size_t      myNumThreads;

  po::options_description myDesc("Allowed options");
  // clang-format off
  myDesc.add_options()
    ("help,h", "produce help message")
    ("nodes-file",
     po::value<std::string>(&myNodesFile)->default_value("nodes"),
     "File containing the specifications of nodes.")
    ("links-file",
     po::value<std::string>(&myLinksFile)->default_value("links"),
     "File containing the specifications of links.")
    ("edges-file",
     po::value<std::string>(&myEdgesFile)->default_value("edges"),
     "File containing the specifications of edges.")
    ("tasks-file",
     po::value<std::string>(&myTasksFile)->default_value("tasks"),
     "File containing the specifications of tasks.")
    ("outdir",
     po::value<std::string>(&myOutdir)->default_value("data"),
     "The directory where to save the results.")
    ("num-functions",
     po::value<size_t>(&myNumFunctions)->default_value(5),
     "The number of lambda functions.")
    ("num-jobs",
     po::value<size_t>(&myNumJobs)->default_value(10),
     "The number of jobs per replication.")
    ("seed-starting",
     po::value<size_t>(&myStartingSeed)->default_value(1),
     "The starting seed.")
    ("num-replications",
     po::value<size_t>(&myNumReplications)->default_value(1),
     "The number of replications.")
    ("ops-factor",
     po::value<double>(&myOpsFactor)->default_value(1000),
     "The multiplier of the number of operations of loaded tasks.")
    ("arg-factor",
     po::value<double>(&myArgFactor)->default_value(1000),
     "The multiplier of the argument size of loaded tasks.")
    ("state-factor",
     po::value<double>(&myStateFactor)->default_value(1000),
     "The multiplier of the state size of loaded tasks.")
    ("num-threads",
     po::value<size_t>(&myNumThreads)->default_value(
       std::max(1u,
                std::thread::hardware_concurrency())),
     "The number of threads to spawn.")
    ;
  // clang-format on

  try {
    po::variables_map myVarMap;
    po::store(po::parse_command_line(argc, argv, myDesc), myVarMap);
    po::notify(myVarMap);

    if (myVarMap.count("help")) {
      std::cout << myDesc << std::endl;
      return EXIT_FAILURE;
    }

    uiiit::support::Chrono myChrono(true);
    ss::Simulation(myNumThreads)
        .run({myNodesFile,
              myLinksFile,
              myEdgesFile,
              myTasksFile,
              std::string(),
              myOutdir,
              myNumFunctions,
              myNumJobs,
              myOpsFactor,
              myArgFactor,
              myStateFactor},
             myStartingSeed,
             myNumReplications);
    LOG(INFO) << "simulation lasted " << myChrono.stop() << " s";

    return EXIT_SUCCESS;

  } catch (const std::exception& aErr) {
    std::cerr << "Exception caught: " << aErr.what() << std::endl;

  } catch (...) {
    std::cerr << "Unknown exception caught" << std::endl;
  }

  return EXIT_FAILURE;
}
