# Install

This documents gives a layout of the required components needed in order to run the services. For installation see description below. For development, build and extending the framework see the [development](DEVELOPMENT.md) draft.

## Runtime requirements

The services require runtime dependencies in order to run. Since services are usually separated over different machines they can be installed where required. Although the list below is per service it is recommended to at least have __python 2.7__ or later installed. Most dependencies can be installed though the operatingssystems package manager.

#### Ecoli (optional)
 - python 2.7

#### Cynder
 - zmq3
 - boost-system
 - boost-thread
 - protobuf9

#### Extractor
 - libmagic
 - zmq3
 - protobuf9

#### Orchestrator

#### Pitcher
 - leveldb
 - protobuf9
 - zmq3

#### Scraper
 - protobuf9
 - zmq3
 - python 2.7
 
## 3rd party

Services depend on third party sources of which some are included in the source tree.
Many of the repositories can be found on Github.

License
----

BSD 3-clause
