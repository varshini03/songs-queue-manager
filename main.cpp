#include <iostream>
#include <fstream>
#include <numeric>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <utility>
#include <algorithm>
#include <cstring>
#include "templates.cpp"

using namespace JsonParser;

struct Song
{
    std::string title;
    std::string artist;
    std::string album;
    int id;
};

class Node
{
public:
    Song song;
    Node *next;

    Node(const Song &s) : song(s), next(nullptr) {}
};

// Comparator function
bool compareSongs(const std::vector<std::pair<std::string, JsonValue>> &a, const std::vector<std::pair<std::string, JsonValue>> &b)
{
    std::string titleA, titleB;
    for (const auto &pair : a)
    {
        if (pair.first == "title")
        {
            titleA = *(pair.second.s);
        }
    }
    for (const auto &pair : b)
    {
        if (pair.first == "title")
        {
            titleB = *(pair.second.s);
        }
    }

    return std::strcmp(titleA.c_str(), titleB.c_str()) < 0;
}

// Binary search function using strcmp
int binarySearch(const std::vector<std::vector<std::pair<std::string, JsonValue>>> &songsList, const std::string &target)
{
    int left = 0;
    int right = songsList.size() - 1;

    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        std::string midTitle;
        for (const auto &pair : songsList[mid])
        {
            if (pair.first == "title")
            {
                midTitle = *(pair.second.s);
                break;
            }
        }

        int cmp = std::strcmp(midTitle.c_str(), target.c_str());
        if (cmp == 0)
        {
            return mid; // Found
        }
        else if (cmp < 0)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }

    return -1; // Not found
}

void appendToPlaylist(Node *head, Node *&tail, const std::string songToFind, auto &songsList)
{
    int index = binarySearch(songsList, songToFind);

    if (index != -1)
    {
        // Print the details of the song found
        std::cout << "Found song details!" << std::endl;
        Song song;
        for (const auto &pair : songsList[index])
        {
            if (pair.first == "id")
            {
                song.id = pair.second.i;
            }
            else if (pair.first == "title")
            {
                song.title = *(pair.second.s);
            }
            else if (pair.first == "artist")
            {
                song.artist = *(pair.second.s);
            }
            else if (pair.first == "album")
            {
                song.album = *(pair.second.s);
            }
        }
        // Create a new node for the song
        Node *newNode = new Node(song);

        tail->next = newNode;
        tail = newNode;
    }
    else
    {
        std::cout << "Song not found" << std::endl;
    }
}

std::vector<int> randomIndices(int size = 50)
{
    std::vector<int> indices(size);
    std::iota(indices.begin(), indices.end(), 0);
    std::random_shuffle(indices.begin(), indices.end());
    indices.resize(10);
    return indices;
}

Node *createPlaylist(const std::vector<std::vector<std::pair<std::string, JsonValue>>> &v, const std::vector<int> &indices, Node *&tail)
{
    Node *head = nullptr;

    // Iterate through the indices to build the linked list
    for (int index : indices)
    {
        // Extract the song information from the vector entry
        const auto &entry = v[index];
        Song song;
        for (const auto &pair : entry)
        {
            if (pair.first == "id")
            {
                song.id = pair.second.i;
            }
            else if (pair.first == "title")
            {
                song.title = *(pair.second.s);
            }
            else if (pair.first == "artist")
            {
                song.artist = *(pair.second.s);
            }
            else if (pair.first == "album")
            {
                song.album = *(pair.second.s);
            }
        }

        if (head == nullptr)
        {
            head = new Node(song);
            tail = head;
        }
        else
        {
            tail->next = new Node(song);
            tail = tail->next;
        }
    }

    return head;
}

void printPlaylist(Node *head)
{
    Node *current = head;
    while (current)
    {
        std::cout << "Song: " << current->song.title
                  << ", Artist: " << current->song.artist
                  << ", Album: " << current->song.album
                  << ", ID: " << current->song.id << std::endl;
        current = current->next;
    }
}

// shuffling
// Function to split the list into two halves
Node *splitList(Node *source)
{
    if (source == nullptr || source->next == nullptr)
        return nullptr; // Return nullptr if the list is empty or has only one node

    Node *slow = source;
    Node *fast = source->next;

    // Use slow and fast pointers to find the middle of the list
    while (fast != nullptr && fast->next != nullptr)
    {
        fast = fast->next->next;
        slow = slow->next;
    }

    // Split the list into two halves
    Node *back = slow->next;
    slow->next = nullptr;

    return back;
}

// Function to merge two lists using random choice (Fisher-Yates inspired)
Node *merge(Node *a, Node *b)
{
    // Base cases
    if (a == nullptr)
        return b;
    if (b == nullptr)
        return a;

    Node *result = nullptr;
    Node *tail = nullptr;

    // Initialize result
    if (rand() % 2 == 0)
    {
        result = a;
        a = a->next;
    }
    else
    {
        result = b;
        b = b->next;
    }
    tail = result;

    // Merge the two lists
    while (a != nullptr && b != nullptr)
    {
        if (rand() % 2 == 0)
        {
            tail->next = a;
            a = a->next;
        }
        else
        {
            tail->next = b;
            b = b->next;
        }
        tail = tail->next;
    }

    // Append the remaining nodes
    tail->next = (a != nullptr) ? a : b;

    return result;
}

// Merge sort function for linked list
Node *mergeSort(Node *head)
{
    if (head == nullptr || head->next == nullptr)
        return head; // Base case: list is empty or has only one node

    Node *back = splitList(head);
    Node *front = head;

    front = mergeSort(front); // Recursively sort the first half
    back = mergeSort(back);   // Recursively sort the second half

    return merge(front, back); // Merge sorted halves
}

int main()
{
    std::string filepath = "F:\\larger the size steeper the curve\\cpp\\MusicPlayer\\songs.json";
    std::vector<std::vector<std::pair<std::string, JsonValue>>> songsList;

    songsList = ParseJson(filepath);
    // Sort the songs list based on song title
    std::sort(songsList.begin(), songsList.end(), compareSongs);

    /*
    // Print the sorted list
    for (const auto &entry : songsList)
    {
        for (const auto &pair : entry)
        {
            union JsonValue u = pair.second;
            std::cout << pair.first << " ";
            if (pair.first == "id")
            {
                std::cout << u.i << std::endl;
            }
            else
            {
                std::cout << *(u.s) << std::endl;
            }
        }
    }
    std::cout << std::endl;
    */

    // generate random indices and create playlist
    std::cout << "creating playlist..." << std::endl;
    srand(time(0));
    std::vector<int> indices = randomIndices();

    Node *tail = nullptr;
    Node *head = createPlaylist(songsList, indices, tail);
    printPlaylist(head);
    std::cout << std::endl;

    std::cout << "adding a song..." << std::endl;
    // add a song
    std::string songToFind = "Roar";
    appendToPlaylist(head, tail, songToFind, songsList);
    songToFind = "Havana";
    appendToPlaylist(head, tail, songToFind, songsList);
    printPlaylist(head);
    std::cout << std::endl;

    std::cout << "shuffling the playlist..." << std::endl;
    // shuffle
    head = mergeSort(head);
    printPlaylist(head);
    std::cout << std::endl;
    return 0;
}
