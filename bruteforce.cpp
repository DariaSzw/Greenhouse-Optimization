#include "bruteforce.h"
#include "plant.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <climits>
#include <algorithm>

using namespace std;

// Defining global variables
int brute_cost=INT_MAX;
vector<int> brute_result;
 
/*
    A brute force algorithm, called recursively to search through all possible options and save the most optimal result to global variables
*/
void brute_force_algorithm(const int &numbers_beds, vector<Plant> plants, const int* layout, const vector<vector<int>> &costs, vector<bool> used_plants, int current_plant, int all_cost, int all_time, vector<int> result)
{
    // Check how many plants remain unvisited
    int used_number=0;
    for (int i=0; i<numbers_beds; i++)
    {
        if (used_plants[i]==false)
            used_number++;
    }
 
    /*
        If all are visited and the cost of the obtained result is less than the saved one, it overwrites the data
    */
    if (used_number==0)
    {
        if (all_cost<brute_cost)
        {
            brute_cost=all_cost;
            brute_result=result;
        }
        return;
    }
    /*
        If there is one last plant to water, it is watered and written to the output path, and the last recursive call occurs
    */
    else if (used_number==1) 
    {
        if (current_plant == -1)
        {
            for (int i=0; i<numbers_beds; i++)
            {
                if (!used_plants[i])
                    current_plant=i;
            }
        }

        int tmp=plants[layout[current_plant]].get_watering_time();
        all_time+=tmp;
 
        result.push_back(layout[current_plant]); result.push_back(plants[layout[current_plant]].get_watering_time());
        result.push_back(all_cost); result.push_back(all_time);
 
        plants[layout[current_plant]].change_watering_time(tmp);
        used_plants[current_plant]=true;
 
        brute_force_algorithm(numbers_beds, plants, layout, costs, used_plants, current_plant, all_cost, all_time, result);
        return;
    }
 
    /*
        The actual part of finding plants to water
    */
 
    vector<vector<int>> plants_sort;
 
    int min;
 
    vector<bool> used=used_plants;
 
    while (true)
    {
        /*
            At the beginning the maximum value, in the loop selects the shortest available time from the remaining unused plants 
            (with each loop call the shortest watering time increases until all plants are sorted)
        */
        min=INT_MAX; 
        for (int i=0; i<numbers_beds; i++)
        {
            if (plants[layout[i]].get_Watering_Time()<min && !used[i])
                min=plants[layout[i]].get_Watering_Time();
        } 
 
        vector<int> plants_min; // Saving plants at selected watering times
        for (int i=0; i<numbers_beds; i++)
        {
            if (plants[layout[i]].get_Watering_Time()==min && !used[i])
            {
                plants_min.push_back(i);
                used[i]=true;
            }  
        }
 
        plants_sort.push_back(plants_min);
 
        /*
            Check if all plants have been used, if so the loop ends
        */
        bool temp=true; 
        for (int i=0; i<numbers_beds; i++)
        {
            if (used[i]==false)
            {
                temp=false;
                break;
            }    
        }
 
        if (temp==true)
            break;
    }
 
    int len_pl_sort=plants_sort.size()-1;
 
    /*
        If there is currently no plant selected
    */
    if (current_plant==-1)
    {
        /*
            Looping through all available plants from sorted plants
            First loop selects the plant with the longest watering time
            Second loop selects the plant with the shortest watering time
        */
        int temp1=plants_sort[len_pl_sort].size();
        for (int i=0; i<temp1; i++)
        {
            int temp2=plants_sort[0].size();
            for (int j=0; j<temp2; j++)
            {
                // If the same plant is selected, the program skips this option (you cannot water one plant as two different ones)
                if (plants_sort[0][j]==plants_sort[len_pl_sort][i])
                    continue;
 
                // If the plant selected as the one with shorter watering time has shorter or equal watering time, we record the given watering time
                if (plants[layout[plants_sort[0][j]]].get_watering_time()<=plants[layout[plants_sort[len_pl_sort][i]]].get_watering_time())
                    min=plants[layout[plants_sort[0][j]]].get_watering_time();
                // If the plant selected as the one with the longer watering time has a shorter watering time, we record the given watering timeelse
                else
                    min=plants[layout[plants_sort[len_pl_sort][i]]].get_watering_time();
 
                // Selected plants are saved to the output path
                result.push_back(layout[plants_sort[0][j]]); result.push_back(plants[layout[plants_sort[0][j]]].get_watering_time());
                result.push_back(layout[plants_sort[len_pl_sort][i]]); result.push_back(plants[layout[plants_sort[len_pl_sort][i]]].get_watering_time());
                result.push_back(all_cost); result.push_back(all_time);
 
                plants[layout[plants_sort[0][j]]].change_watering_time(min);
                plants[layout[plants_sort[len_pl_sort][i]]].change_watering_time(min);
 
                // Increasing the overall time and cost by that of a single operation
                all_time+=min;
                all_cost+=costs[plants_sort[0][j]][plants_sort[len_pl_sort][i]];
 
                // If the plant selected as the one with the shorter watering time finishes watering, the one with the longer watering time remains as the currently selected plant
                if (plants[layout[plants_sort[0][j]]].get_watering_time()==0 && plants[layout[plants_sort[len_pl_sort][i]]].get_watering_time()!=0)
                {
                    used_plants[plants_sort[0][j]]=true;
                    current_plant=plants_sort[len_pl_sort][i];
                }
                // If the plant selected as the one with the longer watering time finishes watering, the one with the shorter watering time remains as the currently selected plant
                else if (plants[layout[plants_sort[len_pl_sort][i]]].get_watering_time()==0 && plants[layout[plants_sort[0][j]]].get_watering_time()!=0)
                {
                    used_plants[plants_sort[len_pl_sort][i]]=true;
                    current_plant=plants_sort[0][j];
                }
                // If both plants finish watering, then there is no currently selected plant
                else
                {
                    used_plants[plants_sort[0][j]]=true;
                    used_plants[plants_sort[len_pl_sort][i]]=true;
                    current_plant=-1;
                }
 
                // Recursive call, relative to the currently selected plant
                brute_force_algorithm(numbers_beds, plants, layout, costs, used_plants, current_plant, all_cost, all_time, result);
            }
        }
    }
    /*
        If one plant is currently selected, it is selected as the one with the shorter watering time
    */
    else if (plants[layout[plants_sort[0][0]]].get_watering_time()==plants[layout[current_plant]].get_watering_time())
    {
        // Choosing the second plant for the longest watering time
        int temp1=plants_sort[len_pl_sort].size();
        for (int i=0; i<temp1; i++)
        {
            // If it finds the same plant it skips it
            if (current_plant==plants_sort[len_pl_sort][i])
                continue;
 
            // If the currently selected plant has a shorter or equal watering time compared to the one selected as the one with the longest time, we record the given watering time
            if (plants[layout[current_plant]].get_watering_time()<=plants[layout[plants_sort[len_pl_sort][i]]].get_watering_time())
                min=plants[layout[current_plant]].get_watering_time();
            // If the currently selected plant has a longer watering time compared to the one selected as the one with the longest time, we record the given watering time
            else
                min=plants[layout[plants_sort[len_pl_sort][i]]].get_watering_time();
 
            // Saving selected plants to the result vector
            result.push_back(layout[current_plant]); result.push_back(plants[layout[current_plant]].get_watering_time());
            result.push_back(layout[plants_sort[len_pl_sort][i]]); result.push_back(plants[layout[plants_sort[len_pl_sort][i]]].get_watering_time());
            result.push_back(all_cost); result.push_back(all_time);
 
            plants[layout[current_plant]].change_watering_time(min);
            plants[layout[plants_sort[len_pl_sort][i]]].change_watering_time(min);
 
            all_time+=min;
            all_cost+=costs[current_plant][plants_sort[len_pl_sort][i]];
 
            // If the current plant finishes watering, the plant with the longer watering time is selected as the currently selected plant
            if (plants[layout[current_plant]].get_watering_time()==0 && plants[layout[plants_sort[len_pl_sort][i]]].get_watering_time()!=0)
            {
                used_plants[current_plant]=true;
                current_plant=plants_sort[len_pl_sort][i];
            }
            // If the current plant does not finish watering, it remains as the currently selected one
            else if (plants[layout[plants_sort[len_pl_sort][i]]].get_watering_time()==0 && plants[layout[current_plant]].get_watering_time()!=0)
            {
                used_plants[plants_sort[len_pl_sort][i]]=true;
                //current_plant=current_plant;
            }
            // If both plants finish watering, then there is no currently selected plant
            else
            {
                used_plants[current_plant]=true;
                used_plants[plants_sort[len_pl_sort][i]]=true;
                current_plant=-1;
            }
 
            // Recursive function call for current plant
            brute_force_algorithm(numbers_beds, plants, layout, costs, used_plants, current_plant, all_cost, all_time, result);   
        }
    }
    /*
        If one plant is currently selected, it is selected as the one with the longer watering time
    */
    else
    {
        // Choosing the second plant for the shortest watering time
        int temp1=plants_sort[0].size();
        for (int j=0; j<temp1; j++)
        {
            // If it finds the same plant it skips it
            if (plants_sort[0][j]==current_plant)
                continue;
 
            // If the currently selected plant has a shorter or equal watering time compared to the one selected as the one with the shortest time, we record the given watering time
            if (plants[layout[plants_sort[0][j]]].get_watering_time()<=plants[layout[current_plant]].get_watering_time())
                min=plants[layout[plants_sort[0][j]]].get_watering_time();
            // If the currently selected plant has a longer watering time compared to the one selected as the one with the shortest time, we record the given watering time
            else
                min=plants[layout[current_plant]].get_watering_time();
 
            // Saving selected plants to the result vector
            result.push_back(layout[plants_sort[0][j]]); result.push_back(plants[layout[plants_sort[0][j]]].get_watering_time());
            result.push_back(layout[current_plant]); result.push_back(plants[layout[current_plant]].get_watering_time());
            result.push_back(all_cost); result.push_back(all_time);
 
            plants[layout[plants_sort[0][j]]].change_watering_time(min);
            plants[layout[current_plant]].change_watering_time(min);
 
            all_time+=min;
            all_cost+=costs[plants_sort[0][j]][current_plant];
 
            // If the current plant does not finish watering, it remains as the currently selected one
            if (plants[layout[plants_sort[0][j]]].get_watering_time()==0 && plants[layout[current_plant]].get_watering_time()!=0)
            {
                used_plants[plants_sort[0][j]]=true;
                //current_plant=current_plant;
            }
            // If the current plant finishes watering, the plant with the shorter watering time is selected as the currently selected plant
            else if (plants[layout[current_plant]].get_watering_time()==0 && plants[layout[plants_sort[0][j]]].get_watering_time()!=0)
            {
                used_plants[current_plant]=true;
                current_plant=plants_sort[0][j];
            }
            // If both plants finish watering, then there is no currently selected plant
            else
            {
                used_plants[plants_sort[0][j]]=true;
                used_plants[current_plant]=true;
                current_plant=-1;
            }
 
            // Recursive function call for current plant
            brute_force_algorithm(numbers_beds, plants, layout, costs, used_plants, current_plant, all_cost, all_time, result);
        }
    }
}