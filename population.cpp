#include <iostream>
#include <algorithm>
#include <time.h>
#include "population.hpp"
using namespace std;

vector<unsigned int> egal::population::create_single_population_element(void)
{
	vector<unsigned int> population_element;
	while (true)
	{
		population_element.clear();
		population_element.push_back(rand() % (task_contents.size() - 1));
		bool unfinishable = false;
		unsigned int try_counter = 0;
		unsigned int difficulty_sum = 0;
		unsigned int task_index;
		while (population_element.size() < exercise_length)
		{
			do
			{
				task_index = rand() % (task_contents.size() - 1);
			}
			while (find(population_element.begin(), population_element.end(), task_index) != population_element.end());
			bool valid_task_index = true;
			for (unsigned int i = 0; i <= population_element.size() - 1; ++i)
			{
				unsigned int row_to_check;
				unsigned int index_to_check;
				if (population_element[i] > task_index)
				{
					row_to_check = population_element[i];
					index_to_check = task_index;

				}
				else
				{
					row_to_check = task_index;
					index_to_check = population_element[i];
				}
				if (coexistence_preferences[row_to_check][index_to_check] == 0)
				{
					valid_task_index = false;
					++try_counter;
					if (try_counter > task_contents.size()*2)
					{
						unfinishable = true;
					}
					break;
				}
			}
			if (unfinishable)
			{
				break;
			}
			if (valid_task_index) 
			{
				try_counter = 0;
				population_element.push_back(task_index);
			}
		}
		if (unfinishable)
		{
			continue;
		}
		for (unsigned int i : population_element)
		{
			difficulty_sum += task_difficulty_values[i];
		}
		if (difficulty_sum != exercise_difficulty_goal)
		{
			continue;
		}
		break;
	}
	sort(population_element.begin(), population_element.end());
	return population_element;
}

void egal::population::define_population(void)
{
	population_elements.clear();
	while (population_elements.size() < population_size)
	{
		population_elements.push_back(create_single_population_element());
	}
}

double egal::population::calculate_single_fitness_value(vector<unsigned int> population_element)
{
	double population_element_fitness_value = 0;
	for (unsigned int i = 0; i < exercise_length - 1; ++i)
	{
		for (unsigned int j = i + 1; j < exercise_length; ++j)
		{
			population_element_fitness_value += coexistence_preferences[population_element[j]][population_element[i]];
		}
	}
	for (unsigned int i = 0; i <= population_size - 1; ++i)
	{
		vector<unsigned int> differences;
		set_symmetric_difference(population_element.begin(), population_element.end(), population_elements[i].begin(), population_elements[i].end(), back_inserter(differences));
		population_element_fitness_value += differences.size(); // (/(population_size/20)?
	}
	return population_element_fitness_value;
}

void egal::population::define_fitness_values(void)
{
	fitness_values.clear();
	for (unsigned int i = 0; i <= population_size - 1; ++i)
	{
		fitness_values.push_back(calculate_single_fitness_value(population_elements[i]));
	}
}

void egal::population::enhance_population
(
	unsigned int generation_limit,
	double epsilon,
	unsigned int failed_epsilon_check_limit,
	double HMCR,
	double PAR,
	double time_limit
)
{
	double last_average_fitness = 0;
	for (double fitness_value : fitness_values)
	{
		last_average_fitness += fitness_value;
	}
	last_average_fitness /= population_size;
	unsigned int failed_epsilon_check_count = 0;
	for (unsigned int g = 0; g <= generation_limit; ++g)
	{
		vector<unsigned int> new_population_element;
		double random_value = (double) rand() / RAND_MAX;
		bool under_time_limit = true;
		if (random_value <= HMCR)
		{
			new_population_element = population_elements[rand() % (population_size - 1)];
			if (random_value <= PAR)
			{
				time_t sequence_start_time = time(0);
				while (true)
				{
					bool valid_change = true;
					unsigned int random_new_task_index;
					unsigned int new_population_element_target_index = rand() % (exercise_length - 1);
					do
					{
						random_new_task_index = rand() % (task_contents.size() - 1);
						under_time_limit = difftime(time(0), sequence_start_time) < time_limit;
					}
					while 
					(
						(find(new_population_element.begin(), new_population_element.end(), random_new_task_index) != new_population_element.end() ||
						task_difficulty_values[new_population_element[new_population_element_target_index]] != task_difficulty_values[random_new_task_index]) &&
						under_time_limit
					);
					if (!under_time_limit)
					{
						break;
					}
					for (unsigned int i = 0; i <= exercise_length - 1; ++i)
					{
						unsigned int row_to_check;
						unsigned int index_to_check;
						if (new_population_element[i] > random_new_task_index)
						{
							row_to_check = new_population_element[i];
							index_to_check = random_new_task_index;

						}
						else
						{
							row_to_check = random_new_task_index;
							index_to_check = new_population_element[i];
						}
						if (coexistence_preferences[row_to_check][index_to_check] == 0 && i != new_population_element_target_index)
						{
							valid_change = false;
							break;
						}
					}
					if (valid_change)
					{
						new_population_element[new_population_element_target_index] = random_new_task_index;
						sort(new_population_element.begin(), new_population_element.end());
						break;
					}
				}
			}
		}
		if (random_value > HMCR || !under_time_limit)
		{
			new_population_element = create_single_population_element();
		}
		double new_population_element_fitness_value = calculate_single_fitness_value(new_population_element);
		unsigned int weakest_population_index = distance(fitness_values.begin(), min_element(fitness_values.begin(), fitness_values.end()));
		if (fitness_values[weakest_population_index] < new_population_element_fitness_value)
		{
			set_single_fitness_value(weakest_population_index, new_population_element_fitness_value);
			set_single_population_element(weakest_population_index, new_population_element);
		}
		double current_average_fitness = 0;
		for (double fitness_value : fitness_values)
		{
			current_average_fitness += fitness_value;
		}
		current_average_fitness /= population_size;
		if (current_average_fitness - last_average_fitness < epsilon)
		{
			++failed_epsilon_check_count;
		}
		else
		{
			failed_epsilon_check_count = 0;
		}
		if (failed_epsilon_check_count == failed_epsilon_check_limit)
		{
			break;
		}
		last_average_fitness = current_average_fitness;
	}
}
