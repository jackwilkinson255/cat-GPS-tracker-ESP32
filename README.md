# Cat GPS Tracker

## Architecture
An object-oriented approach to the software architecture has been taken. Classes have been defined on a component/funcational basis, e.g. there is a class for handling AWS, GPS and WiFi.

### AWS IoT Core

## Components

## Test-Driven Development
The unity framework was used for testing C/C++ code. The tests have been broken down into three .cpp files, to test each of the main subsystems: [test_aws.cpp](/test/test_aws/test_aws.cpp), [test_wifi.cpp](/test/test_wifi/test_wifi.cpp) and [test_gps.cpp](/test/test_gps/test_gps.cpp)

### Unit Testing

#### GPS
The GPS is a fundamental part of the overall system, hence thorough unit tests have been implemented.

### Mocking