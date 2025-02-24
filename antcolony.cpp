#include "antcolony.h"
#include "plant.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <climits>
#include <algorithm>

using namespace std;

/*
    An ant algorithm that returns a vector with the most optimal solution
*/
vector<int> ant_colony(const int &numbers_beds, vector<Plant> plants, const int* layout, const vector<vector<int>> &costs)
{
    int first_watering_time, second_watering_time;
 
    vector<vector<double>> heuristic(numbers_beds, vector<double>(numbers_beds));
 
    /*
        Creating a two-dimensional vector whose content is the inverse of the costs (heuristic)
    */
    for (int i = 0; i<numbers_beds; ++i) 
    {
        for (int j = 0; j<numbers_beds; ++j) 
        {
            if (i != j) 
                heuristic[i][j] = 1.0/costs[i][j];
            else 
                heuristic[i][j] = 0.0;
        }
    }
 
    // Initially set pheromone value for each possible connection to 1
    vector<vector<double>> pheromones(numbers_beds, vector<double>(numbers_beds, 1.0));
 
    vector<vector<int>> all_paths;
    vector<int> path_cost;
 
    int tmp;
 
    for (int a=0; a<numbers_beds*numbers_beds; a++) // Each loop execution is one 'ant'
    {
        vector<bool> visited_plants(numbers_beds, false); // Set all plants as unvisited
        int visited_fields=0, all_cost=0, all_time=0;
 
        /*
             The first plant is chosen based on the shortest watering time,
             The second plant is chosen based on the longest watering time
        */
 
        int first_index=-1, second_index=-1;
 
        vector<int> choice_first, choice_second; // Vectors that record plants with the same shortest watering time and the longest
        vector<int> path; // Currently created path of selected plants
 
        for (int i=0; i<numbers_beds; i++)
            plants[i].reset_watering_time();
 
        while (visited_fields<numbers_beds-1) // The loop is executed until all plants have been visited
        {
            /*
                 A value of -1 means that the program must select the plant with the longest watering time that has not been watered yet
            */
            if (second_index==-1)
            {
                // A loop that checks backwards whether any of the plants added earlier have already been selected for watering
                for (int i=choice_second.size()-1; i>=0; i--)
                {
                    if (visited_plants[choice_second[i]])
                        choice_second.erase(choice_second.begin() + i);
                }
 
                if (choice_second.size()==0) // If there are no selected plants with the same longest watering time
                {
                    second_watering_time=0; // At the beginning the value is zero, in the loop it selects the largest available time
 
                    for (int i=0; i<numbers_beds; i++)
                    {
                        if (!visited_plants[i] && plants[layout[i]].get_watering_time()>second_watering_time)
                            second_watering_time=plants[layout[i]].get_watering_time();
                    }
 
                    // For the longest time found, all that have the same are added to the vector from which the ant will be able to choose a solution
                    for (int i=0; i<numbers_beds; i++)
                    {
                        if (!visited_plants[i] && plants[layout[i]].get_watering_time()==second_watering_time)
                        {
                            choice_second.push_back(i);
                        }
 
                    }
                }
 
                if (first_index==-1) // If the first one is not selected, the plant is drawn from the available ones
                {
                    if (choice_second.size() > 1)
                    {
                        int temp=rand()%choice_second.size();
                        second_index=choice_second[temp];
                    }
                    else 
                        second_index=choice_second[0];
                }
                else // The first plant is selected
                {
                    /*
                        Creating a vector of probabilities of selecting individual plants based on pheromones and heuristics
                    */
                    vector<double> probabilities(choice_second.size(), 0.0);
                    double sum_probabilities = 0.0;
 
                    tmp=choice_second.size();
                    for (int i = 0; i<tmp; i++) 
                    {
                        if (!visited_plants[choice_second[i]]) 
                        {
                            probabilities[i] = pheromones[first_index][choice_second[i]]*heuristic[first_index][choice_second[i]]*heuristic[first_index][choice_second[i]];
                            sum_probabilities += probabilities[i];
                        }
                    }
 
                    double rand_prob = ((double)rand() / RAND_MAX) * sum_probabilities; // Random probability value
                    double cumulative_prob = 0.0;
 
                    tmp=choice_second.size();
                    for (int i = 0; i<tmp; i++) 
                    {
                        if (!visited_plants[choice_second[i]]) 
                        {
                            cumulative_prob += probabilities[i];
                            if (cumulative_prob >= rand_prob) // When the sum of individual probabilities exceeds the one drawn, a given plant is selected (thanks to this, ants have a chance to visit different paths)
                            {
                                second_index=choice_second[i];
                                break;
                            }
                        }
                    }
                }
                // Marking a plant as visited
                choice_second.erase(find(choice_second.begin(), choice_second.end(), second_index), choice_second.end());
                visited_plants[second_index]=true;
            }
 
            /*
                A value of -1 means that the program must select the plant with the shortest watering time that has not been watered yet
            */
            if (first_index==-1)
            {
                // A loop that checks backwards whether any of the plants added earlier have already been selected for watering
                for (int i=choice_first.size()-1; i>=0; i--)
                {
                    if (visited_plants[choice_first[i]])
                        choice_first.erase(choice_first.begin() + i);
                }
 
                if (choice_first.size()==0) // If there are no selected plants with the same longest watering time
                {
                    first_watering_time=INT_MAX; // At the beginning the maximum value, in the loop it selects the shortest available time
 
                    for (int i=0; i<numbers_beds; i++)
                    {
                        if (!visited_plants[i] && plants[layout[i]].get_watering_time()<first_watering_time)
                            first_watering_time=plants[layout[i]].get_watering_time();
                    }
 
                    // For the shortest time found, all that have the same are added to the vector from which the ant will be able to choose a solution
                    for (int i=0; i<numbers_beds; i++)
                    {
                        if (!visited_plants[i] && plants[layout[i]].get_watering_time()==first_watering_time)
                            choice_first.push_back(i);
                    }
                }
 
                /*
                    Creating a vector of probabilities of selecting individual plants based on pheromones and heuristics
                */
                vector<double> probabilities(choice_first.size(), 0.0);
                double sum_probabilities = 0.0;
 
                tmp=choice_first.size();
                for (int i = 0; i<tmp; i++) 
                {
                    if (!visited_plants[choice_first[i]]) 
                    {
                        probabilities[i] = pheromones[choice_first[i]][second_index]*heuristic[choice_first[i]][second_index]*heuristic[choice_first[i]][second_index];
                        sum_probabilities += probabilities[i];
                    }
                }
 
                double rand_prob = ((double)rand() / RAND_MAX) * sum_probabilities; // Random probability value
                double cumulative_prob = 0.0;
 
                tmp=choice_first.size();
                for (int i = 0; i<tmp; i++) 
                {
                    if (!visited_plants[choice_first[i]]) 
                    {
                        cumulative_prob += probabilities[i];
                        if (cumulative_prob >= rand_prob) // When the sum of individual probabilities exceeds the one drawn, a given plant is selected (thanks to this, ants have a chance to visit different paths)
                        {
                            first_index=choice_first[i];
                            choice_first.erase(find(choice_first.begin(), choice_first.end(), first_index), choice_first.end());
                            break;
                        }
                    }
                }
                // Marking a plant as visited
                visited_plants[first_index]=true;
            }
 
            // Recording individual watering times
            first_watering_time=plants[layout[first_index]].get_watering_time();
            second_watering_time=plants[layout[second_index]].get_watering_time();
 
            // Adding selected plants to the vector to be able to get the best option at the end
            path.push_back(layout[first_index]); path.push_back(first_watering_time);
            path.push_back(layout[second_index]); path.push_back(second_watering_time);
            path.push_back(all_cost); path.push_back(all_time);
 
            // Increasing the total cost by the cost of the current watering
            all_cost+=costs[first_index][second_index];
 
            if (first_watering_time==second_watering_time) // The case when the time of both plants remained the same
            {
                all_time+=first_watering_time;
 
                plants[layout[first_index]].change_watering_time(first_watering_time);
                plants[layout[second_index]].change_watering_time(first_watering_time);
                visited_fields+=2;
 
                // Both plants, the time is reset, so both must be selected again
                first_index=-1;
                second_index=-1;
            }
            else if (first_watering_time<second_watering_time) // The case when the first plant finishes watering
            {
                all_time+=first_watering_time;
 
                plants[layout[first_index]].change_watering_time(first_watering_time);
                plants[layout[second_index]].change_watering_time(first_watering_time);
                visited_fields++;
 
                // Only the first plant has its time reset, so the second one is still selected
                first_index=-1;
            }
            else // The case when the second plant finishes watering
            {
                all_time+=second_watering_time;
 
                plants[layout[first_index]].change_watering_time(second_watering_time);
                plants[layout[second_index]].change_watering_time(second_watering_time);
                visited_fields++;
 
                // Only the second plant's time is reset, so the first one remains selected
                second_index=-1;
            }
 
            // Additional check to see if not all plants have been visited
            int temp=0;
            for (int i=0; i<numbers_beds; i++)
            {
                if (visited_plants[i])
                    temp++;
            }
            if (temp == numbers_beds)
                break;
 
        }

        if (visited_fields == numbers_beds-1) // The last plant remains to be watered, we note its watering time and number
        {
            for (int i=0; i<numbers_beds; i++)
            {
                if (!visited_plants[i])
                {
                    second_watering_time=plants[layout[i]].get_watering_time();
                    second_index=i;
                }
            }
     
            visited_plants[second_index]=true; // Changing the plant's designation to visited, i.e. selected for watering
        }
 
        // After completing the loop, check if any plants require watering
        if (first_index!=-1 && second_index==-1) // The case when the first plant was left to be watered
        {
            first_watering_time=plants[layout[first_index]].get_watering_time();
 
            path.push_back(layout[first_index]); path.push_back(first_watering_time);    
 
            plants[layout[first_index]].change_watering_time(first_watering_time);
 
            all_time+=first_watering_time;
        }
        else if (first_index==-1 && second_index!=-1) // A case where there is a second plant to be watered
        {
            second_watering_time=plants[layout[second_index]].get_watering_time();
 
            path.push_back(layout[second_index]); path.push_back(second_watering_time);
 
            plants[layout[second_index]].change_watering_time(second_watering_time);  
 
            all_time+=second_watering_time;
        }
 
        path.push_back(all_cost); path.push_back(all_time);
 
        all_paths.push_back(path);
        path_cost.push_back(all_cost);
 
        /*
             Check, if more than 5 ants found a solution and the last 5 obtained costs are the same, the algorithm ends, it returns the path with the lowest cost
        */
        if (a>5) 
        {
            int temp=path_cost.size();
            if (path_cost[temp-1]==path_cost[temp-2] && path_cost[temp-2]==path_cost[temp-3] && path_cost[temp-3]==path_cost[temp-4] && path_cost[temp-4]==path_cost[temp-5])
            {
                int min=INT_MAX, min_index;
                for (int i=0; i<temp; i++)
                {
                    if (path_cost[i]<min)
                    {
                        min=path_cost[i];
                        min_index=i;
                    }
                }
                return all_paths[min_index];
            }
 
        }
    }
 
    // If the algorithm finishes executing without looping through the cost results, the algorithm returns the path with the lowest cost obtained
    int min=INT_MAX, min_index;
    tmp=path_cost.size();
    for (int i=0; i<tmp; i++)
    {
        if (path_cost[i]<min)
        {
            min=path_cost[i];
            min_index=i;
        }
    }
    return all_paths[min_index];
}