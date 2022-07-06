#ifndef POPULATION_HPP
#define POPULATION_HPP
#include <iostream>
#include <string>
#include <vector>
using namespace std;

namespace egal
{
	class population
	{
	private:
		vector<string> task_contents;
		vector<unsigned char> task_difficulty_values;
		unsigned int exercise_difficulty_goal;
		vector<vector<unsigned char>> coexistence_preferences;
		unsigned int number_of_tasks;
		unsigned int exercise_length;
		unsigned int population_size;
		vector<vector<unsigned int>> population_elements;
		vector<double> fitness_values;
	public:
		population
		(
			vector<string> task_contents,
			vector<unsigned char> task_difficulty_values,
			unsigned int exercise_difficulty_goal,
			vector<vector<unsigned char>> coexistence_preferences,
			unsigned int exercise_length,
			unsigned int population_size
		):
			task_contents(task_contents),
			task_difficulty_values(task_difficulty_values),
			coexistence_preferences(coexistence_preferences),
			exercise_length(exercise_length),
			exercise_difficulty_goal(exercise_difficulty_goal),
			population_size(population_size)
		{
			define_population();
			define_fitness_values();
		}

		vector<unsigned int> create_single_population_element(void);

		void define_population(void);

		double calculate_single_fitness_value(vector<unsigned int> population_element);

		void define_fitness_values(void);

		void enhance_population
		(
			unsigned int generation_limit = 50,
			double epsilon = 0.0000000001,
			unsigned int failed_epsilon_check_limit = 10,
			double HMCR = 0.5,
			double PAR = 0.2,
			double time_limit = 1
		);

		vector<string> get_task_contents(void)
		{
			return task_contents;
		}

		vector<unsigned char> get_task_difficulty_values(void)
		{
			return task_difficulty_values;
		}

		unsigned int get_exercise_difficulty_goal(void)
		{
			return exercise_difficulty_goal;
		}

		vector<vector<unsigned int>> get_population_elements(void)
		{
			return population_elements;
		}

		vector<double> get_fitness_values(void)
		{
			return fitness_values;
		}
	};
}

#endif /* POPULATION_HPP */
