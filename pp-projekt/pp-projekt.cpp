// pp-projekt.cpp: definiuje punkt wejścia dla aplikacji.
//

#include <iostream>
#include <string>
#include <sstream>
#include <format>
#include <concepts>
#include <cmath>
#include <iomanip>
#include <functional>
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


const int MAX_DAYS = 101;
struct SimulationState {
	int day = 0;
	int suspectible = 80;
	int recovered = 10;
	int infected = 10;
};
struct ModelParameters {
	double infectionRate = 0.20;
	double recoveryRate = 0.20;
};

struct SimulationHistory {
	int populationHistory[MAX_DAYS][3] = {};
	int dailyNewInfections[MAX_DAYS] = {};
	int dailyRecoveries[MAX_DAYS] = {};
};

struct MenuItem {
	std::string_view label;
	std::function<bool()> action;
};

void showPopulation(int day, int susceptible, int infected, int recovered);

int showMenu(const std::string menu[], int size);

void showSimulationHistory(
	int populationHistory[MAX_DAYS][3],
	int dailyNewInfections[MAX_DAYS],
	int dailyRecoveries[MAX_DAYS],
	int day
);

void simulateDay(
	int& susceptible, int& infected, int& recovered, int& day, 
	double infectionRate, double recoveryRate, 
	int populationHistory[MAX_DAYS][3], 
	int dailyNewInfections[MAX_DAYS],
	int dailyRecoveries[MAX_DAYS]
);

bool checkPopulation(int susceptalbe, int infected, int recovered);

void modifyParameters(double& infectionRate, double& recoveryRate);


int main()
{
	const std::string menu[] = {
		"Simulate next day",
		"Change model parameters",
		"Show simulation history",
		"Exit",
	};

	//const MenuItem m[] = {
	//	{"Simulate next day", [&] {}}
	//};
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
				simulateDay(
					susceptible, infected, recovered, day, 
					infectionRate, recoveryRate,
					populationHistory,
					dailyNewInfections,
					dailyRecoveries);
				continue;
				break;
			case 2: 
				modifyParameters(infectionRate, recoveryRate);
				break;
			case 3:
				showSimulationHistory(populationHistory, dailyNewInfections, dailyRecoveries, day);
				break;
			case 4: 
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

	const int num_w = 4;
	for (int i = 0; i < size; ++i) {
		std::cout <<std::right << std::setw(num_w) << i + 1 << ". " << std::left << menu[i] << "\n";
	}
	return input<int>("Your choice [1-"+std::to_string(size) + "]: ", "", [size](int choice) {
		return choice >= 1 &&  choice <= size;
	});

}



void simulateDay(
	int& susceptible, int& infected, int& recovered, int& day, 
	double infectionRate, double recoveryRate,
	int populationHistory[101][3],
	int dailyNewInfections[MAX_DAYS],
	int dailyRecoveries[MAX_DAYS]
) {
	if (day >= MAX_DAYS - 1) {
		std::cout << "Simulation error; Max days \n";
		return;
	}
	const int prevInfected = infected;
	const int prevSusceptible = susceptible;
	const int prevRecovered = recovered;

	int newInfected = static_cast<int>(std::floor(prevInfected * infectionRate));
	int newRecovered =static_cast<int>(std::floor(prevInfected * recoveryRate));


	newInfected = std::min(newInfected, prevSusceptible);
	newRecovered = std::min(newRecovered, prevInfected);

	int tempSusceptible = prevSusceptible - newInfected;
	int tempInfected = prevInfected + newInfected - newRecovered;
	int tempRecovered = prevRecovered + newRecovered;

	if (checkPopulation(tempSusceptible, tempInfected, tempRecovered)  && (++day && day <= 100)) {
		susceptible = tempSusceptible;
		infected = tempInfected;
		recovered = tempRecovered;
		
		populationHistory[day][0] = tempSusceptible;
		populationHistory[day][1] = tempInfected;
		populationHistory[day][2] = tempRecovered;
		dailyNewInfections[day - 1] = tempInfected - prevInfected;
		dailyRecoveries[day - 1] = tempRecovered - prevRecovered;
	}
	else {
		std::cout << "Simulation error. Wrong population\n";
		--day;
	}
}
bool checkPopulation(int susceptalbe, int infected, int recovered) {
	return (susceptalbe >= 0 && infected >= 0 && recovered >= 0)
		&& ((susceptalbe + infected + recovered) == 100);
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
void showSimulationHistory(
	int populationHistory[MAX_DAYS][3],
	int dailyNewInfections[MAX_DAYS],
	int dailyRecoveries[MAX_DAYS],
	int day
) {
	const int day_w = 6, value_w = 11, diff_w = 10;
	std::cout
		<< "\nPopulation history:\n";

	std::cout << std::left
		<< std::setw(day_w) << "Day" << " | "
		<< std::setw(value_w) << "Suspectible" << " | "
		<< std::setw(value_w) << "Infected" << " | "
		<< std::setw(value_w) << "Recovered" << " | "
		<< std::setw(diff_w) << "+Infected" << " | "
		<< std::setw(diff_w) << "+Recovered"
		<< "\n";


	for (int i = 0; i <= day; ++i) {
		std::cout << std::right
			<< std::setw(day_w) << i << " | "
			<< std::setw(value_w) << populationHistory[i][0] << " | "
			<< std::setw(value_w) << populationHistory[i][1] << " | "
			<< std::setw(value_w) << populationHistory[i][2] << " | "
			<< std::setw(diff_w) << (i == 0 ? " - " : std::to_string(dailyNewInfections[i - 1])) << " | "
			<< std::setw(diff_w) << (i == 0 ? " - " : std::to_string(dailyRecoveries[i - 1]))
			<< "\n";
	}
	std::cout << "\n";
}