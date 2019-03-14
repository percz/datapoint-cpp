/*
 * DataPoint for C++
 *
 * A C++ Library for accessing weather data via the Met Office's open data API known as DataPoint.
 * Depends on cURL and rapidJSON. Tested on a Raspberry Pi, should work on everything that has dependencies.
 *
 * A primary aim is to make as few calls to the Datapoint API as possible, in order to keep within the fair use
 * policy and improve the general speed of the library. Thus some functions like 'val/wxfcs/all/json/capabilities'
 * are flatly avoided. As secondary aim is to use as little system memory as possible.
 *
 * You might also like to checkout "DataPoint for Python" - https://github.com/jacobtomlinson/datapoint-python
 *
 * Disclaimer: This module is in no way part of the DataPoint project/service.
 * No support for this module is provided by the Met Office and may break as the DataPoint service grows/evolves.
 *
 * MIT License
 * Copyright (c) 2019 Alan Percy Childs
 *
 *
 */

#define FORECAST true
#define OBSERVATION false

#include "../datapoint/datapoint.h"

#include <iostream>

#include <curl/curl.h>
#include <rapidjson/reader.h>
#include <rapidjson/document.h>

using namespace std;

#include <fstream>
size_t write_curlbuffer(void *ptr, size_t size, size_t nmemb, curlbuffer *data) {
    size_t numBytes = size * nmemb;
    if (data->file)
        data->file->write((char*)ptr, numBytes);
    if (data->str)
        *(data->str) += std::string((char*)ptr, numBytes);
    return numBytes;
}

rapidjson::Document datapoint::api::Call(std::string func, bool isForecast) {

    if(key == "" || func == "") {
    	//TODO: Invalid key or func request, deal with it safely. Maybe return an error in JSON format or something.
    	cout << "Couldn't create valid payload for Datapoint. Crashed instead of wasting a request." << endl ;
    	exit(1);
    }
	
    CURL *connection;
    curlbuffer curlbuffer;
    fstream curlfile("content.json", ios_base::out | ios_base::ate);
    string curlstream;
    curlbuffer.file = &curlfile;
    curlbuffer.str = &curlstream;

    connection = curl_easy_init();
    std::string url;
    (isForecast) ? url = url_forecast : url = url_obs ; //Allow both forecast and observation calls
    ( std::strstr(func.c_str(),"?") ) ? func += "&" : func += "?" ; //Append GET as needed


    std::string payload = url + "/" + func + "key=" + key ;

    //cout << payload << endl; //XXX: Uncomment to see what the URL used is when debugging.

    curl_easy_setopt(connection, CURLOPT_URL, payload.c_str());
	curl_easy_setopt(connection, CURLOPT_TIMEOUT, 10); // Don't wait forever, time out after 10 seconds
	curl_easy_setopt(connection, CURLOPT_FOLLOWLOCATION, 1L); // Follow HTTP redirects if necessary
	curl_easy_setopt(connection, CURLOPT_WRITEFUNCTION, write_curlbuffer);
	curl_easy_setopt(connection, CURLOPT_WRITEDATA, &curlbuffer);

	// Run our HTTP GET command, capture the HTTP response code, and clean up.
	CURLcode errorCode =  curl_easy_perform(connection);
	curl_easy_cleanup(connection);
    if (errorCode == CURLE_OK) {

    	rapidjson::Document json;
    	json.Parse(curlstream.c_str());

    	return json;
    } else {
    	//TODO: HTTP Failed, deal with it safely. Maybe return an error in JSON format or something.
    	cout << "Error: " << errorCode << endl ;
    	cout << "DataPoint has not returned any data, this could be due to an incorrect API key" << endl ;
    	exit(1);
    }
    curl_global_cleanup();

};

/*
 * Set private stuff if someone adds error handling...
 */
string datapoint::api::Setkey(string inkey) {
	key = inkey;
	return key;
}
string datapoint::api::Setkey() { return key; }

/*
 * To get the location, supply lat and lon. The code then finds the nearest observation point and sets it privately.
 */
#include <bits/stdc++.h> //MATH!

long double datapoint::api::Distance(long double lat1, long double long1, long double lat2, long double long2) {
	//TODO: Not using pointers, as we convert double's into long doubles. Can probs tidy this up without losing much?
	long double toRadians = (M_PI) / 180 ;
    // Haversine Formula
    long double dlong = (toRadians*long2) - (toRadians*long1);
    long double dlat = (toRadians*lat2) - (toRadians*lat1);

    long double ans = pow(sin(dlat / 2), 2) +
                          cos(lat1) * cos(lat2) *
                          pow(sin(dlong / 2), 2);
    ans = 2 * asin(sqrt(ans));
    long double R = 6371; // Radius of Earth in Kilometres
    ans = ans * R;

    return ans;
}

rapidjson::Value::ConstValueIterator datapoint::api::FindNearest(rapidjson::Document *json, double *our_lat, double *our_lon) {
	long double dist = 0;
	rapidjson::Document& loc = *json;
	const rapidjson::Value& locations = loc["Locations"]["Location"];
	assert(locations.IsArray());
	rapidjson::Value::ConstValueIterator nearest = locations.Begin();
	for (rapidjson::Value::ConstValueIterator  itr = nearest; itr != locations.End(); ++itr) {
	    float json_lat = std::stof( (*itr)["latitude"].GetString() );
	    float json_lon = std::stof( (*itr)["longitude"].GetString() );
	    long double newDistance = Distance(*our_lat, *our_lon, json_lat, json_lon) ;
	    if ( (itr == locations.Begin()) || newDistance < dist ) { //Dist needs setting on first run
	    	dist = newDistance ;
	    	nearest = itr;
	    }
	}

	return nearest;
};

bool datapoint::api::Setlocation(double our_lat, double our_lon) {
	/*
	 * Co-ords are converted to site ID, Name and Area when the first GetObservation or GetForecast is called as Datapoint has
	 * two site lists. It's thus possible to only request a forecast or observation site ID in runtime and save making an unused call.
	 *
	 * Originally this function did both sitelist type calls before I moved the code out. Kept the function so it's easy to add different
	 * ways to SetLocation later via overloading - by setting 'id' manually or using different references etc.
	 *
	 */
	lat = our_lat;
	lon = our_lon;
	observation_id = 0;
	forecast_id = 0;

	return true;

};


datapoint::observation datapoint::api::GetObservation() {
	/*
	 * Assuming we only ever want the last observation;
	 * Datapoint API will supply the last 24 hours if you want it though.
	 */
	rapidjson::Document json;
	rapidjson::Value::ConstValueIterator itr;

	if (observation_id == 0) {
		json = api::Call("sitelist" , OBSERVATION) ;
		itr = datapoint::api::FindNearest(&json, &lat, &lon);
		observation_id = std::stoul( (*itr)["id"].GetString() ) ;
		observation_name = (*itr)["name"].GetString() ;
		( (*itr).HasMember("unitaryAuthArea") ) ? observation_area = (*itr)["unitaryAuthArea"].GetString() : observation_area = "" ;
	}

	std::string func = to_string(observation_id) + "?res=hourly" ;
	json = api::Call(func , OBSERVATION) ;

	const rapidjson::Value& days = json["SiteRep"]["DV"]["Location"]["Period"];

	if (days.IsArray()) { //The last hour before midnight, it isn't.
		assert(days.IsArray());
		itr = days.End(); --itr; //Because rapidJSON doesn't have a Back func?
	} else {
		itr = &days;
	}

	const rapidjson::Value& hours = (*itr)["Rep"];
	if (hours.IsArray()) { //The first hour after midnight, it isn't.
		assert(hours.IsArray());
		itr = hours.End(); --itr; //Shifty pointy
	} else {
		itr = &hours;
	}

	observation obs;
	/*
	 * Note that Datapoint returns everything in strings. We don't convert the strings here so that each use
	 * of this class can be customised. There's no point converting temperature string to an float/double if the user
	 * then decides not to store the decimals and converts again to int. Some users might prefer string anyway.
	 * As a plus, it means we can return empty strings as a sort of error message to be handled.
	 */
	( (*itr).HasMember("T") ) ? obs.temperature = (*itr)["T"].GetString() : obs.temperature = "" ;
	( (*itr).HasMember("V") ) ? obs.visibility = (*itr)["V"].GetString() : obs.visibility = "" ;
	( (*itr).HasMember("D") ) ? obs.windDirection = (*itr)["D"].GetString() : obs.windDirection = "" ;
	( (*itr).HasMember("S") ) ? obs.windSpeed = (*itr)["S"].GetString() : obs.windSpeed = "" ;
	( (*itr).HasMember("G") ) ? obs.windGust = (*itr)["G"].GetString() : obs.windGust = obs.windSpeed ; //When it's not gusty it's the same?
	( (*itr).HasMember("W") ) ? obs.weatherType = std::stoul( (*itr)["W"].GetString() ) : obs.weatherType = 33 ; //Slow? 33 might become used?
	( (*itr).HasMember("P") ) ? obs.pressure = (*itr)["P"].GetString() : obs.pressure = "" ;
	( (*itr).HasMember("Pt") ) ? obs.tendency = (*itr)["Pt"].GetString() : obs.tendency = "" ;
	( (*itr).HasMember("Dp") ) ? obs.dewPoint = (*itr)["Dp"].GetString() : obs.dewPoint = "" ;
	( (*itr).HasMember("H") ) ? obs.humidity = (*itr)["H"].GetString() : obs.humidity = "" ;
	return obs;
};

datapoint::forecast datapoint::api::dumpForecast(rapidjson::Value::ConstValueIterator itr) {
	forecast frcst;
	( (*itr).HasMember("F") ) ? frcst.feelsLike = (*itr)["F"].GetString() : frcst.feelsLike = "" ;
	( (*itr).HasMember("G") ) ? frcst.windGust = (*itr)["G"].GetString() : frcst.windGust = "" ;
	( (*itr).HasMember("H") ) ? frcst.humidity = (*itr)["H"].GetString() : frcst.humidity = "" ;
	( (*itr).HasMember("T") ) ? frcst.temperature = (*itr)["T"].GetString() : frcst.temperature = "" ;
	( (*itr).HasMember("V") ) ? frcst.visibility = (*itr)["V"].GetString() : frcst.visibility = "" ;
	( (*itr).HasMember("D") ) ? frcst.windDirection = (*itr)["D"].GetString() : frcst.windDirection = "" ;
	( (*itr).HasMember("S") ) ? frcst.windSpeed = (*itr)["S"].GetString() : frcst.windSpeed = "" ;
	( (*itr).HasMember("U") ) ? frcst.uv = (*itr)["U"].GetString() : frcst.uv = "" ;
	( (*itr).HasMember("W") ) ? frcst.weatherType = std::stoul( (*itr)["W"].GetString() ) : frcst.weatherType = 33 ;
	( (*itr).HasMember("Pp") ) ? frcst.precipitationProb = (*itr)["Pp"].GetString() : frcst.precipitationProb = "" ;
	frcst.minsofday = (*itr)["$"].GetString();
	return frcst;
}

datapoint::forecast * datapoint::api::GetForecast() {
	/*
	 * This currently runs though the two-dimensional days/hours array that Datapoint uses. It's a bit blind.
	 *
	 * It's possible to use the time of the system or first 'hour' returned to work out how many hours are
	 * left in the first day, then pull four full days.
	 *
	 * TODO: There is a slight C++ issue in that the forecast array, locally defined, can not be returned outside
	 * this function without defining the array size as static. If anyone can work around this so that the
	 * memory allocation of the array is only that of a required amount I'd be extremely grateful for that pull
	 * request! (I suspect the answer is something with vectors.)
	 */
	#define MAXHOURS 28 //Sometimes a bit more, but he have to define this or compile complains frcst[] isn't static.
	static forecast frcst[MAXHOURS];

	rapidjson::Document json;
	rapidjson::Value::ConstValueIterator itr = 0; //Set 0 to avoid compiler warnings.

	if (forecast_id == 0) {
		json = api::Call("sitelist" , FORECAST) ;
		itr = datapoint::api::FindNearest(&json, &lat, &lon);
		forecast_id = std::stoul( (*itr)["id"].GetString() ) ;
		forecast_name = (*itr)["name"].GetString() ;
		( (*itr).HasMember("unitaryAuthArea") ) ? forecast_area = (*itr)["unitaryAuthArea"].GetString() : forecast_area = "" ;
	}

	std::string func = to_string(forecast_id) + "?res=3hourly" ; //You can also use "?res=daily" for a Day and Night forecast.
	json = api::Call(func , FORECAST) ;

	const rapidjson::Value& days = json["SiteRep"]["DV"]["Location"]["Period"];
	assert(days.IsArray()); //Unlike Observations, safely assume this is always an array of 4 or 5.
	static unsigned short int reads = 0;
	for (rapidjson::Value::ConstValueIterator d_itr = days.Begin(); d_itr != days.End() ; ++d_itr ) {
	const rapidjson::Value& hours = (*d_itr)["Rep"];
		if (hours.IsArray()) {
			assert(hours.IsArray());
			for (itr = hours.Begin() ; itr != hours.End()  ; ++itr ) {
				frcst[reads] = dumpForecast(itr);
				reads++;
				if (reads == MAXHOURS) { break; }
				;
			}
		} else {
			itr = &hours;
			frcst[reads] = dumpForecast(itr);
			reads++;
			if (reads == MAXHOURS) { break; }
		}
	if (reads == MAXHOURS) { break; }
	}

	return frcst;
};


