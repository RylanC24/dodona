//boost include files
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>

//core include files
#include "FitnessFunctions.h"
#include "FitnessResult.h"
#include "Serialization.h"
#include "DataFormat.h"

#include "InputModels/SimpleGaussianModel.h"
#include "InputModels/SimpleInterpolationModel.h"
#include "InputModels/NeuralNetworkModel.h"
#include "InputModels/Interpolation.h"
#include "RadixTree.h"

#include <sstream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

//special boost-python headers that contain functinos necessary for the python interface
#include "RadixTree_py.h"
#include "Polygon_py.h"
#include "Keyboard_py.h"
#include "WordList_py.h"
#include "InputModels/InputModel_py.h"
#include "InputModels/SimpleInterpolationModel_py.h"
#include "DataFormat_py.h"
#ifndef NO_FANN
#include "InputModels/NeuralNetworkModel_py.h"
#endif


using namespace boost::python;

//for general use in deep copying
template<typename T> const T DeepCopy(const T& v, dict d) { return T(v); }

//for general use in pickling
template<typename T> struct serialization_pickle_suite : pickle_suite {
    static object getstate(const T& t) {
        std::ostringstream os;
        boost::archive::text_oarchive oa(os);
        oa << t;
        return str(os.str());
    };
    static void setstate(T& t, object entries) {
        str s = extract<str>(entries)();
        std::string st = extract<std::string>(s)();
        std::istringstream is(st);

        boost::archive::text_iarchive ia(is);
        ia >> t;
    };
};


/********************************************************/
/***************** Python Module ************************/
/********************************************************/

BOOST_PYTHON_MODULE(core)
{

/***************** WordList class ***********************/
    
    class_<WordList>("WordList")
        .def("AddWord", &WordList::AddWord, AddWord_overloads())
        .def("Occurances", Occurances1)
        .def("Occurances", Occurances2)
        .def("TotalOccurances", &WordList::TotalOccurances)
        .def("Word", &WordList::Word)
        .def("Words", &WordList::Words)
        .def("RandomWord", &WordList::RandomWord)
        .def("WordIndex", &WordList::WordIndex)
        .def("LetterOccurances", &WordList::LetterOccurances)
        .def("TotalLetterOccurances", &WordList::TotalLetterOccurances)
        .def("SetSeed", &WordList::SetSeed)
        .def("Reset", &WordList::Reset)
        .def(self + self)
        .def("WordListDict",&WordListMapDict)
        .def("SetWordListDict",&SetWordListMapDict)
        .def("__deepcopy__", &DeepCopy<WordList>)
        .def_pickle(serialization_pickle_suite<WordList>())
        .def("SaveToFile", &SaveToFile<WordList>)
        .def("LoadFromFile", &LoadFromFile<WordList>)
        .def("SubstringMatches", &WordListTreeMatches)
    ;
/********************************************************/

/***************** Polygon class ************************/
    
    class_<Polygon>("Polygon")
        .def("Translate", &Polygon::Translate)
        .def("AddVertex", &Polygon::AddVertex)
        .def("ReplaceVertex", &Polygon::ReplaceVertex)
        .def("Reset", &Polygon::Reset)
        .def("VertexList", &PolygonVertexList)
        .def("VertexList", &PolygonVertexListClosed)
        .def("LeftExtreme", &Polygon::LeftExtreme)
        .def("RightExtreme", &Polygon::RightExtreme)
        .def("TopExtreme", &Polygon::TopExtreme)
        .def("BottomExtreme", &Polygon::BottomExtreme)
        .def("IsInside", &Polygon::IsInside)
        .def("__deepcopy__", &DeepCopy<Polygon>)
        .def_pickle(serialization_pickle_suite<Polygon>())
        .def("SaveToFile", &SaveToFile<Polygon>)
        .def("LoadFromFile", &LoadFromFile<Polygon>)
    ;
/********************************************************/

/***************** Keyboard class ***********************/
    
    class_<Keyboard>("Keyboard")
        .def("AddKey", &AddKeyStr)
        .def("RemoveKey", &RemoveKeyStr)
        .def("GetKey", &GetKeyStr)
        .def("CharIndex", &GetCharIndex)
        .def("CharN", &GetCharN)
        .def("SwapCharacters", &SwapCharactersStr)
        .def("PolygonDict", &KeyboardPolygonDict)
        .def("SetPolygonDict", &SetKeyboardPolygonDict)
        .def("OrderedKeyList", &KeyboardOrderedKeyList)
        .def("Randomize", &Keyboard::Randomize)
        .def("RandomSwap", &Keyboard::RandomSwap)
        .def("RandomNoop", &Keyboard::RandomNoop)
        .def("SetSeed", &Keyboard::SetSeed)
        .def("__deepcopy__", &DeepCopy<Keyboard>)
        .def_pickle(serialization_pickle_suite<Keyboard>())
        .def("SaveToFile", &SaveToFile<Keyboard>)
        .def("LoadFromFile", &LoadFromFile<Keyboard>)
    ;
/********************************************************/

/***************** InputModel classes ***********************/

    class_<InputModelWrapper, boost::noncopyable>("InputModel")
        .def("RandomVector", pure_virtual(&InputModel::RandomVector))
        .def("PerfectVector", pure_virtual(&InputModel::PerfectVector))
        .def("Distance", pure_virtual(&InputModel::Distance))
        .def("VectorDistance", pure_virtual(&InputModel::VectorDistance))
        .def("SetSeed", &InputModel::SetSeed)
        .def("BestMatch", &InputModel::BestMatch)
    ;
    
    class_<SimpleGaussianModel, bases<InputModel> >("SimpleGaussianModel")
        .def(init<double,double,double>())
        .def(init<double,double>())
        .def("MarginalProbability", &SimpleGaussianModel::MarginalProbability)
        .def("SetXScale", &SimpleGaussianModel::SetXScale)
        .def("SetYScale", &SimpleGaussianModel::SetYScale)
        .def("SetScale", &SimpleGaussianModel::SetScale)
        .def("SetCorrelation", &SimpleGaussianModel::SetCorrelation);
//        .def("__deepcopy__", &DeepCopy<Keyboard>)
    ;

    class_<SimpleInterpolationModel, SimpleInterpolationModelCallback, bases<InputModel> >("SimpleInterpolationModel")
        .def("SetXScale", &SimpleInterpolationModel::SetXScale)
        .def("SetYScale", &SimpleInterpolationModel::SetYScale)
        .def("SetScale", &SimpleInterpolationModel::SetScale)
        .def("SetMaxDistance", &SimpleInterpolationModel::SetMaxDistance)
        .def("SetVectorLength", &SimpleInterpolationModel::SetVectorLength)
        .def("SetLoops", &SimpleInterpolationModel::SetLoops)
        .def("SetCorrelation", &SimpleInterpolationModel::SetCorrelation)
        .def("Interpolation", &SimpleInterpolationModelCallback::default_Interpolation)
        .def("SetInterpolation", &SetInterpolationByName)
    ;

#ifndef NO_FANN
    class_<NeuralNetworkModel, bases<SimpleInterpolationModel, InputModel> >("NeuralNetworkModel", init<const char*>())
        .def("CreateInputs", &NeuralNetworkInputs)
    ;
#endif
/********************************************************/

/***************** InputVector class ********************/
    
    class_<InputVector>("InputVector")
        .def("Length", &InputVector::Length)
        .def("AddPoint", &InputVector::AddPoint)
        .def("RemovePoint", &InputVector::RemovePoint)
        .def("X", &InputVector::X)
        .def("Y", &InputVector::Y)
        .def("T", &InputVector::T)
        .def("SpatialLength", &InputVector::SpatialLength)
        .def("TemporalLength", &InputVector::TemporalLength)
        .def("PointList", &InputVectorList)
        .def("DeltaPhi", &InputVector::DeltaPhi)
        .def("StringForm", &InputVector::StringForm)
        .def_pickle(serialization_pickle_suite<InputVector>())
        .def("SaveToFile", &SaveToFile<InputVector>)
        .def("LoadFromFile", &LoadFromFile<InputVector>)
    ;
/********************************************************/

/******************* RadixTree class ********************/

    class_<RadixTree>("RadixTree", init<bool>())
        .def(init<>())
        .def("AddWord", &RadixTree::AddWord)
        .def("CheckWord", &RadixTree::CheckWord)
        .def("Matches", &RadixTreeMatches)
        .def("Reset", &RadixTree::Reset)
    ;

/********************************************************/

/***************** FitnessResult class ********************/
    
    class_<FitnessResult>("FitnessResult", init<unsigned int, double, double>())
        .def(init<>())
        .def(self + self)
        .def("Fitness", &FitnessResult::Fitness)
        .def("Error", &FitnessResult::Error)
        .def("Iterations", &FitnessResult::Iterations)
        .def_pickle(serialization_pickle_suite<FitnessResult>())
        .def("SaveToFile", &SaveToFile<FitnessResult>)
        .def("LoadFromFile", &LoadFromFile<FitnessResult>)
    ;
/********************************************************/

/***************** FitnessFunctions ********************/
    def("MonteCarloEfficiency", &FitnessFunctions::MonteCarloEfficiency);
    def("ExactEfficiency", &FitnessFunctions::ExactEfficiency);
    def("FastEfficiency", &FitnessFunctions::FastEfficiency);
    def("RadixMonteCarloEfficiency", &FitnessFunctions::RadixMonteCarloEfficiency);
/********************************************************/

/***************** Interpolation ************************/
    def("SpatialInterpolation", &SpatialInterpolation);
    def("MonotonicCubicSplineInterpolation", &MonotonicCubicSplineInterpolation);
    def("HermiteCubicSplineInterpolation", &HermiteCubicSplineInterpolation);
    def("CubicSplineInterpolation", &CubicSplineInterpolation);
    def("ModCubicSplineInterpolation", &ModCubicSplineInterpolation);
    def("BezierInterpolation", &BezierInterpolation);
    def("BezierSloppyInterpolation", &BezierSloppyInterpolation);
/********************************************************/

/************** DataFormat ******************/
    class_<DataFormat>("DataFormat", init<WordList&, std::string, std::string, std::string>())
        .def(init<>())
        .def("AddEntry", &DataFormat::AddEntry)
        .def("GetEntry", &DataFormat::GetEntry)
        .def("Entries", &DataFormat::Entries)
        .def_pickle(serialization_pickle_suite<DataFormat>())
        .def("SaveToFile", &SaveToFile<DataFormat>)
        .def("PythonData", &DataFormatPythonForm)
        .def("LoadFromFile", &LoadFromFile<DataFormat>)
        .def("SetWordList", &DataFormat::SetWordList)
        .def("SetWordListName", &DataFormat::SetWordListName)
        .def("SetInputModelName", &DataFormat::SetInputModelName)
        .def("SetDistanceMeasureName", &DataFormat::SetDistanceMeasureName)
        .def("GetWordList", &DataFormat::GetWordList)
        .def("GetWordListName", &DataFormat::GetWordListName)
        .def("GetInputModelName", &DataFormat::GetInputModelName)
        .def("GetDistanceMeasureName", &DataFormat::GetDistanceMeasureName)
        .def(self+self)
    ;
/********************************************************/

/***************** Neural Network Static Functions ******/
#ifndef NO_FANN
    def("CreateNeuralNetworkInputs", &CreateNeuralNetworkInputs);
#endif
/********************************************************/
}
