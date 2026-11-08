// pp-projekt.cpp: definiuje punkt wejścia dla aplikacji.
//

#include <iostream>
#include <string>
#include <sstream>
#include <format>
#include <concepts>
#include <cmath>
#include <iomanip>
#include <algorithm>


template <typename T, typename Pred = decltype([](const T&) {return true;}) >
requires (std::integral<T> || std::floating_point<T> || std::same_as<T, std::string>)
	&& std::predicate<Pred, const T&>
T input(std::string_view prompt, std::string_view errorMessage = "Wrong data. Try one more time \n", Pred predicate = {}) {
		std::string line;
	while (true) {
		std::cout << prompt;
		if (!(std::getline(std::cin, line))) {
			return T{};
		}
		T value{};

		if constexpr (std::same_as<T, std::string>) {
			value = line;
			if (predicate(value)) return value;
		}
		else {
			std::stringstream stream(line);
			char residual;

			if ((stream >> value) && !(stream >> residual)) {
				if (predicate(value)) return value;
			}
		}
		std::cout << errorMessage;
	}
}






void showPopulation(int day, int susceptible, int infected, int recovered);

int showMenu(const std::string menu[], int size);


void simulateDay(int& susceptible, int& infected, int& recovered, int& day, double infectionRate, double recoveryRate, int populationHistory[101][3]);

bool checkPopulation(int susceptalbe, int infected, int recovered);

void modifyParameters(double& infectionRate, double& recoveryRate);


int main()
{
	const std::string menu[] = {
		"Simulate next day",
		"Change model parameters",
		"Exit",
	};
	const int MAX_DAYS = 101;
	const int menu_size = sizeof(menu) / sizeof(menu[0]);

	int susceptible = 80;
	int infected = 10;
	int recovered = 10;
	int day = 0;


	double infectionRate = 0.20;
	double recoveryRate = 0.10;

	int populationHistory[MAX_DAYS][3] = {
		{susceptible, infected, recovered},
	};
	int dailyNewInfections[MAX_DAYS] = {};
	int dailyRecoveries[MAX_DAYS] = {};

	bool running = true;
	while (running) {

		showPopulation(day, susceptible, infected, recovered);

		auto choice = showMenu(menu, menu_size);
		switch (choice) {
			case 1: 
				simulateDay(susceptible, infected, recovered, day, infectionRate, recoveryRate, populationHistory);
				continue;
				break;
			case 2: 
				modifyParameters(infectionRate, recoveryRate);
				break;
			case 3: 
				std::cout << "Exiting... \n";
				running = false;
				break;
			default: {
				std::cout << "Wrong choice \n";
				break;
			}
		}
	}
	return 0;
}
void showPopulation(int day, int susceptible, int infected, int recovered) {
	const int label_w = 25;
	const int value_w = 5;
	int total = susceptible + infected + recovered;
	auto showRow = [&label_w, &value_w](std::string_view label, int value) {
		std::cout 
			<< std::left << std::setw(label_w) << label 
			<< std::right << std::setw(value_w) << value 
			<< "\n";
		};
	//std::cout << std::left << std::setw(label_w) << "Day: " << std::right << std::setw(value_w) << day << "\n"
	//	<< "Susceptible: " << susceptible << " \n"
	//	<< "Infected: " << infected << "\n"
	//	<< "Recovered: " << recovered << "\n"
	//	<< "Total: " << total << " \n";

	showRow("Day: ", day);
	showRow("Susceptible: ", susceptible);
	showRow("Infected: ", infected);
	showRow("Recovered: ", recovered);
	showRow("Total: ", total);
}
int showMenu(const std::string menu[], int size) {


	std::cout << "\n===MENU===\n";
	for (int i = 0; i < size; ++i) {
		std::cout << i + 1 << ". " << menu[i] << "\n";
	}
	return input<int>("Your choice: ", "", [size](int choice) {
		return choice >= 1 &&  choice <= size;
	});

}



void simulateDay(int& susceptible, int& infected, int& recovered, int& day, double infectionRate, double recoveryRate, int populationHistory[101][3]) {
const	int prevInfected = infected;
const 	int prevSusceptible = susceptible;
const 	int prevRecovered = recovered;

	int newInfected = static_cast<int>(std::floor(prevInfected * infectionRate));
	int newRecovered =static_cast<int>(std::floor(prevInfected * recoveryRate));


	newInfected = std::min(newInfected, prevSusceptible);
	newRecovered = std::min(newRecovered, prevInfected);

	int  tempSusceptible = prevSusceptible - newInfected;
	int tempInfected = prevInfected + newInfected - newRecovered;
	int tempRecovered = prevRecovered + newRecovered;

	if (checkPopulation(tempSusceptible, tempInfected, tempRecovered)  && (++day && day <= 100)) {
		susceptible = tempSusceptible;
		infected = tempInfected;
		recovered = tempRecovered;
		
		populationHistory[day][0] = tempSusceptible;
		populationHistory[day][1] = tempInfected;
		populationHistory[day][2] = tempRecovered;

	}
	else {
		std::cout << "Simulation error. Wrong population\n";
		--day;
	}
}
bool checkPopulation(int susceptalbe, int infected, int recovered) {
	if ((susceptalbe + infected + recovered) != 100)
		return false;
	if (susceptalbe < 0 || infected < 0 || recovered < 0)
		return false;

	return true;
}

void modifyParameters(double& infectionRate, double& recoveryRate) {
	std::cout << "\n--- Change Model Parameters ---\n";

	auto validRate = [](double rate) -> bool {
			return rate >= 0.0 && rate <= 1.0;
		};

	double newInfection = input<double>(
		"Enter infection rate (0-1): ",
		"Invalid rate! Must be between 0.0 and 1.0.\n",
		validRate
	);

	double newRecovery = input<double>(
		"Enter recovery rate (0-1): ",
		"Invalid rate! Must be between 0.0 and 1.0.\n",
		validRate
	);

	infectionRate = newInfection;
	recoveryRate = newRecovery;

	std::cout << "Parameters updated successfully\n";

}