#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

int main(void)
{
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
	vector<vector<unsigned char>> coextistence_preference;
	while (getline(data_bank, line))
	{
		istringstream string_stream_line(line);
		getline(string_stream_line, token, ';');
		task_contents.push_back(token);
		getline(string_stream_line, token, ';');
		task_difficulty_values.push_back(stoi(token));
		getline(string_stream_line, token);
		istringstream string_stream_token(token);
		vector<unsigned char> coextistence_preference_vector;
		while (getline(string_stream_token, token, ','))
		{
			coextistence_preference_vector.push_back(stoi(token));
		}
		coextistence_preference.push_back(coextistence_preference_vector);
	}

	unsigned int exercise_length = 6;
	//cout << "Please specify the desired task size:\n";
	//cin >> TaskSize;
	unsigned int population_size = 30;
	//cout << "Please specify the desired population size:\n";
	//cin >> PopulationSize;

	ofstream output_file("output.txt");

	output_file.close();

	return 0;
}
