
#include <utility>
#include <set>
#include <iostream>
#include <sstream>

using Grouping = std::size_t;
using Combination = std::multiset<Grouping>;
using Solution = std::set<Combination>;

using NWhite = std::size_t;
using NBlack = std::size_t;
using MultiGrouping = std::pair<NWhite, NBlack>;
using MultiCombination = std::multiset<MultiGrouping>;
using MultiSolution = std::set<MultiCombination>;

bool operator<(const MultiGrouping& lhs_, const MultiGrouping& rhs_)
{
  return (lhs_.first == rhs_.first) ?
    (lhs_.second < rhs_.second) : (lhs_.first < rhs_.first);
}

void printSolution(const Solution& solution_)
{
  std::cout << "Solution size: " << solution_.size() << std::endl;
  std::cout << "Combinations: " << std::endl;

  for(const auto& combination : solution_)
    {
      std::stringstream ss;
      for(const auto& grouping : combination)
        {
          for(std::size_t ii = 0; ii < grouping; ++ii)
            {
              ss << "O";
            }
          ss << ",";
        }

      // Print a combination and remove the trailing comma.
      const auto& line = ss.str();
      std::cout << line.substr(0, line.size() - 1) << std::endl;
    }
}

void printMultiSolution(const MultiSolution& solution_)
{
  std::cout << "Solution size: " << solution_.size() << std::endl;
  std::cout << "Combinations: " << std::endl;

  for(const auto& combination : solution_)
    {
      std::stringstream ss;
      for(const auto& grouping : combination)
        {
          for(std::size_t ii = 0; ii < grouping.first; ++ii)
            {
              ss << "W";
            }
          for(std::size_t ii = 0; ii < grouping.second; ++ii)
            {
              ss << "B";
            }
          ss << ",";
        }

      // Print a combination and remove the trailing comma.
      const auto& line = ss.str();
      std::cout << line.substr(0, line.size() - 1) << std::endl;
    }
}

MultiSolution expandMultiCombination(const MultiCombination& combination_, bool isWhite_)
{
  MultiSolution result;

  // Simple case: Add the additional sphere as it's own grouping.
  MultiCombination simpleCombination = combination_;
  simpleCombination.insert(isWhite_ ? std::make_pair(1,0) : std::make_pair(0,1));
  result.insert(simpleCombination);

  // Group case: Add the adiitional sphere to each unique grouping in the combination.
  MultiGrouping justProcessed{std::pair<NWhite, NBlack>(0,0)};
  for(auto it = combination_.begin(); it != combination_.end(); ++it)
    {
      MultiGrouping currGrouping = *it;

      // Ignore a grouping size we have already processed
      if(currGrouping == justProcessed)
        {
          continue;
        }

      justProcessed = currGrouping;

      // TODO: Improve naive combination using insertion hint.
      MultiCombination groupingCombination = combination_;
      MultiGrouping groupingToAdd = currGrouping;
      if(isWhite_)
        {
          ++groupingToAdd.first;
        }
      else
        {
          ++groupingToAdd.second;
        }

      auto hint = groupingCombination.erase(groupingCombination.find(currGrouping));
      groupingCombination.insert(hint, groupingToAdd);
      result.insert (groupingCombination);
    }

  return result;
}

Solution expandCombination(const Combination& combination_)
{
  Solution result;

  // Simple case: Add the additional sphere as it's own grouping.
  Combination simpleCombination = combination_;
  simpleCombination.insert(1);
  result.insert(simpleCombination);

  // Group case: Add the adiitional sphere to each unique grouping in the combination.
  Grouping justProcessed{0};
  for(auto it = combination_.begin(); it != combination_.end(); ++it)
    {
      Grouping currGrouping = *it;

      // Ignore a grouping size we have already processed
      if(currGrouping == justProcessed)
        {
          continue;
        }

      justProcessed = currGrouping;

      // TODO: Improve naive combination using insertion hint.
      Combination groupingCombination = combination_;
      auto hint = groupingCombination.erase(groupingCombination.find(currGrouping));
      Grouping groupingToAdd = currGrouping + 1;
      groupingCombination.insert(hint, groupingToAdd);
      result.insert (groupingCombination);
    }

  return result;
}

Solution solution(std::size_t numSpheres_)
{
  Solution currSolution;
  if(numSpheres_ == 0)
    {
      return currSolution;
    }

  if(numSpheres_ == 1)
    {
      Combination baseSolution = {1};
      currSolution.insert(baseSolution);
      return currSolution;
    }

  auto prevSolution = solution(numSpheres_ - 1);

  for(const auto& combination : prevSolution)
    {
      const auto& expandedCombinations = expandCombination(combination);
      currSolution.insert(expandedCombinations.begin(), expandedCombinations.end());
    }

  return currSolution;
}

MultiSolution multiFromSingle(const Solution& solution_, bool isWhite_)
{
  MultiSolution result;
  for(const auto& combination : solution_)
    {
      MultiCombination multiComb;
      for(const auto& grouping : combination)
        {
          multiComb.insert(isWhite_ ? std::pair<NWhite, NBlack>(grouping, 0) : std::pair<NWhite, NBlack>(0, grouping));
        }

      result.insert(multiComb);
    }
  return result;
}

// TODO: Symmetry between white and black should be exploited and memoized G(w,b) = G(b,w)
MultiSolution solution(std::size_t numWhite_, std::size_t numBlack_)
{
  MultiSolution result;

   if(numBlack_ == 0)
     {
       return multiFromSingle(solution(numWhite_), true);
       return result;
     }

  if(numWhite_ == 0)
    {
      return multiFromSingle(solution(numBlack_), false);
    }

  MultiSolution oneFewerWhiteSolution = solution(numWhite_ - 1, numBlack_);
  for(const auto& combination : oneFewerWhiteSolution)
    {
      const auto& expandedCombinations = expandMultiCombination(combination, true);
      result.insert(expandedCombinations.begin(), expandedCombinations.end());
    }

  return result;
}

int main(int argc_, char** argv_)
{
  if(argc_ == 2)
    {
      printSolution(solution(atoi(argv_[1])));
    }
  else if(argc_ == 3)
    {
      printMultiSolution(solution(atoi(argv_[1]), atoi(argv_[2])));
    }
  return 0;
}

