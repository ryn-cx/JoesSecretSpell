import time
from typing import Any

start_time = time.time()

letters = "thisisafakentryfortesting"
double_word_location = (3, 5)
double_letter_location = (2, 3)
triple_letter_location = (5, 5)
swaps = 3

CHAR_VALUES = {
    "a": 1,
    "e": 1,
    "i": 1,
    "o": 1,
    "n": 2,
    "r": 2,
    "s": 2,
    "t": 2,
    "d": 3,
    "g": 3,
    "l": 3,
    "b": 4,
    "h": 4,
    "p": 4,
    "m": 4,
    "u": 4,
    "y": 4,
    "c": 5,
    "f": 5,
    "v": 5,
    "w": 5,
    "k": 6,
    "j": 7,
    "x": 7,
    "q": 8,
    "z": 8,
}

# Convert game state into a matrix to make it a little bit easier to work with
matrix: list[list[str]] = []


def make_matrix(letters: str) -> None:
    for i in range(0, len(letters), 5):
        matrix.append(list(letters[i : i + 5]))


word_tree = {}


def make_word_tree() -> None:
    with open("words.txt", "r") as file:
        for word in file.read().splitlines():
            current_node = word_tree
            for char in word:
                if char not in current_node:
                    current_node[char] = {}
                current_node = current_node[char]
            current_node["$"] = None


def possible_next_characters(
    row_number: int,
    col_number: int,
    used_characters: list[tuple[int, int]],
) -> list[tuple[int, int]]:
    surrounding_characters: list[tuple[int, int]] = []
    for i in range(-1, 2):
        for j in range(-1, 2):
            possible_cell = (row_number + i, col_number + j)
            if (
                0 <= possible_cell[0] < 5
                and 0 <= possible_cell[1] < 5
                and (i, j) != (0, 0)
            ):
                if possible_cell not in used_characters:
                    surrounding_characters.append((row_number + i, col_number + j))
    return surrounding_characters


def word_getter(
    row_number: int,
    col_number: int,
    current_word: str,
    current_path: list[tuple[int, int]],
    valid_words: list[tuple[int, str, int, list[tuple[int, int]]]],
    swaps: int,
) -> list[tuple[int, str, int, list[tuple[int, int]]]]:
    next_char_options = possible_next_characters(row_number, col_number, current_path)
    for i, j in next_char_options:
        new_word = current_word + matrix[i][j]
        new_path = current_path.copy() + [(i, j)]
        if partial_word(new_word):
            word_getter(i, j, new_word, new_path, valid_words, swaps)
            if complete_word(new_word):
                valid_words.append((swaps, new_word, word_score(new_path), new_path))
        elif swaps > 0:
            for letter in partial_word(current_word).keys():
                if letter != "$":
                    new_word = current_word + letter
                    if partial_word(new_word):
                        word_getter(i, j, new_word, new_path, valid_words, swaps - 1)
                        if complete_word(new_word):
                            valid_words.append(
                                (swaps, new_word, word_score(new_path), new_path)
                            )

    return valid_words


def word_score(word_path: list[tuple[int, int]]) -> int:
    score = 0
    for row_number, col_number in word_path:
        if (row_number, col_number) == triple_letter_location:
            score += CHAR_VALUES[matrix[row_number][col_number]] * 3
        elif (row_number, col_number) == double_letter_location:
            score += CHAR_VALUES[matrix[row_number][col_number]] * 2
        else:
            score += CHAR_VALUES[matrix[row_number][col_number]]

    if double_word_location in word_path:
        score *= 2

    return score


def partial_word(word: str) -> dict[str, dict[str, Any]]:
    current_tree = word_tree
    for char in word:
        if char not in current_tree:
            return {}
        current_tree = current_tree[char]
    return current_tree


def complete_word(word: str) -> bool:
    return "$" in partial_word(word)


if __name__ == "__main__":
    make_matrix(letters)
    make_word_tree()
    # Start at each character in the matrix and find the best word
    all_words: list[tuple[int, str, int, list[tuple[int, int]]]] = []
    for row_number, row in enumerate(matrix):
        for col_number, character in enumerate(row):
            all_words += word_getter(
                row_number,
                col_number,
                character,
                [(row_number, col_number)],
                [],
                swaps,
            )

    sorted = all_words.sort(key=lambda x: x[2], reverse=True)
    printed_words: set[str] = set()
    output: list[tuple[int, str, int, list[tuple[int, int]]]] = []
    for x in all_words:
        id = f"{x[0]}{x[1]}"
        if id not in printed_words:
            output.append(x)
            printed_words.add(id)

    for x in reversed(output):
        print(x)

    end_time = time.time()
    elapsed_time = end_time - start_time

    print(f"Elapsed time: {elapsed_time} seconds")
