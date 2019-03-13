# Datapoint for C++

A C++ Library for accessing weather data via the Met Office's open data API known as DataPoint.
Depends on cURL and rapidJSON.

Designed to make few calls to the Datapoint API as possible, in order to keep within the fair use
policy and improve the general speed of the library. Thus some functions like 'val/wxfcs/all/json/capabilities'
are avoided.

You might also like to checkout "DataPoint for Python" - https://github.com/jacobtomlinson/datapoint-python
 
### Disclaimer
This module is in no way part of the DataPoint project/service.
No support for this module is provided by the Met Office and may break as the DataPoint service grows/evolves.

### Prerequisites

Datapoint for C++ example and wrapper code requires [libcurl ](https://curl.haxx.se/libcurl/) and [rapidJSON](https://github.com/Tencent/rapidjson) to compile.
  
 ## Authors

**Alan Percy Childs** - *Initial work* 

## Licence

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
