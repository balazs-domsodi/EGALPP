#include <algorithm>
#include <time.h>
#include <stdexcept>
#include "population.hpp"
using namespace std;

vector<unsigned int> egal::population::create_single_population_element(bool check_difficulty)
{
	vector<unsigned int> population_element;
	while (true)
	{
		population_element.clear();
		population_element.push_back(rand() % (task_contents.size() - 1));
		bool unfinishable = false;
		unsigned int try_counter = 0, difficulty_sum = 0, task_index;
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
				unsigned int row_to_check, index_to_check;
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
		if (check_difficulty)
		{
			for (unsigned int i : population_element)
			{
				difficulty_sum += task_difficulty_values[i];
			}
			if (difficulty_sum != populations.begin()->first)
			{
				continue;
			}
		}
		break;
	}
	sort(population_element.begin(), population_element.end());
	return population_element;
}

void egal::population::generate_population_options
(
	unsigned char occurrency_multiplier,
	unsigned char valid_difficulties_treshold,
	double constraint_time_limit
)
{
	if (occurrency_multiplier == 0)
	{
		throw invalid_argument("generate_population_options::occurrency_multiplier = 0");
	}
	unsigned int occurrency_treshold = population_size * occurrency_multiplier;
	if (valid_difficulties_treshold == 0)
	{
		throw invalid_argument("generate_population_options::valid_difficulties_treshold = 0");
	}
	populations.clear();
	unsigned int difference_goal, minimum_difficulty = 0, maximum_difficulty = 0;
	if (valid_difficulties_treshold > 1)
	{
		vector<unsigned char> sorted_task_difficulty_values = task_difficulty_values;
		sort(sorted_task_difficulty_values.begin(), sorted_task_difficulty_values.end());
		for (unsigned int i = 0; i < exercise_length; ++i)
		{
			minimum_difficulty += sorted_task_difficulty_values[i];
			maximum_difficulty += sorted_task_difficulty_values[sorted_task_difficulty_values.size() - 1 - i];
		}
		difference_goal = (maximum_difficulty - minimum_difficulty) / (valid_difficulties_treshold - 1);
	}
	else
	{
		difference_goal = 1;
	}
	time_t sequence_start_time = time(0);
	while (true)
	{
		pair<vector<unsigned int>, double> p(create_single_population_element(false), 0);
		unsigned int difficulty_sum = 0;
		for (unsigned int i : p.first)
		{
			difficulty_sum += task_difficulty_values[i];
		}
		map<unsigned int, vector<pair<vector<unsigned int>, double>>>::iterator it = populations.find(difficulty_sum);
		if (it != populations.end())
		{
			if (find(it->second.begin(), it->second.end(), p) == it->second.end())
			{
				it->second.push_back(p);
			}
		}
		else
		{
			populations[difficulty_sum] = vector<pair<vector<unsigned int>, double>>(1, p);
		}
		vector<unsigned int> valid_difficulty_options;
		for (it = populations.begin(); it != populations.end(); ++it)
		{
			if (it->second.size() >= occurrency_treshold)
			{
				valid_difficulty_options.push_back(it->first);
				break;
			}
		}
		for (++it; it != populations.end(); ++it)
		{
			if (it->second.size() >= occurrency_treshold && valid_difficulty_options.back() + difference_goal <= it->first)
			{
				valid_difficulty_options.push_back(it->first);
			}
		}
		if (valid_difficulty_options.size() >= valid_difficulties_treshold)
		{
			for (it = populations.begin(); it != populations.end();)
			{
				if (find(valid_difficulty_options.begin(), valid_difficulty_options.end(), it->first) != valid_difficulty_options.end())
				{
					it->second.resize(population_size);
					++it;
				}
				else
				{
					map<unsigned int, vector<pair<vector<unsigned int>, double>>>::iterator tmp_it = it++;
					populations.erase(tmp_it);
				}
			}
			break;
		}
		if (difftime(time(0), sequence_start_time) >= constraint_time_limit)
		{
			if (valid_difficulties_treshold != 1)
			{
				if (difference_goal != 1)
				{
					--difference_goal;
				}
				else
				{
					if (valid_difficulties_treshold > 2)
					{
						--valid_difficulties_treshold;
						difference_goal = (maximum_difficulty - minimum_difficulty) / (valid_difficulties_treshold - 1);
					}
					else
					{
						valid_difficulties_treshold = 1;
					}
				}
				sequence_start_time = time(0);
			}
		}
	}
}

vector<unsigned int> egal::population::get_difficulty_options(void)
{
	vector<unsigned int> difficulty_options;
	for(pair<unsigned int, vector<pair<vector<unsigned int>, double>>> population : populations)
	{
		difficulty_options.push_back(population.first);
	}
	return difficulty_options;
}

double egal::population::calculate_single_fitness_value
(
	vector<unsigned int> &examined_population_element,
	vector<pair<vector<unsigned int>, double>> &population,
	vector<pair<vector<unsigned int>, double>>::iterator excluded_population_element
)
{
	double fitness_value = 0;
	for (unsigned int i = 0; i < exercise_length - 1; ++i)
	{
		for (unsigned int j = i + 1; j < exercise_length; ++j)
		{
			fitness_value += coexistence_preferences[examined_population_element[j]][examined_population_element[i]];
		}
	}
	for (vector<pair<vector<unsigned int>, double>>::iterator it = population.begin(); it != population.end(); ++it)
	{
		if (it != excluded_population_element)
		{
			vector<unsigned int> differences(exercise_length);
			fitness_value +=
			set_difference
			(
				examined_population_element.begin(), examined_population_element.end(),
				it->first.begin(), it->first.end(),
				vector<unsigned int>::iterator(differences.begin())
			)
			- differences.begin(); // (/(population_size/20)?
		}
	}
	return fitness_value;
}

void egal::population::finalize_initial_population(unsigned int difficulty_option)
{
	if (populations.find(difficulty_option) == populations.end())
	{
		throw invalid_argument("finalize_initial_population::difficulty_option not in map");
	}
	for (map<unsigned int, vector<pair<vector<unsigned int>, double>>>::iterator it = populations.begin(); it != populations.end();)
	{
		if (it->first != difficulty_option)
		{
			map<unsigned int, vector<pair<vector<unsigned int>, double>>>::iterator tmp_it = it++;
			populations.erase(tmp_it);
		}
		else
		{
			++it;
		}
	}
	for (vector<pair<vector<unsigned int>, double>>::iterator it = populations.begin()->second.begin(); it != populations.begin()->second.end(); ++it)
	{
		it->second = calculate_single_fitness_value(it->first, populations.begin()->second, populations.begin()->second.end());
	}
}

void egal::population::enhance_population
(
	unsigned int generation_limit,
	double epsilon,
	unsigned int failed_epsilon_check_limit,
	double HMCR,
	double PAR,
	double PAR_time_limit
)
{
	double last_average_fitness = 0;
	for (pair<vector<unsigned int>, double> p : populations.begin()->second)
	{
		last_average_fitness += p.second;
	}
	last_average_fitness /= population_size;
	unsigned int failed_epsilon_check_count = 0;
	for (unsigned int g = 0; g <= generation_limit; ++g)
	{
		vector<unsigned int> new_population_element;
		double random_value = (double) rand() / RAND_MAX;
		bool under_PAR_time_limit = true;
		if (random_value <= HMCR)
		{
			new_population_element = populations.begin()->second[rand() % (population_size - 1)].first;
			if (random_value <= PAR)
			{
				time_t sequence_start_time = time(0);
				while (true)
				{
					unsigned int random_new_task_index;
					unsigned int new_population_element_target_index = rand() % (exercise_length - 1);
					bool coexistence_validity;
					do
					{
						random_new_task_index = rand() % (task_contents.size() - 1);
						coexistence_validity = true;
						for (unsigned int i = 0; i <= exercise_length - 1; ++i)
						{
							unsigned int row_to_check, index_to_check;
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
							if (i != new_population_element_target_index && coexistence_preferences[row_to_check][index_to_check] == 0)
							{
								coexistence_validity = false;
								break;
							}
						}
						under_PAR_time_limit = difftime(time(0), sequence_start_time) < PAR_time_limit;
					}
					while 
					(
						(find(new_population_element.begin(), new_population_element.end(), random_new_task_index) != new_population_element.end() ||
						task_difficulty_values[new_population_element[new_population_element_target_index]] != task_difficulty_values[random_new_task_index] ||
						!coexistence_validity) &&
						under_PAR_time_limit
					);
					if (!under_PAR_time_limit)
					{
						break;
					}
					new_population_element[new_population_element_target_index] = random_new_task_index;
					sort(new_population_element.begin(), new_population_element.end());
					break;
				}
			}
		}
		if (random_value > HMCR || !under_PAR_time_limit)
		{
			new_population_element = create_single_population_element(true);
		}
		unsigned int weakest_fitness_value = populations.begin()->second[0].second;
		unsigned int weakest_i;
		for (unsigned int i = 0; i < population_size; ++i)
		{
			if (weakest_fitness_value > populations.begin()->second[i].second)
			{
				weakest_fitness_value = populations.begin()->second[i].second;
				weakest_i = i;
			}
		}
		double current_average_fitness = 0;
		for (pair<vector<unsigned int>, double> p : populations.begin()->second)
		{
			current_average_fitness += p.second;
		}
		current_average_fitness /= population_size;
		double new_fitness_value = calculate_single_fitness_value(new_population_element, populations.begin()->second, populations.begin()->second.begin() + weakest_i);
		double new_average_fitness = 0;
		if (weakest_fitness_value < new_fitness_value)
		{
			populations.begin()->second.push_back(pair<vector<unsigned int>, double>(new_population_element, 0));
			for (vector<pair<vector<unsigned int>, double>>::iterator it = populations.begin()->second.begin(); it != populations.begin()->second.end(); ++it)
			{
				new_average_fitness += calculate_single_fitness_value(it->first, populations.begin()->second, populations.begin()->second.begin() + weakest_i);
			}
			populations.begin()->second.pop_back();
			new_average_fitness /= population_size;
			if (new_average_fitness > current_average_fitness)
			{
				populations.begin()->second[weakest_i].first = new_population_element;
				populations.begin()->second[weakest_i].second = new_fitness_value;
				for (vector<pair<vector<unsigned int>, double>>::iterator it = populations.begin()->second.begin(); it != populations.begin()->second.end(); ++it)
				{
					it->second = calculate_single_fitness_value(it->first, populations.begin()->second, populations.begin()->second.end());
				}
				current_average_fitness = 0;
				for (pair<vector<unsigned int>, double> p : populations.begin()->second)
				{
					current_average_fitness += p.second;
				}
				current_average_fitness /= population_size;
			}
		}
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
