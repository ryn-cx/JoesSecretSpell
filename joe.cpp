#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include "joe.h"
#include <chrono>

std::string letters = "thisisafakentryfortesting";
std::pair<int, int> double_word_location = {5, 5};
std::pair<int, int> double_letter_location = {2, 3};
std::pair<int, int> triple_letter_location = {5, 5};
int swaps = 1;

// This is a real constant
std::map<char, int> CHAR_VALUES = {
    {'a', 1},
    {'e', 1},
    {'i', 1},
    {'o', 1},
    {'n', 2},
    {'r', 2},
    {'s', 2},
    {'t', 2},
    {'d', 3},
    {'g', 3},
    {'l', 3},
    {'b', 4},
    {'h', 4},
    {'p', 4},
    {'m', 4},
    {'u', 4},
    {'y', 4},
    {'c', 5},
    {'f', 5},
    {'v', 5},
    {'w', 5},
    {'k', 6},
    {'j', 7},
    {'x', 7},
    {'q', 8},
    {'z', 8},
};

// Convert game state into a matrix to make it a little bit easier to work with
std::vector<std::vector<char>> matrix;
void make_matrix()
{
    matrix.reserve(letters.size() / 5);
    for (int i = 0; i < letters.size(); i += 5)
    {
        std::vector<char> row;
        for (int j = 0; j < 5; ++j)
        {
            row.emplace_back(letters[i + j]);
        }
        matrix.emplace_back(row);
    }
}

auto word_tree = new TrieNode();
void make_word_tree()
{
    std::ifstream file("words.txt");
    std::string word;
    while (std::getline(file, word))
    {
        TrieNode *currentNode = word_tree;
        for (char c : word)
        {
            // Skip end of line symbol
            if (c == '\n' || c == '\r')
                continue;

            if (currentNode->children.count(c) == 0)
            {
                currentNode->children[c] = new TrieNode();
            }
            currentNode = currentNode->children[c];
        }
        currentNode->isEndOfWord = true;
    }
    file.close();
}

std::vector<std::pair<int, int>> possible_next_characters(int row_number, int col_number, std::vector<std::pair<int, int>> &used_characters)
{
    std::vector<std::pair<int, int>> surrounding_characters;
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            std::pair<int, int> possible_cell = {row_number + i, col_number + j};
            if (possible_cell.first >= 0 && possible_cell.first < 5 &&   // Must be in x coordinates
                possible_cell.second >= 0 && possible_cell.second < 5 && // Must be in y coordinates
                !(i == 0 && j == 0))                                     // Can't use the same character twice

            {
                if (std::find(used_characters.begin(), used_characters.end(), possible_cell) == used_characters.end())
                {
                    surrounding_characters.emplace_back(possible_cell);
                }
            }
        }
    }
    return surrounding_characters;
}

std::vector<std::tuple<int, std::string, int, std::vector<std::pair<int, int>>>> word_getter(
    std::vector<std::vector<char>> &matrix,
    int row_number,
    int col_number,
    std::string current_word,
    std::vector<std::pair<int, int>> current_path,
    std::vector<std::tuple<int, std::string, int, std::vector<std::pair<int, int>>>> &valid_words,
    int swaps)
{
    auto next_char_options = possible_next_characters(row_number, col_number, current_path);
    for (auto [i, j] : next_char_options)
    {
        auto new_word = current_word + matrix[i][j];
        auto new_path = current_path;
        new_path.push_back({i, j});
        if (partial_word(new_word))
        {
            word_getter(matrix, i, j, new_word, new_path, valid_words, swaps);
            if (complete_word(new_word))
            {
                valid_words.push_back({swaps, new_word, word_score(new_path), new_path});
            }
        }
        else if (swaps > 0)
        {
            for (const auto &[letter, node] : partial_word(current_word)->children)
            {
                new_word = current_word + letter;
                if (partial_word(new_word))
                {
                    word_getter(matrix, i, j, new_word, new_path, valid_words, swaps - 1);
                    if (complete_word(new_word))
                    {
                        valid_words.push_back({swaps, new_word, word_score(new_path), new_path});
                    }
                }
            }
        }
    }
    return valid_words;
}

int word_score(const std::vector<std::pair<int, int>> &word_path)
{
    int score = 0;
    for (const auto &[row_number, col_number] : word_path)
    {
        if (std::make_pair(row_number, col_number) == triple_letter_location)
        {
            score += CHAR_VALUES[matrix[row_number][col_number]] * 3;
        }
        else if (std::make_pair(row_number, col_number) == double_letter_location)
        {
            score += CHAR_VALUES[matrix[row_number][col_number]] * 2;
        }
        else
        {
            score += CHAR_VALUES[matrix[row_number][col_number]];
        }
    }

    if (std::find(word_path.begin(), word_path.end(), double_word_location) != word_path.end())
    {
        score *= 2;
    }

    return score;
}

std::unordered_map<std::string, TrieNode *> partial_word_cache;

TrieNode *partial_word(const std::string &word)
{
    if (partial_word_cache.count(word))
    {
        return partial_word_cache[word];
    }

    TrieNode *currentNode = word_tree;
    for (char c : word)
    {
        if (currentNode->children.count(c) == 0)
        {
            partial_word_cache[word] = nullptr;
            return nullptr;
        }
        currentNode = currentNode->children[c];
    }

    partial_word_cache[word] = currentNode;
    return currentNode;
}

bool complete_word(const std::string &word)
{
    auto node = partial_word(word);
    return node != nullptr && node->isEndOfWord;
}

int main()
{

    auto start = std::chrono::high_resolution_clock::now();
    make_matrix();
    make_word_tree();
    // get_game_state_hardcoded();

    std::vector<std::tuple<int, std::string, int, std::vector<std::pair<int, int>>>>
        all_words;
    all_words.reserve(10000);

    for (int row_number = 0; row_number < 5; ++row_number)
    {
        for (int col_number = 0; col_number < 5; ++col_number)
        {
            std::string character(1, matrix[row_number][col_number]);
            // This function call kills performance
            word_getter(matrix, row_number, col_number, character, {{row_number, col_number}}, all_words, swaps);
        }
    }

    std::sort(all_words.begin(), all_words.end(), [](const auto &a, const auto &b)
              { return std::get<2>(a) < std::get<2>(b); });

    std::set<std::string> printed_words;
    std::vector<std::tuple<int, std::string, int, std::vector<std::pair<int, int>>>> output;
    for (const auto &x : all_words)
    {
        std::string id = std::to_string(std::get<0>(x)) + std::get<1>(x);
        if (printed_words.find(id) == printed_words.end())
        {
            output.push_back(x);
            printed_words.insert(id);
        }
    }

    for (auto it = output.begin(); it != output.end(); ++it)
    {
        std::cout << '(' << std::get<0>(*it) << ", " << std::get<1>(*it) << ", " << std::get<2>(*it) << ", [";
        for (const auto &pair : std::get<3>(*it))
        {
            std::cout << '(' << pair.first << ", " << pair.second << "), ";
        }
        std::cout << "])\n";
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Elapsed time: " << elapsed.count() << " seconds\n";
    return 0;
}

void get_game_state()
{
    std::string letters;
    do
    {
        std::cout << "Please enter 25 letters: ";
        std::getline(std::cin, letters);
    } while (letters.length() != 25);

    // Convert game state into a matrix to make it a little bit easier to work with
    for (int i = 0; i < letters.size(); i += 5)
    {
        std::vector<char> row;
        for (int j = 0; j < 5; ++j)
        {
            row.emplace_back(letters[i + j]);
        }
        matrix.emplace_back(row);
    }

    int row, col, input;
    std::cout << "Input number of swaps: ";
    std::cin >> input;
    swaps = input;

    std::cout << "Input letter multiplier: ";
    std::cin >> input;

    std::cout << "Enter the row number for the LETTER multiplier: ";
    std::cin >> row;
    std::cout << "Enter the column number for the LETTER multiplier: ";
    std::cin >> col;

    triple_letter_location = {row, col};

    if (input == 2)
    {
        double_letter_location = {row, col};
    }
    else if (input == 3)
    {
        triple_letter_location = {row, col};
    }
}
