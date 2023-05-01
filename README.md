This program is an improved metaheuristic algorithm for exercise generation. It is designed to address the widely recognized problem of generating diverse exercises to measure students’ knowledge on various topics. The algorithm generates more subsets of tasks in such a way that the quality of these subsets should be good enough according to a predefined quality matrix. The difficulty values of these subsets should also be equal.

# Steps to take before running the program

The textual content of the tasks, their difficulty values, and the value expressing the preference for joint inclusion taken in pairs (coexistence preferences) must be located in the databank folder. Any number of such files can be created with the extension txt.

It is necessary to build these files line by line, where each line expresses the properties of a task, and individual properties are separated by a tab character. The first value is the textual content of the task (essentially the task itself), the second value is its difficulty value on a scale of 1-5, and the last value is the values expressing the coexistence preferences taken with previous tasks, separated by semicolons, on a scale of 0-10, where 0 means prohibiting the inclusion of the two task pairs together. In the file, the last value of the first line plays a placeholder role, its value has no significance, it can be entered as desired in the range 0-255 - its existence serves the purpose that the values of the coexistence preferences are indexed in the same way as the other properties of the task queues, since otherwise the first element would not have such a value, since there is no value expressing a common inclusion preference related to the elements preceding it. An example of this input file is databank/test.txt.

# How to operate the program (Windows)

1.	To start the program, run the compiled main.exe in the root directory.
2.	A command-line window opens, where you can enter the name of the question bank file you would like to use. If you do not wish to create your own question bank, enter "test", which will use a pre-designed test question bank which was created for demonstration purposes. After entering the name of the file, hit the enter key on your keyboard.
3.	Next, you will be asked to specify the desired exercise length. For example, in the case of the test question bank, 6 can be used as a valid exercise length value. After entering the desired value, hit the enter key.
4.	For the next input parameter, you need to provide in this interface, please enter the desired population size and hit the enter key. For the test question bank, 30 can be used as an example population size value.
5.	You will be prompted with three difficulty options. Please choose a difficulty for the task sequences, by entering one of the displayed numbers and hitting the enter key again.
6.	Once the exercise generation is complete, the command-line window will close. The compiled task sequences can be found in the tasks.txt file in the output folder. For additional background information, refer to the initial.txt and enhanced.txt files, which contain the indexes of the task sequences in the initial and final populations, along with their respective fitness values.

If you wish to use your own question bank, you can create a file following the rules described at the beginning of this chapter. Alternatively, you can generate a question bank using the test.xlsm Excel macro file located in the databank folder. This file was created for demonstration purposes and can reliably generate a question bank with up to 300 items. In this Excel file, you can directly edit the content of each task, their corresponding difficulty values, and coexistence preferences. You can also use the button 'Set New Co-existence Preference' to change the coexistence preference of a task pair and it is recommended to use the button 'Add New Tasks' to include new tasks that can be edited later. By clicking the ‘Export Data’ button, a txt question bank file will be created from the data given with the necessary formatting and the name of the xlsm file, overwriting any existing file of the same name in the folder.

