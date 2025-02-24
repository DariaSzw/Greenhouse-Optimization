#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <climits>
#include "json.hpp"
#include "plant.h"
#include "greedy.h"
#include "antcolony.h"
#include "bruteforce.h"

using namespace std;
using json = nlohmann::json;

int main()
{   
    srand(time(nullptr));

    int numbers_rows=9; // Number of rows in the greenhouse
    int numbers_beds_in_row=3; // Number of beds in a row
    int numbers_beds=numbers_rows*numbers_beds_in_row; // Number of beds in the greenhouse

    /*
        Loading plants from the database
    */
    
    vector <Plant> plants;
    int lenght_vector_plants=0;
    
    while (lenght_vector_plants!=numbers_beds)
    {
        ifstream file("output/plant_base.json"); // The database contains 50 different plants, if the expected number of plants is greater, the file opens again

        if (!file.is_open()) 
        {
            cerr << "Unable to read plant database from file!" << endl;
            return 1;
        }

        json jsonData;
        file >> jsonData;

        for (const auto& item : jsonData) {
            string name = item["name"];
            int water_demand = item["demand_(in_liters)"];

            plants.emplace_back(name, water_demand);
            lenght_vector_plants++;
            
            if (lenght_vector_plants==numbers_beds)
                break;
        }

        file.close();
    }

    /*
        Physical parameters needed to calculate water flow
    */

    int hose_diameter=5;
    double water_pressure=6;  // Water pressure per minute
    
    int k=hose_diameter-4; // 'k' is the coefficient needed for calculations, hose_diameter is the hose diameter in millimeters (min 5mm)
    double water_flow=k*water_pressure/60; // Water flow in seconds

    int layout[numbers_beds]; // A board responsible for the arrangement of plants - which one was planted where
    int random_plant;

    int field=0; // A variable indicating how many plants were planted
    while (field<numbers_beds)
    {
        random_plant=rand()%numbers_beds; // Plant draw
        
        int temp=0;

        /*
            A board to check whether the plant has already been planted
        */
        for (int j=field-1; j>=0; j--)
        {
            if (random_plant==layout[j])
            {
                temp++;
                break;
            }
        }

        if (temp==0) // If the plant has not been planted yet
        {
            layout[field]=random_plant; // planting a plant in a specific place
            plants[random_plant].planting(field);
            plants[random_plant].counting_watering_time(water_flow);
            field++;
        }
    }

    /*
        Vector with costs, between all possible plants - costs correspond to distances
    */
    vector<vector<int>> costs(numbers_beds,vector<int>(numbers_beds));

    for (int first=0; first<numbers_beds; first++)
    {
        for (int second=0; second<numbers_beds; second++)
        {
            if (first==second)
                costs[first][second]=0;
            else if (first<second)
                costs[first][second]=second-first;
            else
                costs[first][second]=numbers_beds+second-first;
        }
    }

    greedy(numbers_beds, plants, layout, costs);

    unsigned long long time1=Get_time();
    vector<int> ant_colony_algorithm=ant_colony(numbers_beds, plants, layout, costs);
    unsigned long long time2=Get_time();
    int aco_len=ant_colony_algorithm.size();

    ofstream ant_colony_optimisation("output/ant_colony_optimisation.json");
    ant_colony_optimisation << "[\n";

    for (int i=0; i<aco_len-4; i+=6)
    {
        ant_colony_optimisation << "\t{\n";
        ant_colony_optimisation << "\t\t\"first_plant\": \"" << plants[ant_colony_algorithm[i]].get_name() << "\",  \"remaining_watering_time_for_the_first_plant\": \"" << ant_colony_algorithm[i+1] << "\",\n";
        ant_colony_optimisation << "\t\t\"second_plant\": \"" << plants[ant_colony_algorithm[i+2]].get_name() << "\",  \"remaining_watering_time_for_the_second_plant\": \"" << ant_colony_algorithm[i+3] << "\",\n";
        ant_colony_optimisation << "\t\t\"current_cost\": \"" << ant_colony_algorithm[i+4] << "\",  \"current_time\": \"" << ant_colony_algorithm[i+5] << "\"\n";
        ant_colony_optimisation << "\t},\n";
    }

    ant_colony_optimisation << "\t{\n";

    int al=aco_len%6; // If the value is 4 it means that there is one single plant left at the end, if not it is 2
    if (al == 4)
    {
        ant_colony_optimisation << "\t\t\"last_plant\": \"" << plants[ant_colony_algorithm[aco_len-4]].get_name() << "\",  \"last_watering_time\": \"" << ant_colony_algorithm[aco_len-3] << "\",\n";
    }
    
    ant_colony_optimisation << "\t\t\"total_cost\": \"" << ant_colony_algorithm[aco_len-2] << "\",  \"total_watering_time\": \"" << ant_colony_algorithm[aco_len-1] << "\",\n";
    ant_colony_optimisation << "\t\t\"algorithm_duration\": \"" << (time2-time1)/1000.0 << "\"\n"; //algorithm duration
    ant_colony_optimisation << "\t}\n";

    ant_colony_optimisation << "]\n";
    ant_colony_optimisation.close();

    vector<int> result;
    int all_time=0, all_cost=0, current_plant=-1;
    vector<bool> used_plants(numbers_beds, false);
    time1=Get_time();
    brute_force_algorithm(numbers_beds, plants, layout, costs, used_plants, current_plant, all_cost, all_time, result);
    time2=Get_time();

    ofstream brute_force_algorithm("output/brute_force_algorithm.json");
    brute_force_algorithm << "[\n";

    int brute_len=brute_result.size();
    
    for (int i=0; i<brute_len-4; i+=6)
    {
        brute_force_algorithm << "\t{\n";
        brute_force_algorithm << "\t\t\"first_plant\": \"" << plants[brute_result[i]].get_name() << "\",  \"remaining_watering_time_for_the_first_plant\": \"" << brute_result[i+1] << "\",\n";
        brute_force_algorithm << "\t\t\"second_plant\": \"" << plants[brute_result[i+2]].get_name() << "\",  \"remaining_watering_time_for_the_second_plant\": \"" << brute_result[i+3] << "\",\n";
        brute_force_algorithm << "\t\t\"current_cost\": \"" << brute_result[i+4] << "\",  \"current_time\": \"" << brute_result[i+5] << "\"\n";
        brute_force_algorithm << "\t},\n";
    }

    brute_force_algorithm << "\t{\n";

    int bf=brute_len%6; // If the value is 4 it means that there is one single plant left at the end, if not it is 2
    if (bf == 4)
    {
        brute_force_algorithm << "\t\t\"last_plant\": \"" << plants[brute_result[brute_len-4]].get_name() << "\",  \"last_watering_time\": \"" << brute_result[brute_len-3] << "\",\n";
    }
    
    brute_force_algorithm << "\t\t\"total_cost\": \"" << brute_result[brute_len-2] << "\",  \"total_watering_time\": \"" << brute_result[brute_len-1] << "\",\n";
    brute_force_algorithm << "\t\t\"algorithm_duration\": \"" << (time2-time1)/1000.0 << "\"\n"; //algorithm duration
    brute_force_algorithm << "\t}\n";

    brute_force_algorithm << "]\n";
    brute_force_algorithm.close();

    return 0;
}