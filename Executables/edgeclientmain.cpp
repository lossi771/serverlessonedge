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

#include "Simulation/unifclient.h"
#include "Support/chrono.h"
#include "Support/glograii.h"
#include "Support/saver.h"
#include "Support/split.h"
#include "Support/threadpool.h"
#include "Support/tostring.h"

#include <glog/logging.h>

#include <boost/program_options.hpp>

#include <cstdlib>
#include <iostream>
#include <thread>
#include <vector>

namespace po = boost::program_options;
namespace ec = uiiit::edge;
namespace es = uiiit::simulation;

int main(int argc, char* argv[]) {
  uiiit::support::GlogRaii myGlogRaii(argv[0]);

  std::string myServerEndpoints;
  std::string myClientConf;
  std::string myLambda;
  std::string mySizes;
  std::string myContent;
  std::string myOutputFile;
  size_t      myDuration;
  size_t      myMaxRequests;
  size_t      myNumThreads;
  double      myInitialDelay;
  double      myInterRequestTime;
  std::string myInterRequestType;
  size_t      mySeedUser;

  po::options_description myDesc("Allowed options");
  // clang-format off
  myDesc.add_options()
    ("help,h", "produce help message")
    ("server-endpoints",
     po::value<std::string>(&myServerEndpoints)->default_value("localhost:6473"),
     "Server end-points (comma-separated list).")
    ("client-conf",
     po::value<std::string>(&myClientConf)->default_value("type=grpc, persistence=0.05"),
     "Configuration of the edge clients.")
    ("lambda",
     po::value<std::string>(&myLambda)->default_value("clambda0"),
     "Lambda function name.")
    ("sizes",
     po::value<std::string>(&mySizes)->default_value("100"),
     "Input sizes, separated by comma. Repeated values admitted.")
    ("content",
     po::value<std::string>(&myContent)->default_value(""),
     "Use the given fixed content for all lambda requests.")
    ("output-file",
     po::value<std::string>(&myOutputFile)->default_value("/dev/stdout"),
     "Output file name. If specified suppresses normal output.")
    ("duration",
     po::value<size_t>(&myDuration)->default_value(0),
     "Experiment duration, in s. 0 means infinite.")
    ("max-requests",
     po::value<size_t>(&myMaxRequests)->default_value(std::numeric_limits<size_t>::max()),
     "Number of requests to issue.")
    ("num-threads",
     po::value<size_t>(&myNumThreads)->default_value(1),
     "Number of threads.")
    ("initial-delay",
     po::value<double>(&myInitialDelay)->default_value(0),
     "Wait for the specified interval before starting.")
    ("inter-request-time",
     po::value<double>(&myInterRequestTime)->default_value(0),
     "Wait for the specified interval before consecutive requests.")
    ("inter-request-type",
     po::value<std::string>(&myInterRequestType)->default_value("constant"),
     "One of: constant, uniform, poisson.")
    ("dry", "Do not execute the lambda requests, just ask for an estimate of the time required.")
    ("seed",
     po::value<size_t>(&mySeedUser)->default_value(0),
     "Seed generator.")
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

    if (myServerEndpoints.empty()) {
      throw std::runtime_error("Empty end-points: " + myServerEndpoints);
    }

    const auto myEdgeClientConf = uiiit::support::Conf(myClientConf);

    const auto mySizeSet =
        uiiit::support::split<std::vector<size_t>>(mySizes, ",");
    if (mySizeSet.empty()) {
      throw std::runtime_error("Invalid --sizes option");
    }

    LOG_IF(INFO, not myContent.empty())
        << "Using a custom content for all lambda requests: " << myContent;

    std::this_thread::sleep_for(
        std::chrono::nanoseconds(static_cast<int64_t>(myInitialDelay * 1e9)));

    const uiiit::support::Saver mySaver(
        myOutputFile, true, myDuration == 0, false);
    using ClientPtr = std::unique_ptr<es::UnifClient>;
    uiiit::support::ThreadPool<ClientPtr> myPool;
    std::list<es::Client*>                myClients;
    for (size_t i = 0; i < myNumThreads; i++) {
      auto myNewClient = ClientPtr(new es::UnifClient(
          mySizeSet,
          myInterRequestTime,
          es::distributionFromString(myInterRequestType),
          mySeedUser,
          i,
          myMaxRequests,
          uiiit::support::split<std::set<std::string>>(myServerEndpoints, ","),
          myEdgeClientConf,
          myLambda,
          mySaver,
          myVarMap.count("dry") > 0));
      myNewClient->setContent(myContent);
      myClients.push_back(myNewClient.get());
      myPool.add(std::move(myNewClient));
    }

    std::thread myTerminationThread;
    if (myDuration > 0) {
      myTerminationThread = std::thread([&myPool, myDuration]() {
        std::this_thread::sleep_for(std::chrono::seconds(myDuration));
        myPool.stop();
      });
    }

    myPool.start();
    const auto& myErrors = myPool.wait();

    if (myTerminationThread.joinable()) {
      myTerminationThread.join();
    }

    LOG_IF(ERROR, not myErrors.empty())
        << myErrors.size() << " error" << (myErrors.size() > 1 ? "s" : "")
        << " occurred: " << ::toString(myErrors, " | ");

    for (auto myClient : myClients) {
      assert(myClient != nullptr);
      LOG_IF(INFO, myClient->latencyStat().count() >= 1)
          << "latency " << myClient->latencyStat().mean() << " +- "
          << myClient->latencyStat().stddev();
      LOG_IF(INFO, myClient->processingStat().count() >= 1)
          << "processing " << myClient->processingStat().mean() << " +- "
          << myClient->processingStat().stddev();
    }

    return EXIT_SUCCESS;
  } catch (const std::exception& aErr) {
    LOG(ERROR) << "Exception caught: " << aErr.what();
  } catch (...) {
    LOG(ERROR) << "Unknown exception caught";
  }

  return EXIT_FAILURE;
}
