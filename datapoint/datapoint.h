/*
 * DataPoint for C++
 *
 * A C++ Library for accessing weather data via the Met Office's open data API known as DataPoint.
 *
 */


#ifndef DATAPOINT_DATAPOINT_H_H_
#define DATAPOINT_DATAPOINT_H_T_H_

#include <iostream>
#include <vector>

struct curlbuffer
{
    std::fstream *file;
    std::string *str;
};

#include <rapidjson/document.h>
namespace datapoint {
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
		} ;//forecast
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
		} ;//observation
		class api {
			private:
				const char* url_forecast = "http://datapoint.metoffice.gov.uk/public/data/val/wxfcs/all/json" ;
				const char* url_obs =      "http://datapoint.metoffice.gov.uk/public/data/val/wxobs/all/json" ;
				std::string key;
				double lat;
				double lon;
				long double Distance(long double lat1, long double long1, long double lat2, long double long2 );
				rapidjson::Value::ConstValueIterator FindNearest(rapidjson::Document *json, double *our_lat, double *our_lon);
				datapoint::forecast dumpForecast(rapidjson::Value::ConstValueIterator itr);
			public:
				unsigned int forecast_id = 0;
				std::string forecast_name;
				std::string forecast_area;
				unsigned short int observation_id = 0;
				std::string observation_name;
				std::string observation_area;
				rapidjson::Document Call(std::string func, bool forecast) ;
				std::string Setkey(std::string inkey); std::string Setkey();
				bool Setlocation(double our_lat, double our_lon) ;
				datapoint::forecast * GetForecast() ;
				datapoint::observation GetObservation() ;
				std::string weather_codes[32] = { //Use designators for readability...
					[0] = "Clear night",
					[1] = "Sunny day",
					[2] = "Partly cloudy (night)",
					[3] = "Partly cloudy (day)",
					[4] = "Not used",
					[5] = "Mist",
					[6] = "Fog",
					[7] = "Cloudy",
					[8] = "Overcast",
					[9] = "Light rain shower (night)",
					[10] = "Light rain shower (day)",
					[11] = "Drizzle",
					[12] = "Light rain",
					[13] = "Heavy rain shower (night)",
					[14] = "Heavy rain shower (day)",
					[15] = "Heavy rain",
					[16] = "Sleet shower (night)",
					[17] = "Sleet shower (day)",
					[18] = "Sleet",
					[19] = "Hail shower (night)",
					[20] = "Hail shower (day)",
					[21] = "Hail",
					[22] = "Light snow shower (night)",
					[23] = "Light snow shower (day)",
					[24] = "Light snow",
					[25] = "Heavy snow shower (night)",
					[26] = "Heavy snow shower (day)",
					[27] = "Heavy snow",
					[28] = "Thunder shower (night)",
					[29] = "Thunder shower (day)",
					[30] = "Thunder",
					[31] = "Unknown" //Not an official Datapoint response!
				}; //weather


		~api() {}
		} ;//api
} //datapoint

#endif /* DATAPOINT_DATAPOINT_H_INT_H_ */
