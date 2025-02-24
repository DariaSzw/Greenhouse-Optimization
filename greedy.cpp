#include "greedy.h"
#include "plant.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <ctime>
#include <chrono>
#include <fstream>
#include <sstream>
#include <climits>
#include <algorithm>

using namespace std;

unsigned long long Get_time()
{
    // using namespace chrono;
    return chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
}

/*
    A function that checks if all vertices have been visited. When it finds the first unvisited one, it immediately returns a negative result.
*/
int check_all_visit(vector<Plant> plants)
{
    int tmp=plants.size();
    int free_place=0; // Assuming that all are occupied
    for (int i=0; i<tmp; i++)
    {
        if (!plants[i].get_visit_field())
            free_place++;

        if (free_place>1)
            break; // When there are at least two free ones, the loop ends
    }
    return free_place; // The result is 0, 1 or 2
}
 
/*
    Greedy algorithm
    Results are directly written to a .json file
*/
void greedy(const int &numbers_beds, vector<Plant> plants, const int* layout, const vector<vector<int>> &costs)
{
    unsigned long long time1, time2;
 
    ofstream greedy_algorithm("output/greedy_algorithm.json");
    greedy_algorithm << "[\n";
 
    /*
        The first plant is chosen based on the shortest watering time,
        The second plant is chosen based on the longest watering time
    */
 
    int first_index=-1, second_index=-1;
    int all_costs=0, all_time=0;
    int cost; // Cost of a single watering
    int first_watering_time, second_watering_time;
 
    time1=Get_time();
 
    while (check_all_visit(plants)>1) // The loop ends when all plants are watered or the last one will be left
    {
        /*
            A value of -1 means that the program must select the plant with the longest watering time that has not been watered yet
        */
        if (second_index==-1)
        {
            second_watering_time=0; // At the beginning the value is zero, in the loop it selects the largest available time
            for (int i=0; i<numbers_beds; i++)
            {
                if (!plants[layout[i]].get_visit_field() && plants[layout[i]].get_watering_time()>second_watering_time)
                    second_watering_time=plants[layout[i]].get_watering_time(); 
            }
 
            cost=INT_MAX; // At the beginning the cost is the highest, from all the plants with the same, longest watering time, chooses the one with the lowest cost
            for (int i=0; i<numbers_beds; i++)
            {
                if (!plants[layout[i]].get_visit_field() && plants[layout[i]].get_watering_time()==second_watering_time)
                {
                    if (first_index==-1) // If the first plant is not selected, it assigns the current one without considering the cost
                    {
                        second_index=i;
                    }
                    else if (costs[first_index][i]<cost && i!=first_index) // If the first plant is selected it looks for the second one with the lowest cost 
                    {
                        second_index=i;
                        cost=costs[first_index][i];  
                    }
                }
            }
            plants[layout[second_index]].change_visit_field(); // Changing the plant's designation to visited, i.e. selected for watering
        }
        /*
            A value of -1 means that the program must select the plant with the shortest watering time that has not been watered yet
        */
        if (first_index==-1) 
        {
            first_watering_time=INT_MAX; // At the beginning the maximum value, in the loop it selects the shortest available time
            for (int i=0; i<numbers_beds; i++)
            {
                if (!plants[layout[i]].get_visit_field() && plants[layout[i]].get_watering_time()<first_watering_time)
                    first_watering_time=plants[layout[i]].get_watering_time();           
            }
 
            cost=INT_MAX; // At the beginning the cost is the highest, from all the plants with the same, longest watering time, chooses the one with the lowest cost
            for (int i=0; i<numbers_beds; i++)
            {
                if ((!plants[layout[i]].get_visit_field()) && (plants[layout[i]].get_watering_time()==first_watering_time))
                {
                    /*
                        When choosing the first plant, the second one is always already selected (previous check), so you only need to look for the plant that has the lowest cost relative to the second one
                    */
                    if ((costs[i][second_index]<cost) && (i!=second_index))
                    {
                        first_index=i;
                        cost=costs[i][second_index];
                    }
                }    
            }
            
            plants[layout[first_index]].change_visit_field(); // Changing the plant's designation to visited, i.e. selected for watering
        }
 
        /*
            Saving selected plants directly to a file
        */

        greedy_algorithm << "\t{\n";
        greedy_algorithm << "\t\t\"first_plant\": \"" << plants[layout[first_index]].get_name() << "\",  \"remaining_watering_time_for_the_first_plant\": \"" << plants[layout[first_index]].get_watering_time() << "\",\n";
        greedy_algorithm << "\t\t\"second_plant\": \"" << plants[layout[second_index]].get_name() << "\",  \"remaining_watering_time_for_the_second_plant\": \"" << plants[layout[second_index]].get_watering_time() << "\",\n";
        greedy_algorithm << "\t\t\"current_cost\": \"" << all_costs << "\",  \"current_time\": \"" << all_time << "\"\n";
        greedy_algorithm << "\t},\n";
 
        all_costs+=cost; // Increasing the total cost by the cost of the plants currently watered
 
        if (first_watering_time==second_watering_time) // The case when the time of both plants remained the same
        {
            all_time+=first_watering_time;
 
            plants[layout[first_index]].change_watering_time(first_watering_time);
            plants[layout[second_index]].change_watering_time(first_watering_time);
 
            // Both plants, the time is reset, so both must be selected again
            first_index=-1;
            second_index=-1;
        }
        else if (first_watering_time<second_watering_time) // The case when the first plant finishes watering
        {
            all_time+=first_watering_time;
 
            plants[layout[first_index]].change_watering_time(first_watering_time);
            plants[layout[second_index]].change_watering_time(first_watering_time);
 
            // Only the first plant has its time reset, so the second one is still selected
            first_index=-1;
            second_watering_time=plants[layout[second_index]].get_watering_time();
        }
        else // The case when the second plant finishes watering
        {
            all_time+=second_watering_time;
 
            plants[layout[first_index]].change_watering_time(second_watering_time);
            plants[layout[second_index]].change_watering_time(second_watering_time);
 
            // Only the second plant's time is reset, so the first one remains selected
            second_index=-1;
            first_watering_time=plants[layout[first_index]].get_watering_time();
        }
    }

    if (check_all_visit(plants) == 1) // The last plant remains to be watered, we note its watering time and number
    {
        for (int i=0; i<numbers_beds; i++)
        {
            if (!plants[layout[i]].get_visit_field())
            {
                second_watering_time=plants[layout[i]].get_watering_time();
                second_index=i;
            }
        }
 
        plants[layout[second_index]].change_visit_field(); // Changing the plant's designation to visited, i.e. selected for watering
    }
 
    greedy_algorithm << "\t{\n";
 
    // After completing the loop, check if any plants require watering
    if (first_index!=-1 && second_index==-1) // The case when the first plant was left to be watered
    {
        greedy_algorithm << "\t\t\"ostatnia_roślina\": \"" << plants[layout[first_index]].get_name() << "\",  \"ostatni_czas_podlewania\": \"" << plants[layout[first_index]].get_watering_time() << "\",\n";
 
        all_time+=first_watering_time;
 
        plants[layout[first_index]].change_watering_time(first_watering_time);
    }
    else if (first_index==-1 && second_index!=-1) // A case where there is a second plant to be watered
    {
        greedy_algorithm << "\t\t\"last_plant\": \"" << plants[layout[second_index]].get_name() << "\",  \"last_watering_time\": \"" << plants[layout[second_index]].get_watering_time() << "\",\n";
 
        all_time+=second_watering_time;
 
        plants[layout[second_index]].change_watering_time(second_watering_time);  
    }
 
    greedy_algorithm << "\t\t\"total_cost\": \"" << all_costs << "\",  \"total_watering_time\": \"" << all_time << "\",\n";
 
    time2=Get_time();
 
    greedy_algorithm << "\t\t\"algorithm_duration\": \"" << (time2-time1)/1000.0 << "\"\n"; // Algorithm duration
    greedy_algorithm << "\t}\n";
 
    greedy_algorithm << "]\n";
    greedy_algorithm.close();
}