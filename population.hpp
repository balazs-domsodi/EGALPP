#ifndef POPULATION_HPP
#define POPULATION_HPP
#include <string>
#include <vector>
#include <map>

namespace egal
{
	class population
	{
	private:
		std::vector<std::string> task_contents;
		std::vector<unsigned char> task_difficulty_values;
		std::vector<std::vector<unsigned char>> coexistence_preferences;
		unsigned int exercise_length;
		unsigned int population_size;
		std::map<unsigned int, std::vector<std::pair<std::vector<unsigned int>, double>>> population_options;
		unsigned char number_of_options_goal;
		unsigned int difficulty_difference_goal_in_options;

	public:
		population
		(
			std::vector<std::string> task_contents,
			std::vector<unsigned char> task_difficulty_values,
			std::vector<std::vector<unsigned char>> coexistence_preferences,
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
		std::vector<unsigned int> create_single_population_element(bool check_difficulty) const;
		double calculate_single_fitness_value
		(
			const std::vector<unsigned int> &examined_population_element,
			std::vector<std::pair<std::vector<unsigned int>, double>>::const_iterator excluded_population_element
		) const;

	public:
		void generate_population_options
		(
			unsigned char occurrency_multiplier,
			unsigned char valid_difficulties_treshold,
			double constraint_time_limit
		);
		std::vector<std::pair<unsigned int, unsigned int>> get_difficulty_options_with_size(void) const;
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
		std::pair<unsigned int, std::vector<std::pair<std::vector<unsigned int>, double>>> get_population(void) const
		{
			return *(population_options.cbegin());
		}
		std::vector<std::string> get_task_contents() const
		{
			return task_contents;
		}
		unsigned char get_number_of_options_goal() const
		{
			return number_of_options_goal;
		}
		unsigned int get_difficulty_difference_goal_in_options() const
		{
			return difficulty_difference_goal_in_options;
		}
	};
}
#endif /* POPULATION_HPP */
