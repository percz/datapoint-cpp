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

## Prerequisites

Datapoint for C++ example program and library code requires [libcurl ](https://curl.haxx.se/libcurl/) and [rapidJSON](https://github.com/Tencent/rapidjson) to compile.

## Example Usage (lib files)

```cpp

#include "datapoint/datapoint.h"

//Create API object
	datapoint::api weather;
 
//Set DataPoint API key as string.
 string key = "01234567-89ab-cdef-0123-456789abcdef"; //Get from https://www.metoffice.gov.uk/datapoint/api
	weather.Setkey(key);

//Set DataPoint latitude and longitude co-ords as floats.
 float lat = 52.634001, lon = 1.293240;
 weather.Setlocation(lat, lon);

// Create object of current observation
datapoint::observation observation = weather.GetObservation();
// Print current weather; convert code to text using array
cout << "Current Weather : " << weather.weather_codes[observation.weatherType] << endl;

// Create object (array) of current forecasts in 3 hour resolutions
datapoint::forecast *forecast = weather.GetForecast();
// Print out the site and next weather forecast
cout << "Future Weather : " << weather.weather_codes[forecast[0].weatherType] << endl;
```
###Current class
```cpp
  #define MAXHOURS 28 //Furthest away forecast - forecast[27].weatherType
		class forecast {
			public:
				std::string feelsLike; //Celsius
				std::string windGust; //Miles Per GHour
				std::string humidity; //Percent
				std::string temperature; //Celsius
				std::string visibility; //Descriptive: G/VG
				std::string windDirection; //Compass: N/E/S/W
				std::string windSpeed; //Miles Per Hour
				std::string uv; //
				unsigned short int weatherType; //Ref weather_codes
				std::string precipitationProb; //Percent
				std::string minsofday;
		} ;
		class observation {
			public:
				std::string windGust; //Miles Per Hour
				std::string temperature; //Celsius
				std::string visibility; //Meters
				std::string windDirection; //Compass: N/E/S/W
				std::string windSpeed; //Miles Per Hour
				unsigned short int weatherType; //Ref weather_codes
				std::string pressure; //Hectopascal (hpa)
				std::string tendency; //Pascals per Second  (Pa/s)
				std::string dewPoint; //Celsius
				std::string humidity; //Percent
		} ;
```

 ## Authors

**Alan Percy Childs** - *Initial work* 

## Licence

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
