
// Simplified C++ Implementation of Genetic Algorithm for Class Scheduling
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>

struct ScheduleEntry {
    int subjectId;
    int teacherId;
    int timeSlot;
    int roomId;
};

using Chromosome = std::vector<ScheduleEntry>;
using Population = std::vector<Chromosome>;

const int POPULATION_SIZE = 100;
const int GENERATIONS = 500;
const double CROSSOVER_RATE = 0.8;
const double MUTATION_RATE = 0.1;
const int NUM_SUBJECTS = 5;
const int NUM_TIMESLOTS = 36;
const int NUM_ROOMS = 5;
const int NUM_TEACHERS = 5;

std::mt19937 rng(time(0));

int randomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

ScheduleEntry randomScheduleEntry(int subjectId) {
    return { subjectId, randomInt(0, NUM_TEACHERS - 1), randomInt(0, NUM_TIMESLOTS - 1), randomInt(0, NUM_ROOMS - 1) };
}

Chromosome createRandomChromosome() {
    Chromosome c;
    for (int i = 0; i < NUM_SUBJECTS; ++i) {
        c.push_back(randomScheduleEntry(i));
    }
    return c;
}

Population initializePopulation() {
    Population pop;
    for (int i = 0; i < POPULATION_SIZE; ++i) {
        pop.push_back(createRandomChromosome());
    }
    return pop;
}

int evaluateFitness(const Chromosome& c) {
    int conflicts = 0;
    for (size_t i = 0; i < c.size(); ++i) {
        for (size_t j = i + 1; j < c.size(); ++j) {
            if (c[i].timeSlot == c[j].timeSlot) {
                if (c[i].teacherId == c[j].teacherId || c[i].roomId == c[j].roomId) {
                    conflicts++;
                }
            }
        }
    }
    return -conflicts;
}

Chromosome crossover(const Chromosome& a, const Chromosome& b) {
    Chromosome child = a;
    int point = randomInt(0, NUM_SUBJECTS - 1);
    for (int i = point; i < NUM_SUBJECTS; ++i) {
        child[i] = b[i];
    }
    return child;
}

void mutate(Chromosome& c) {
    if (randomInt(0, 100) < MUTATION_RATE * 100) {
        int i = randomInt(0, NUM_SUBJECTS - 1);
        c[i].timeSlot = randomInt(0, NUM_TIMESLOTS - 1);
        c[i].roomId = randomInt(0, NUM_ROOMS - 1);
    }
}

Chromosome selectParent(const Population& pop, const std::vector<int>& fitnesses) {
    int minFitness = *std::min_element(fitnesses.begin(), fitnesses.end());
    int totalFitness = 0;
    for (int f : fitnesses) totalFitness += (f - minFitness + 1);

    int r = randomInt(0, totalFitness);
    int acc = 0;
    for (size_t i = 0; i < pop.size(); ++i) {
        acc += (fitnesses[i] - minFitness + 1);
        if (acc >= r) return pop[i];
    }
    return pop.back();
}

void runGA() {
    Population pop = initializePopulation();
    for (int gen = 0; gen < GENERATIONS; ++gen) {
        std::vector<int> fitnesses;
        for (const auto& c : pop) fitnesses.push_back(evaluateFitness(c));

        Population newPop;
        while (newPop.size() < POPULATION_SIZE) {
            Chromosome parent1 = selectParent(pop, fitnesses);
            Chromosome parent2 = selectParent(pop, fitnesses);
            Chromosome child = (randomInt(0, 100) < CROSSOVER_RATE * 100) ? crossover(parent1, parent2) : parent1;
            mutate(child);
            newPop.push_back(child);
        }
        pop = newPop;
    }

    Chromosome best = *std::max_element(pop.begin(), pop.end(), [](const Chromosome& a, const Chromosome& b) {
        return evaluateFitness(a) < evaluateFitness(b);
    });

    std::cout << "Best schedule with fitness = " << evaluateFitness(best) << ":\n";
    for (const auto& e : best) {
        std::cout << "Subject " << e.subjectId << " - Teacher " << e.teacherId << " - Slot " << e.timeSlot << " - Room " << e.roomId << "\n";
    }
}

int main() {
    runGA();
    return 0;
}
