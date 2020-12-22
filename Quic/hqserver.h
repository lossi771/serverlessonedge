/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/   Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /    C++ edge computing libraries and tools
|   |   |  |/__/  /   /  /  https://bitbucket.org/ccicconetti/edge_computing/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2018 Claudio Cicconetti <https://about.me/ccicconetti>

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

#pragma once

#include "Quic/quicparams.h"

#include <quic/server/QuicServer.h>

namespace {
const std::string kDefaultCertData = R"(
-----BEGIN CERTIFICATE-----
MIIGGzCCBAOgAwIBAgIJAPowD79hiDyZMA0GCSqGSIb3DQEBCwUAMIGjMQswCQYD
VQQGEwJVUzETMBEGA1UECAwKQ2FsaWZvcm5pYTETMBEGA1UEBwwKTWVubG8gUGFy
azERMA8GA1UECgwIUHJveHlnZW4xETAPBgNVBAsMCFByb3h5Z2VuMREwDwYDVQQD
DAhQcm94eWdlbjExMC8GCSqGSIb3DQEJARYiZmFjZWJvb2stcHJveHlnZW5AZ29v
Z2xlZ3JvdXBzLmNvbTAeFw0xOTA1MDgwNjU5MDBaFw0yOTA1MDUwNjU5MDBaMIGj
MQswCQYDVQQGEwJVUzETMBEGA1UECAwKQ2FsaWZvcm5pYTETMBEGA1UEBwwKTWVu
bG8gUGFyazERMA8GA1UECgwIUHJveHlnZW4xETAPBgNVBAsMCFByb3h5Z2VuMREw
DwYDVQQDDAhQcm94eWdlbjExMC8GCSqGSIb3DQEJARYiZmFjZWJvb2stcHJveHln
ZW5AZ29vZ2xlZ3JvdXBzLmNvbTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoC
ggIBALXZs4+YnCE8aMAL5gWNjLRm2EZiFHWoKpt42on8y+SZdb1xdSZ0rx6/jl4w
8V5aiLLNmboa1ULNWLS40mEUoqRPEUiBiN3T/3HomzMCLZ52xaaKS1sW9+ZPsSlT
omwV4HupJWKQaxpu+inY98mxGaZjzHie3AoydovD+rWWLj4mSX9DchWbC8DYq7xu
4qKedgHMJlsP3luYgnRSsZ+vlTEe/K41Czt+GGhViRNL8Nm3wZrxAGYqTx/zrqsT
R8qA3gwfPPqJJH5UprtvHXDS99yiy6MYyWBr/BbZ37A5X9pWCL09aLEIrQGQWtVu
CnBNCrQgYDgD7Y4+Q4Lfouap7I3YpuJM5cP1NO1x0Voyv2km1tmZpjUavnKyYT/v
XUCkGrWxeuMkqm68eOnadA7A8BM9b++f6NIgaexb9+Rq8QK74MpMm7/+XMWiAS9z
62hgKBd4mtUulJH1YxoQBIkfRa8pkB45nGiTrL2zzpIOoOirNe3/7FVI9LqPphPN
64ojfqZsTiGrC50R/86/p2jBs0fwrXy8opWM7Kmp1h2oNPqtgOC0Zj7IcmvEp2xa
wI6jN4XxbhDQpo3Iz/KRDxXFT4kAjdLDibWH41PccwSbHvg8zjmAGCxW6sC6bmp6
lywMzonS1VWkp1iNQ2u4bdMeDGnsaN0hOBemBLr/p3L1ee/RAgMBAAGjUDBOMB0G
A1UdDgQWBBSHFEM/GlCxZgg9qpi9REqm/RDkZDAfBgNVHSMEGDAWgBSHFEM/GlCx
Zgg9qpi9REqm/RDkZDAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBCwUAA4ICAQBG
AtowRS0Wsr9cVRKVxEM/7ZxCDcrTg7gUBD/S8RYnS2bJp5ut/3SgO0FZsQKG4k8O
CXE/dQgwIaBqxSioE3L/l+m/+gedZgqaXg7l6EJLr20sUB5PVrJoQznMIwr/FuYZ
LG4nKK/K7eKf2m1Gn54kpeWz+BtgIRU4YPkZHGtQW3ER+wnmlPQfGDiN0JymqR80
TTXlgg03L6jCFQpYGKCLbKpql+cBixmI6TeUtArosCsqZokUXNM7j5u7m1IhY1EL
pNpSaUMU7LmHOmfnxIHzmNzages+mxKOHJLKBbuQx0u87uGy3HInwbNK7hDHXWLF
mXPXDhrWjBbm1RPnq8cX9nFuPS6Cd+hROEr+VB7m+Sij5QyV5pRBS0x/54tiiEv3
8eIFl6aYqTBcCMrtlxVn8sHcA/iGrysIuidWVxQfs4wmM/apR5YgSjTvN/OAB5Mo
/5RWdxBg3jNPGk/GzPDk6FcN5kp7yRLLyAOAnPDUQRC8CkSkyOwriOMe310CnTL4
KCWp7UpoF/qZJEGhYffH85SORpxj09284tZUnLSthnRmIdYB2kWg9AARu3Vhugx8
E9HGSZzTGAsPEBikDbpUimN0zWLw8VJKL+KJURl4dX4tDRe+R2u5cWm8x3HOcDUI
j9aXkPagbL/an2g05K0hIhyANbER7HAZlJ21pJdCIQ==
-----END CERTIFICATE-----
)";

// The private key below is only used for test purposes
// @lint-ignore-every PRIVATEKEY
const std::string kDefaultKeyData = R"(
-----BEGIN RSA PRIVATE KEY-----
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
-----END RSA PRIVATE KEY-----
)";

const std::string kPrime256v1CertData = R"(
-----BEGIN CERTIFICATE-----
MIICkDCCAjWgAwIBAgIJAOILJQbZxXtaMAoGCCqGSM49BAMCMIGjMQswCQYDVQQG
EwJVUzETMBEGA1UECAwKQ2FsaWZvcm5pYTETMBEGA1UEBwwKTWVubG8gUGFyazER
MA8GA1UECgwIUHJveHlnZW4xETAPBgNVBAsMCFByb3h5Z2VuMREwDwYDVQQDDAhQ
cm94eWdlbjExMC8GCSqGSIb3DQEJARYiZmFjZWJvb2stcHJveHlnZW5AZ29vZ2xl
Z3JvdXBzLmNvbTAeFw0yMDA0MDcyMDMyMDRaFw0zMDA0MDUyMDMyMDRaMIGjMQsw
CQYDVQQGEwJVUzETMBEGA1UECAwKQ2FsaWZvcm5pYTETMBEGA1UEBwwKTWVubG8g
UGFyazERMA8GA1UECgwIUHJveHlnZW4xETAPBgNVBAsMCFByb3h5Z2VuMREwDwYD
VQQDDAhQcm94eWdlbjExMC8GCSqGSIb3DQEJARYiZmFjZWJvb2stcHJveHlnZW5A
Z29vZ2xlZ3JvdXBzLmNvbTBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABB5MtBjA
TaKYREMWTIbzK6utt7Jjb3xWcWowKeN14WFz8sDqvHcAufaN8OP2NBHRAZGi4UDs
1thkXHtSPcc7DT+jUDBOMB0GA1UdDgQWBBSWhUXpZWkCj6YywA8iZIvl52GvzDAf
BgNVHSMEGDAWgBSWhUXpZWkCj6YywA8iZIvl52GvzDAMBgNVHRMEBTADAQH/MAoG
CCqGSM49BAMCA0kAMEYCIQCduzLSWUJ2RgxYvNiApmmH9Yml/s7T2bB2r6+1wlPw
OgIhAPfLxzClQvbpPvchgQkWEJTsMgmI/CgNWX02SIzeg934
-----END CERTIFICATE-----
)";

const std::string kPrime256v1KeyData = R"(
-----BEGIN PRIVATE KEY-----
MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQg/NeWSkmQEmaO2f0T
5ogGmfvwGId3k5i8o8hJOoV9pOuhRANCAAQeTLQYwE2imERDFkyG8yurrbeyY298
VnFqMCnjdeFhc/LA6rx3ALn2jfDj9jQR0QGRouFA7NbYZFx7Uj3HOw0/
-----END PRIVATE KEY-----
)";
} // namespace

namespace uiiit {
namespace edge {

using HTTPTransactionHandlerProvider =
    std::function<proxygen::HTTPTransactionHandler*(proxygen::HTTPMessage*,
                                                    const HQParams&)>;

using FizzServerContextPtr =
    std::shared_ptr<const fizz::server::FizzServerContext>;

class HQServer
{
 public:
  explicit HQServer(
      const HQParams&                params,
      HTTPTransactionHandlerProvider httpTransactionHandlerProvider);

  // Starts the QUIC transport in background thread
  std::thread start();

 private:
  const HQParams&                   params_;
  folly::EventBase                  eventbase_;
  std::shared_ptr<quic::QuicServer> server_;
  folly::Baton<>                    cv_;
};

FizzServerContextPtr createFizzServerContext(const HQParams& params);

} // namespace edge
} // namespace uiiit