#ifndef POPULATION_HPP
#define POPULATION_HPP
#include <string>
#include <vector>
#include <map>
using namespace std;

namespace egal
{
	class population
	{
	private:
		vector<string> task_contents;
		vector<unsigned char> task_difficulty_values;
		vector<vector<unsigned char>> coexistence_preferences;
		unsigned int exercise_length;
		unsigned int population_size;
		map<unsigned int, vector<pair<vector<unsigned int>, double>>> populations;

	public:
		population
		(
			vector<string> task_contents,
			vector<unsigned char> task_difficulty_values,
			vector<vector<unsigned char>> coexistence_preferences,
			unsigned int exercise_length,
			unsigned int population_size
		):
			task_contents(task_contents),
			task_difficulty_values(task_difficulty_values),
			coexistence_preferences(coexistence_preferences),
			exercise_length(exercise_length),
			population_size(population_size)
		{}

	private:
		vector<unsigned int> create_single_population_element(bool check_difficulty);
		double calculate_single_fitness_value
		(
			vector<unsigned int> &examined_population_element,
			vector<pair<vector<unsigned int>, double>> &population,
			vector<pair<vector<unsigned int>, double>>::iterator excluded_population_element
		);

	public:
		void generate_population_options
		(
			unsigned char occurrency_multiplier,
			unsigned char valid_difficulties_treshold,
			double constraint_time_limit
		);
		vector<unsigned int> get_difficulty_options(void);
		void finalize_initial_population(unsigned int difficulty_option);
		void enhance_population
		(
			unsigned int generation_limit,
			double epsilon,
			unsigned int failed_epsilon_check_limit,
			double HMCR,
			double PAR,
			double PAR_time_limit
		);
		pair<unsigned int, vector<pair<vector<unsigned int>, double>>> get_population(void)
		{
			return *(populations.begin());
		}
	};
}
#endif /* POPULATION_HPP */
