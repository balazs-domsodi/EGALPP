#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "population.hpp"
using namespace std;

void debug_output(const egal::population &population, string file_name)
{
	ofstream output_file(file_name);
	output_file << "*BEGIN_DEBUG_OUTPUT*\n";
	pair<unsigned int, vector<pair<vector<unsigned int>, double>>> p1 = population.get_population();
	output_file << p1.first << "\n";
	double fitness_sum = 0;
	sort
	(
		p1.second.begin(), p1.second.end(),
		[](const pair<vector<unsigned int>, double> &a, const pair<vector<unsigned int>, double> &b) -> bool
		{ 
			return a.second < b.second;
		}
	);
	for (pair<vector<unsigned int>, double> p2 : p1.second)
	{
		for (unsigned int i : p2.first)
		{
			output_file << i << ",";
		}
		fitness_sum += p2.second;
		output_file << " " << p2.second << "\n";
	}
	output_file << "Average fitness value: " << fitness_sum / p1.second.size() << "\n";
	output_file << "*END_DEBUG_OUTPUT*\n";
	output_file.close();
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
	data_bank.close();

	unsigned int exercise_length = 6;
	//cout << "Please specify the desired exercise length:\n";
	//cin >> exercise_length;
	unsigned int population_size = 30;
	//cout << "Please specify the desired population size:\n";
	//cin >> population_size;

	egal::population population(task_contents, task_difficulty_values, coexistence_preferences, exercise_length, population_size);
	
	population.generate_population_options(2, 3, 1);

	for (unsigned int i : population.get_difficulty_options())
	{
		cout << i << "\n";
	}
	unsigned int chosen_difficulty;
	cin >> chosen_difficulty;
	population.finalize_initial_population(chosen_difficulty);
	
	debug_output(population, "initial.txt");
	
	population.enhance_population(50, 0.0000000001, 10, 0.5, 0.2, 1);

	debug_output(population, "enhanced.txt");

	return 0;
}
