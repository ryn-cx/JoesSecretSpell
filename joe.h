// go.h

#ifndef GO_H
#define GO_H

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

struct TrieNode
{
    std::unordered_map<char, TrieNode *> children;
    bool isEndOfWord = false;
};

std::vector<std::pair<int, int>> possible_next_characters(int row_number, int col_number, std::vector<std::pair<int, int>> &used_characters);

void insertWord(TrieNode *root, const std::string &word);

TrieNode *partial_word(const std::string &word);

bool complete_word(const std::string &word);

void get_game_state();

int word_score(const std::vector<std::pair<int, int>> &word_path);
void get_game_state_hardcoded();

#endif // GO_H