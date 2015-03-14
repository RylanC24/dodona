#ifndef FitnessFunctions_h
#define FitnessFunctions_h

#include "FitnessResult.h"
#include "InputModels/InputModel.h"
#include "Keyboard.h"
#include "WordList.h"

namespace FitnessFunctions {
    FitnessResult MonteCarloEfficiency(Keyboard& keyboard, InputModel& model, WordList& words, unsigned int iterations);
    FitnessResult ExactEfficiency(Keyboard& keyboard, InputModel& model, WordList& words);
    FitnessResult FastEfficiency(Keyboard& keyboard, InputModel& model, WordList& words, double exp_par);
    FitnessResult RadixMonteCarloEfficiency(Keyboard& keyboard, InputModel& model, WordList& words, unsigned int iterations, unsigned int possibility_tries);
};

#endif
