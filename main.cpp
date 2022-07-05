#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "population.hpp"
using namespace std;

void debug_output(egal::population population, vector<vector<unsigned char>> coexistence_preferences)
{
	cout << "*BEGIN_DEBUG_OUTPUT*\n";
	vector<vector<unsigned int>> population_elements = population.get_population_elements();
	for (unsigned int i = 0; i < population_elements.size(); ++i)
	{
		unsigned int difficulty_sum = 0;
		for (unsigned int task_index : population_elements[i])
		{
			cout << task_index << ",";
			difficulty_sum += population.get_task_difficulty_values()[task_index];
			for (unsigned int j = 0; j < population_elements[i].size() - 1; ++j)
			{
				for (unsigned int k = j + 1; k < population_elements[i].size(); ++k)
				{
					if (coexistence_preferences[population_elements[i][k]][population_elements[i][j]] == 0)
					{
						cout << "(!" << population_elements[i][k] << "-" << population_elements[i][j] << "!)";
					}
				}
			}
		}
		if (difficulty_sum != population.get_exercise_difficulty_goal())
		{
			cout << "difficulty error!\n";
		}
		cout << population.get_fitness_values()[i] << "\n";
	}
	cout << "*END_DEBUG_OUTPUT*\n";
}

int main(void)
{
	srand (time(0));
	string file_name = "test";
	//cout << "Please declare the file name (without extension) in the databank folder you would like to use:\n";
	//cin >> file_name;
	ifstream data_bank;
	data_bank.open("databank\\" + file_name + ".txt", ios::in);
	if (data_bank.fail())
	{
		cout << "Unable to open file.\n";
	}
	else
	{
		cout << "File opened succesfully.\n";
	}

	string line;
	string token;
	vector<string> task_contents;
	vector<unsigned char> task_difficulty_values;
	unsigned int exercise_difficulty_goal;
	vector<vector<unsigned char>> coexistence_preferences;
	while (getline(data_bank, line))
	{
		istringstream string_stream_line(line);
		getline(string_stream_line, token, ';');
		task_contents.push_back(token);
		getline(string_stream_line, token, ';');
		task_difficulty_values.push_back(stoi(token));
		getline(string_stream_line, token);
		istringstream string_stream_token(token);
		vector<unsigned char> coexistence_preferences_vector;
		while (getline(string_stream_token, token, ','))
		{
			coexistence_preferences_vector.push_back(stoi(token));
		}
		coexistence_preferences.push_back(coexistence_preferences_vector);
	}

	unsigned int exercise_length = 6;
	//cout << "Please specify the desired exercise length:\n";
	//cin >> exercise_length;
	unsigned int population_size = 30;
	//cout << "Please specify the desired population size:\n";
	//cin >> population_size;

	bool calculate_difficulty = false;
	//cout << "Please specify wheter you would like to use difficulty values (y/n):\n";
	//cin >> ;

	if (calculate_difficulty)
	{

	}
	else
	{
		exercise_difficulty_goal = exercise_length;
		task_difficulty_values = vector<unsigned char>(task_contents.size(), 1);
	}

	egal::population population(task_contents, task_difficulty_values, exercise_difficulty_goal, coexistence_preferences, exercise_length, population_size);
	
	debug_output(population, coexistence_preferences);
	
	population.enhance_population();

	debug_output(population, coexistence_preferences);

	ofstream output_file("output.txt");

	output_file.close();
	return 0;
}
