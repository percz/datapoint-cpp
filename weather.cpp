/*
 * DataPoint for C++ (example program)
 *
 * An example C++ program for accessing weather data via the Met Office's open data API known as DataPoint.
 * Along with the datapoint.cpp and datapoint.h library files, this depends on cURL and rapidJSON.
 * Tested on a Raspberry Pi with Debian. Should work on everything that has the dependencies.
 *
 * Disclaimer: This module is in no way part of the DataPoint project/service.
 * No support for this module is provided by the Met Office and may break as the DataPoint service grows/evolves.
 *
 * MIT License
 * Copyright (c) 2019 Alan Percy Childs
 *
 *
 */
#define LAYOUT_SPACER "  |  "

#include <iostream>
#include "datapoint/datapoint.h"

using namespace std;

int main(int argc, char *argv[]) {
	/*
	 * For this example program, unsafely assuming the user inputs correctly formatted arguments.
	 * Expect crashes if you get it wrong.
	 */
    bool needforecast = false, needobservation = false, needhelp = false;
    string key = "", lat = "", lon = "";
	   for(int i = 1; i < argc; i++) {
	      if (string(argv[i]) == "--help") {
	    	  needhelp = true;
	    	  break;
	      } else if ( string(argv[i]) == "--forecast" ) {
	    	  needforecast = true;
	    	  continue;
	      } else if ( string(argv[i]) == "--observation" ) {
	    	  needobservation = true;
	    	  continue;
	      }  else if ( string(argv[i]) == "--setkey" ) {
	    	  i++; key = argv[i];
	    	  continue;
	      }  else if ( string(argv[i]) == "--lat" ) {
	    	  i++; lat = argv[i];
	    	  continue;
	      }  else if ( string(argv[i]) == "--lon" ) {
	    	  i++; lon = argv[i];
	    	  continue;
	      } else {
	    	  cout << "Invalid Option \"" << string(argv[i]) << "\"\n";
	    	  needhelp = true;
	    	  break;
	      }
	   }

   if (needhelp) {
	   cout << "\nUsage: " << string(argv[0]) << " [--key]... [--location]... [--observation &| --forecast]\n"
		       "\nValid command-line options : \n"
			   "\t--setkey\n"
			   "\t\tRequied: Datapoint API Key\n"
			   "\t--lat\n"
			   "\t\tRequired: You latitude location, in decimal format. (--lat 52.634001) \n"
			   "\t--lon\n"
			   "\t\tRequired: You longitude location, in decimal format. (--lon 1.293240) \n"
			   "\t--observation\n"
			   "\t\tGet and print latest observation.\n"
			   "\t--forecast\n"
			   "\t\tGet and print latest forecasts.\n"
	           "\t--help\n"
	   	   	   "\t\tPrint this help message.\n"
			   "\n\n" ;
	   exit(1);
	} else if (key == "" || lat == "" || lon == "") {
		cout << "\nYou must specify all - Datapoint API Key, your latitude, and your longitude.\nSee --help for full usage.\n\n";
		exit(1);
	} else if ((!needobservation) && (!needforecast)) {
		cout << "\nYou must request either --observation, --forecast, or both.\n\nSee --help for full usage.\n\n";
		exit(1);
	}

	datapoint::api weather;
	weather.Setkey(key);
	weather.Setlocation(stof(lat),stof(lon));

	if (needobservation) {
		datapoint::observation observation = weather.GetObservation();
		cout << "*** Current Observation **" << endl;
		cout << "Site Name: " << weather.observation_name; (weather.observation_area == "") ? cout << endl : cout << ", " << weather.observation_area  << endl;
		cout << "Weather Type: " << weather.weather_codes[observation.weatherType] << LAYOUT_SPACER; //Array magic
		cout << "Temperature: " << observation.temperature << "c" << endl;
		cout << "Wind Speed: " << observation.windSpeed << "mph" << LAYOUT_SPACER;
		cout << "Wind Direction: " << observation.windDirection << LAYOUT_SPACER;
		cout << "Wind Gust: " << observation.windGust << "mph" << endl;
		cout << "Visibility: " << observation.visibility << " meters" << LAYOUT_SPACER;
		cout << "Pressure: " << observation.pressure << "P" << LAYOUT_SPACER;
		cout << "Pressure Tendency: " << observation.tendency << "Pt" << endl;
		cout << "Dew Point: " << observation.dewPoint << "C" <<  LAYOUT_SPACER;
		cout << "Humidity: " << observation.humidity << "%" << endl;
		cout << "" << endl;
	}

	if (needforecast) {
		datapoint::forecast *forecast = weather.GetForecast();
		cout << "*** 3 Hourly Weather Forecast ***" << endl;
		cout << "Site Name: " << weather.forecast_name; (weather.forecast_area == "") ? cout << endl : cout << ", " << weather.forecast_area  << endl;
		float hours_req = 24; //Get the next 24 hours weather
		unsigned short int reqs = (hours_req / 3) + 0.5; //The GetForecast gets 28 even if we don't use it all here. Also this is a cheap ceil.
		for ( unsigned short int i = 0 ; i < reqs ; i++  ) {
			unsigned short int time = (stoi(forecast[i].minsofday) / 60);
			if (i > 0 && time == 00) {cout << "Tomorrow " << endl ;}
			cout << "Time: " << time << ":00" << endl;
			cout << "Weather Type: " << weather.weather_codes[forecast[i].weatherType] << LAYOUT_SPACER;
			cout << "Precipitation Probability: " << forecast[i].precipitationProb << "%" << endl;
			cout << "Temperature: " << forecast[i].temperature << "c" << LAYOUT_SPACER;
			cout << "Feels Like: " << forecast[i].feelsLike << "c" << endl;
			cout << "Wind Speed: " << forecast[i].windSpeed << "mph" << LAYOUT_SPACER;
			cout << "Wind Direction: " << forecast[i].windDirection << LAYOUT_SPACER;
			cout << "Wind Gust: " << forecast[i].windGust << "mph" << endl;
			cout << "Humidity: " << forecast[i].humidity << "%" << LAYOUT_SPACER;
			cout << "Visibility: " << forecast[i].visibility << LAYOUT_SPACER;
			cout << "UV Level: " << forecast[i].uv << endl;
			cout << "" << endl;
		}
		cout << "" << endl;
	}

	return 0;

}
