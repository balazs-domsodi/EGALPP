#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <future>
#include <chrono>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "population.hpp"

void output(const egal::population& population, std::string file_name)
{
	std::ofstream output_file(file_name);
	std::pair<unsigned int, std::vector<std::pair<std::vector<unsigned int>, double>>> p1 = population.get_population();
	sort
	(
		p1.second.begin(), p1.second.end(),
		[](const std::pair<std::vector<unsigned int>, double>& a, const std::pair<std::vector<unsigned int>, double>& b) -> bool
		{
			return a.second < b.second;
		}
	);
	for (std::pair<std::vector<unsigned int>, double> p2 : p1.second)
	{
		bool first = true;
		for (unsigned int i : p2.first)
		{
			if (!first)
			{
				output_file << ", ";
			}
			output_file << population.get_task_contents()[i];
			first = false;
		}
		output_file << "\n";
	}
	output_file.close();
}

void debug_output(const egal::population& population, std::string file_name)
{
	std::ofstream output_file(file_name);
	output_file << "*BEGIN_DEBUG_OUTPUT*\n";
	std::pair<unsigned int, std::vector<std::pair<std::vector<unsigned int>, double>>> p1 = population.get_population();
	output_file << p1.first << "\n";
	double fitness_sum = 0;
	sort
	(
		p1.second.begin(), p1.second.end(),
		[](const std::pair<std::vector<unsigned int>, double>& a, const std::pair<std::vector<unsigned int>, double>& b) -> bool
		{
			return a.second < b.second;
		}
	);
	for (std::pair<std::vector<unsigned int>, double> p2 : p1.second)
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

size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* userp)
{
	userp->append((char*)contents, size * nmemb);
	return size * nmemb;
}

std::string read_file(const std::string& filename)
{
	std::ifstream ifs(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

	std::ifstream::pos_type fileSize = ifs.tellg();
	ifs.seekg(0, std::ios::beg);

	std::vector<char> bytes(fileSize);
	ifs.read(bytes.data(), fileSize);

	return std::string(bytes.data(), fileSize);
}

int main()
{
	while (true)
	{
		std::cout << "Please choose an operation:\n";
		std::cout << "1. Generate a new question bank\n";
		std::cout << "2. Modify coexistence preferences in a question bank\n";
		std::cout << "3. Generate an exam\n";
		std::cout << "4. Exit\n";
		
		bool valid_input = false;
		int choice;

		std::cin >> std::ws;

		while (!valid_input) {
			std::string choice_string;
			std::getline(std::cin, choice_string);
			try {
				choice = std::stoi(choice_string);
				valid_input = true;
			}
			catch (const std::invalid_argument& ia) {
				std::cerr << "Invalid choice. Please enter a number.\n";
			}
		}

		if (choice == 1)
		{
			try
			{
				std::string question_bank_name;
				std::cout << "Enter the file name for the question bank: ";
				std::getline(std::cin, question_bank_name);

				std::string client_id = read_file("client_id.txt");
				std::string client_secret = read_file("client_secret.txt");

				std::string topic, source_file_name, ques_count, page_counts;
				std::cout << "Enter topic: ";
				std::getline(std::cin, topic);
				std::cout << "Enter input file name (in the source folder): ";
				std::getline(std::cin, source_file_name);
				std::cout << "Enter question count: ";
				std::getline(std::cin, ques_count);
				std::cout << "Enter page counts (e.g., 2,5,6-10,12): ";
				std::getline(std::cin, page_counts);

				CURL* hnd = curl_easy_init();

				curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
				curl_easy_setopt(hnd, CURLOPT_URL, "https://api.prepai.io/generateQuestionsFromPdfDocApi");

				struct curl_slist* headers = NULL;
				headers = curl_slist_append(headers, ("clientId: " + client_id).c_str());
				headers = curl_slist_append(headers, ("clientSecret: " + client_secret).c_str());
				curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

				curl_mime* mime;
				curl_mimepart* part;

				mime = curl_mime_init(hnd);

				part = curl_mime_addpart(mime);
				curl_mime_name(part, "topic");
				curl_mime_data(part, topic.c_str(), CURL_ZERO_TERMINATED);

				part = curl_mime_addpart(mime);
				curl_mime_name(part, "file");
				curl_mime_filedata(part, ("source/" + source_file_name).c_str());

				part = curl_mime_addpart(mime);
				curl_mime_name(part, "quesType");
				curl_mime_data(part, "1,2", CURL_ZERO_TERMINATED);

				part = curl_mime_addpart(mime);
				curl_mime_name(part, "quesCount");
				curl_mime_data(part, ques_count.c_str(), CURL_ZERO_TERMINATED);

				part = curl_mime_addpart(mime);
				curl_mime_name(part, "pageCounts");
				curl_mime_data(part, page_counts.c_str(), CURL_ZERO_TERMINATED);

				curl_easy_setopt(hnd, CURLOPT_MIMEPOST, mime);

				std::string read_buffer;
				curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_callback);
				curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &read_buffer);

				std::cout << "Sending API request." << std::endl;
				CURLcode ret = curl_easy_perform(hnd);
				if (ret != CURLE_OK)
				{
					throw std::runtime_error("curl_easy_perform() failed: " + std::string(curl_easy_strerror(ret)));
				}
				else
				{
					std::cout << "Received API response: " << read_buffer << std::endl;

					nlohmann::json json_response = nlohmann::json::parse(read_buffer);

					std::ofstream out_file("databank/" + question_bank_name);
					if (!out_file.is_open())
					{
						throw std::runtime_error("Failed to open output file: databank/" + question_bank_name);
					}

					unsigned int question_count = 0;
					for (const auto& question : json_response["response"])
					{
						for (const auto& q : question["question"])
						{
							std::string str = q.get<std::string>();
							if (str.find("Ques  : ") == 0) {
								str = str.substr(9);
							}
							out_file << str << "\t" << question["category_type"].get<int>() << "\t";
						}

						unsigned int coexistence_count = question_count < 2 ? 1 : question_count;
						for (int i = 0; i < coexistence_count; ++i)
						{
							out_file << "10";
							if (i != coexistence_count - 1) {
								out_file << ";";
							}
						}
						out_file << "\t";

						for (size_t i = 0; i < question["options"].size(); ++i)
						{
							out_file << question["options"][i].get<std::string>();
							if (i != question["options"].size() - 1) {
								out_file << ";";
							}
						}
						out_file << "\n";

						++question_count;
					}
					out_file.close();
				}

				curl_easy_cleanup(hnd);
				curl_slist_free_all(headers);
				curl_mime_free(mime);
			}
			catch (const std::exception& e)
			{
				std::cerr << "Error: " << e.what() << std::endl;
				return 1;
			}
		}
		else if (choice == 2)
		{
			std::string question_bank_name;
			std::cout << "Enter the file name of the question bank: ";
			std::getline(std::cin, question_bank_name);

			std::vector<std::vector<int>> groups;

			std::string define_group = "yes";

			while (define_group == "yes")
			{
				std::string question_group, joint_inclusion_pref;
				std::cout << "Enter the group of questions (e.g., 2,5,6-10,12): ";
				std::getline(std::cin, question_group);
				std::cout << "Enter the joint inclusion preference (0-10): ";
				std::getline(std::cin, joint_inclusion_pref);

				std::ifstream in_file("databank/" + question_bank_name);
				std::ofstream temp_file("databank/temp.txt");

				std::vector<int> group;
				std::stringstream ss(question_group);
				std::string token;
				while (std::getline(ss, token, ','))
				{
					if (token.find('-') != std::string::npos)
					{
						std::stringstream range(token);
						int start, end;
						char dash;
						range >> start >> dash >> end;
						for (int i = start; i <= end; ++i)
						{
							group.push_back(i);
						}
					}
					else
					{
						group.push_back(std::stoi(token));
					}
				}
				groups.push_back(group);

				std::string line;
				int line_number = 1;
				while (std::getline(in_file, line))
				{
					if (std::find(group.begin(), group.end(), line_number) != group.end())
					{
						std::stringstream ss(line);
						std::string question, category, coexistence, answers;
						std::getline(ss, question, '\t');
						std::getline(ss, category, '\t');
						std::getline(ss, coexistence, '\t');
						std::getline(ss, answers, '\t');

						std::stringstream coexistence_ss(coexistence);
						std::string coexistence_token;
						std::string new_coexistence;
						unsigned int coexistence_index = 1;
						while (std::getline(coexistence_ss, coexistence_token, ';'))
						{
							if (std::find(group.begin(), group.end(), coexistence_index) != group.end())
							{
								if (std::stoi(joint_inclusion_pref) < std::stoi(coexistence_token))
								{
									new_coexistence += joint_inclusion_pref + ";";
								}
								else
								{
									new_coexistence += coexistence_token + ";";
								}
							}
							else
							{
								new_coexistence += coexistence_token + ";";
							}
							++coexistence_index;
						}
						new_coexistence.pop_back();

						temp_file << question << "\t" << category << "\t" << new_coexistence << "\t" << answers << "\n";
					}
					else
					{
						temp_file << line << "\n";
					}
					++line_number;
				}

				in_file.close();
				temp_file.close();
				std::remove(("databank/" + question_bank_name).c_str());
				std::rename("databank/temp.txt", ("databank/" + question_bank_name).c_str());

				std::cout << "Do you want to define another group of questions? (yes/no): ";
				std::getline(std::cin, define_group);
			}

			for (size_t i = 0; i < groups.size(); ++i)
			{
				for (size_t j = i + 1; j < groups.size(); ++j)
				{
					std::vector<int> intersection;
					std::set_intersection(groups[i].begin(), groups[i].end(), groups[j].begin(), groups[j].end(), std::back_inserter(intersection));
					if (intersection.empty()) {
						std::string joint_inclusion_pref;
						std::cout << "Enter the joint inclusion preference for the questions in group " << i + 1 << " and group " << j + 1 << " (0-10): ";
						std::getline(std::cin, joint_inclusion_pref);

						std::ifstream in_file("databank/" + question_bank_name);
						std::ofstream temp_file("databank/temp.txt");

						std::string line;
						int line_number = 1;
						while (std::getline(in_file, line))
						{
							if (std::find(groups[i].begin(), groups[i].end(), line_number) != groups[i].end() ||
								std::find(groups[j].begin(), groups[j].end(), line_number) != groups[j].end()) {
								std::stringstream ss(line);
								std::string question, category, coexistence, answers;
								std::getline(ss, question, '\t');
								std::getline(ss, category, '\t');
								std::getline(ss, coexistence, '\t');
								std::getline(ss, answers, '\t');

								std::stringstream coexistence_ss(coexistence);
								std::string coexistence_token;
								std::string new_coexistence;
								int coexistence_index = 1;
								while (std::getline(coexistence_ss, coexistence_token, ';'))
								{
									if ((std::find(groups[i].begin(), groups[i].end(), line_number) != groups[i].end() &&
										std::find(groups[j].begin(), groups[j].end(), coexistence_index) != groups[j].end()) ||
										(std::find(groups[j].begin(), groups[j].end(), line_number) != groups[j].end() &&
											std::find(groups[i].begin(), groups[i].end(), coexistence_index) != groups[i].end())) {
										if (std::stoi(joint_inclusion_pref) < std::stoi(coexistence_token))
										{
											new_coexistence += joint_inclusion_pref + ";";
										}
										else
										{
											new_coexistence += coexistence_token + ";";
										}
									}
									else
									{
										new_coexistence += coexistence_token + ";";
									}
									++coexistence_index;
								}
								new_coexistence.pop_back();

								temp_file << question << "\t" << category << "\t" << new_coexistence << "\t" << answers << "\n";
							}
							else
							{
								temp_file << line << "\n";
							}
							++line_number;
						}
						in_file.close();
						temp_file.close();
						std::remove(("databank/" + question_bank_name).c_str());
						std::rename("databank/temp.txt", ("databank/" + question_bank_name).c_str());
					}
				}
			}
		}
		else if (choice == 3)
		{
			srand(time(0));
			std::string question_bank_name;
			std::cout << "Enter the name of the question bank: ";
			std::getline(std::cin, question_bank_name);
			std::ifstream databank("databank/" + question_bank_name);
			if (!databank.is_open())
			{
				throw std::runtime_error("Failed to open output file: databank/" + question_bank_name);
			}

			std::string line;
			std::string token;
			std::vector<std::string> task_contents;
			std::vector<unsigned char> task_difficulty_values;
			std::vector<std::vector<unsigned char>> coexistence_preferences;
			while (std::getline(databank, line))
			{
				std::istringstream string_stream_line(line);
				std::getline(string_stream_line, token, '\t');
				task_contents.push_back(token);
				std::getline(string_stream_line, token, '\t');
				task_difficulty_values.push_back(std::stoi(token));
				std::getline(string_stream_line, token, '\t');
				std::istringstream string_stream_token(token);
				std::vector<unsigned char> coexistence_preferences_vector;
				while (std::getline(string_stream_token, token, ';'))
				{
					coexistence_preferences_vector.push_back(std::stoi(token));
				}
				coexistence_preferences.push_back(coexistence_preferences_vector);
			}
			databank.close();

			unsigned int exercise_length;
			std::cout << "Please specify the desired exercise length:\n";
			std::cin >> exercise_length;
			unsigned int population_size;
			std::cout << "Please specify the desired population size:\n";
			std::cin >> population_size;

			egal::population population(task_contents, task_difficulty_values, coexistence_preferences, exercise_length, population_size);

			population.generate_population_options(2, 3, 2.7);

			std::cout << "Please choose from the difficulty options below:\n";
			for (std::pair<unsigned int, unsigned int> options : population.get_difficulty_options_with_size())
			{
				std::cout << options.first << "\n";
			}
			unsigned int chosen_difficulty;
			std::cin >> chosen_difficulty;
			population.finalize_initial_population(chosen_difficulty);

			debug_output(population, "output/initial.txt");

			population.enhance_population(50, 0.0000000001, 10, 0.5, 0.2, 1000);

			debug_output(population, "output/enhanced.txt");
			output(population, "output/tasks.txt");

			std::cout << "Exam successfully generated!\n";
		}
		else if (choice == 4)
		{
			break;
		}
		else
		{
			std::cout << "Invalid choice. Please try again.\n";
		}
	}

	return 0;
}
