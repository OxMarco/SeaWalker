/**
 * @file    ReadConfig.hpp
 *
 * @brief   Reads configuration params from a json file.
 *
 */

#ifndef READCONFIG_HPP
#define READCONFIG_HPP

#include "DBHandler.hpp"
#include "../Libs/json/include/nlohmann/json.hpp"
#include <string>

using json = nlohmann::json;

class ReadConfig {
   public:
    /**
     * @brief Parses the json file and puts the data in the json object cfg
     */
    static void readFromJsonFile(const std::string& filename, json& cfg);

    /**
     * @brief Returns true if the key exists in cfg, and false if it doesn't
     */
    static bool exists(json& cfg, const std::string& key);

    /**
     * @brief Returns the waypoints in the db in the json object wp
     */
    static void waypointsInJson(json& cfg, DBHandler db);

    /**
     * @brief Updates the database with the data in the file
     * (Reads the file and puts it in the database)
     */
    static bool updateConfiguration(const std::string& file, DBHandler db);

   private:
    /**
     * @brief Private help function to add "id": "1" to the json
     *  Needs to be there, because the "id" field is required
     *  for the dbHandler functions to work
     */
    static json merge(const json& a, const json& b);
};

#endif /* READCONFIG_HPP */
