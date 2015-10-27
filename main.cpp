#include <iostream>
#include <set>
#include <map>
#include <unordered_map>
#include <math.h>
#include <vector>

using namespace std;

set<int> get_divisors(const int&);
map<int, int> get_pythagorean_pairs(const int&);
vector< unordered_map<int, int>> solve(const int&, const set<int>&);
vector< unordered_map<int, int>> solve_recursive(const int& target, const set<int>& candidates,
                                                         const int& sum, unordered_map<int, int> used_candidates,
                                                         const int& current_candidate);
int find_smallest_factor(const int& target, const set<int>& candidates);
pair<bool, int> adds_up(const vector< set<int>>& candidate_sets, const int& target, const set<int>& key_zero_candidates);
pair<bool, int> adds_up_recursive(const vector< set<int>>& candidate_sets, const int& target, const set<int>& key_zero_candidates,
                    const int& index, const int& passed_value);

int main() {
    int n;
    
    cin >> n;
    if (!cin || n < 1 || n > 100)
        return -1;
    
    for (int run=0; run < n; ++run)
    {
        int m, S;
        cin >> m >> S;
        
        if (!cin || m < 1 || m > 40 || S < 1 || S > 300)
            return -1;
        
        int conventional_value, infoTechnological_value;
        unordered_map<int, set<int>> ecoins_indexes[2];
        set<int> separate_values[2]; // Store conventional and infoTechnological values separately
        
        for (int i=0; i<m; ++i)
        {
            cin >> conventional_value >> infoTechnological_value;
            
            if (!cin || conventional_value < 0 || conventional_value > S || infoTechnological_value < 0 || infoTechnological_value > S)
                return -1;
            
            if (conventional_value > 0)
                separate_values[0].insert(conventional_value);
            if (infoTechnological_value > 0)
                separate_values[1].insert(infoTechnological_value);
            
            auto ecoins_it = ecoins_indexes[0].insert(make_pair(conventional_value, set<int>{infoTechnological_value}));
            if (!ecoins_it.second)
                ecoins_it.first->second.insert(infoTechnological_value);
            auto ecoins_index_it = ecoins_indexes[1].insert(make_pair(infoTechnological_value, set<int>{conventional_value}));
            if (!ecoins_index_it.second)
                ecoins_index_it.first->second.insert(conventional_value);
        }
        
        map<int, int> pythagorean_pairs = get_pythagorean_pairs(S); // Will contain possible values of X and Y to achieve e-modulus = S
        
        int min_ecoins = -1;
        int target_sums[2];
        for (auto entry : pythagorean_pairs)
        {
            target_sums[0] = entry.first;
            target_sums[1] = entry.second;
            
            for (int j=0; j<2; ++j)
            {
                int target = target_sums[j];
                for (int i=0; i<2; ++i)
                {
                    if (target == 0)
                        break;
                    
                    auto candidates_of_target = solve(target, separate_values[i]);
                    if (candidates_of_target.size() != 0)
                    {
                        for (auto candidates_it = candidates_of_target.rbegin();
                             candidates_it != candidates_of_target.rend(); ++candidates_it) // Backwards for optimization
                        {
                            int total_factor = 0;
                            vector< set<int>> set_of_sums;
                            bool match_found = false;
                            for (auto candidate : *candidates_it)
                            {
                                int factor = candidate.second;
                                total_factor += factor;
                                
                                set<int> sums;
                                auto it_ecoin_key_values = ecoins_indexes[i].find(candidate.first);
                                for (auto value : it_ecoin_key_values->second)
                                    sums.insert(value*factor);
                                
                                set_of_sums.push_back(sums);
                                
                                auto key_zero_candidates = ecoins_indexes[i].find(0)->second;
                                auto success_indicator = adds_up(set_of_sums, target_sums[(j-1)*-1], key_zero_candidates);
                                
                                if (success_indicator.first)
                                {
                                    match_found = true;
                                    total_factor += success_indicator.second;
                                }
                                
                            }
                            
                            if (match_found && (min_ecoins < 0 || total_factor < min_ecoins))
                                min_ecoins = total_factor;
                        }
                    }
                }
            }
            
        }
        
        if (min_ecoins > 0)
            cout << min_ecoins << endl;
        else
            cout << "not possible" << endl;
    }
    
    return 0;
}

map<int, int> get_pythagorean_pairs(const int& S)
{
    map<int, int> pythagorean_pairs;
    
    set<int> divisors = get_divisors(S);
    for (int k : divisors)
    {
        for (int m=0; pow(m, 2) <= S/k; ++m)
        {
            int n = sqrt((S/k - pow(m,2)));
            if (m>n)
            {
                int a = k*(pow(m,2)-pow(n,2));
                int b = 2*k*m*n;
                if ( pow(a, 2) + pow(b, 2) == pow(S, 2) )
                {
                    pythagorean_pairs.insert(make_pair(min(a, b), max(a, b)));
                }
            }
        }
    }
    
    return pythagorean_pairs;
}

pair<bool, int> adds_up(const vector< set<int>>& candidate_sets, const int& target, const set<int>& key_zero_candidates)
{
    return adds_up_recursive(candidate_sets, target, key_zero_candidates, 0, 0);
}

pair<bool, int> adds_up_recursive(const vector< set<int>>& candidate_sets, const int& target, const set<int>& key_zero_candidates,
                    const int& index, const int& passed_value)
{
    int extra_factor = 0;
    if (index == candidate_sets.size())
        return make_pair(false, 0);
    
    bool match_found = false;
    for (auto value : candidate_sets.at(index))
    {
        if (passed_value + value > target)
            return make_pair(false, 0);
        else if (index == candidate_sets.size()-1)
        {
            if (passed_value + value == target)
                return make_pair(true, 0);
            
            int smallest_factor = find_smallest_factor(target-(passed_value + value), key_zero_candidates);
            if (smallest_factor != 0)
            {
                match_found = true;
                if (extra_factor == 0 || smallest_factor < extra_factor)
                    extra_factor = smallest_factor;
            }
        }
        else
        {
            auto asd = adds_up_recursive(candidate_sets, target, key_zero_candidates, index+1, value);
            if (asd.first)
                return asd;
        }
    }
    
    if (match_found)
        return make_pair(true, extra_factor);
    return make_pair(false, 0);
}

int find_smallest_factor(const int& target, const set<int>& candidates)
{
    int factor = false;
    
    auto candidates_rit = candidates.rbegin();
    for (auto candidate_rit = candidates_rit; candidate_rit != candidates.rend(); ++candidate_rit)
    {
        int temp = target % *candidate_rit;
        if (temp == 0)
            return target / *candidate_rit;
    }
    
    return factor;
}

set<int> get_divisors(const int& number)
{
    set<int> divisors;
    
    for (int i=1; i<=sqrt(number); ++i)
    {
        if (number%i == 0)
            divisors.insert(i);
    }
    
    for (int divisor : divisors)
    {
        divisors.insert(number/divisor);
    }
    
    return divisors;
}

vector< unordered_map<int, int>> solve(const int& target, const set<int>& candidates)
{
    return solve_recursive(target, candidates, 0, {}, 0);
}

vector< unordered_map<int, int>> solve_recursive(const int& target, const set<int>& candidates,
                                                                   const int& sum, unordered_map<int, int> used_candidates,
                                                                   const int& current_candidate)
{
    set<int>::iterator it_begin;
    if (current_candidate)
        it_begin = candidates.find(current_candidate);
    else
        it_begin = candidates.begin();
    
    
    vector< unordered_map<int, int>> combinations;
    
    for (auto it = it_begin; it != candidates.end(); ++it)
    {
        int new_sum = sum+*it;
        if (new_sum <= target)
        {
            auto index = used_candidates.insert(make_pair(*it, 1));
            if (!index.second) // If key did already exist
                ++index.first->second; // Increase value by 1
            
            if (new_sum == target)
                combinations.push_back(used_candidates);
            else if (new_sum < target)
            {
                auto temp_combinations = solve_recursive(target, candidates, sum+(*it), used_candidates, *it);
                for (auto combination : temp_combinations)
                    combinations.push_back(combination);
                auto key_value_pair = used_candidates.find(*it);
                --key_value_pair->second;
                if (key_value_pair->second == 0)
                    used_candidates.erase(key_value_pair);
            }
        }
        else
            break;
    }
    
    return combinations;
}

