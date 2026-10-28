// pp-projekt.cpp: definiuje punkt wejścia dla aplikacji.
//

#include <iostream>
#include "format"
#include "concepts"
template <typename T, typename Pred = decltype([](const T&) {return true;}) >
requires (std::integral<T> || std::floating_point<T> || std::same_as<T, std::string>)
	&& std::predicate<Pred, const T&>
T input(std::string_view prompt, std::string_view error = "Błąd \n", Pred predicate = {}) {
	T buff;
	while (true) {
		std::cout << prompt;
		if (!(std::cin >>buff)) {
			std::cout << error;
			continue;
		}
		return buff;
	}
}




const std::string menu[] = {
	"Simulate next day",
	"Change model parameters",
	"Exit",
};


void showPopulation(int day, int susceptible, int infected, int recovered);

int showMenu();

void simulateDay(int& susceptible, int& infected, int& recovered, int& day, double infectionRate, double recoveryRate);
	bool checkPopulation(int susceptalbe, int infected, int recovered);
int main()
{
	int susceptible = 80;
	int infected = 10;
	int recovered = 10;
	int day = 0;


	double infectionRate = 0.20;
	double recoveryRate = 0.10;
	while (true) {

	showPopulation(day, susceptible, infected, recovered);
	auto choice = showMenu();
	switch (--choice) {
	case 0: 
		simulateDay(susceptible, infected, recovered, day, infectionRate, recoveryRate);
		continue;
		break;
	case 1: 

		break;
	case 2: 
		exit(0);
		break;
	default: {
		std::cout << "wrong choice \n";
		continue;
	}
	}
	}
	return 0;
}
void showPopulation(int day, int susceptible, int infected, int recovered) {

	int total = susceptible + infected + recovered;

	std::cout << std::format(
		"Day: {} \n"
		"Susceptible: {} \n"
		"Infected: {} \n"
		"Recovered: {} \n"
		"Total: {} \n",
		day, susceptible, infected, recovered, total);
}
int showMenu() {
	std::cout << "===MENU== \n";
	for (int i = 0; i < std::size(menu); ++i) {
		std::cout << i + 1 << ". " << menu[i] << "\n";
	}
	auto choice = input<int>("Your choice: ");

	return choice;
}



void simulateDay(int& susceptible, int& infected, int& recovered, int& day, double infectionRate, double recoveryRate) {
	int prevInfected = infected;
	int prevSusceptible = susceptible;
	int prevRecovered = recovered;

	int newInfected = static_cast<int>( std::floor(prevSusceptible * infectionRate));
	int newRecovered =static_cast<int>(std::floor(prevInfected * recoveryRate));

	int  tempSusceptible = prevSusceptible - newInfected;
	int tempInfected = prevInfected + newInfected - newRecovered;
	int tempRecovered = prevRecovered + recovered;

	if (checkPopulation(tempSusceptible, tempInfected, tempRecovered)) {
		susceptible = tempSusceptible;
		infected = tempInfected;
		recovered = tempRecovered;
		++day;

	}
	else {
		std::cout << "bład symulacji";
	}
}
bool checkPopulation(int susceptalbe, int infected, int recovered) {
	if ((susceptalbe + infected + recovered) != 100)
		return false;
	if (susceptalbe < 0 || infected < 0 || recovered < 0)
		return false;

	return true;
}