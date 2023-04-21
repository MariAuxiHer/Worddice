/********************************************************************************************************************************************************
    Maria Hernandez 
    COSC302
    04/20/2023
    Project5
    worddice.cpp
    This program converts a bipartite graph containing in one set the dice and in the other set the letters of a given word into a network flow by adding
    source and sink. Then, uses the Edmonds Karp's algorithm to find the maximum flow. Finding the maximum flow will discover the matching if it exists.
    Then, it outputs whether we can spell or not the given word with the provided dice. 
***********************************************************************************************************************************************************/

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <queue>

using namespace std; 

// Function that prints whether there is a matching or not. 
void print_results(vector<map<int, int> > &residual_graph, vector<int> &vertices_to_sink, string &word) {
    map<int, int>::iterator adjacent_edges_it; 
    map<int, int>::iterator backtrack_it; 
    bool can_spell = true; 

    // Checking whether there is an edge from the sink to each letter of the word.
    for (size_t i = 0; i < vertices_to_sink.size(); i++) {
        adjacent_edges_it = residual_graph.at(residual_graph.size()-1).find(vertices_to_sink.at(i));

        // If there is not an edge from the sink to some of the letters of the word is because we can't spell the word 
        // so we change the flag to false.
        if (adjacent_edges_it == residual_graph.at(residual_graph.size()-1).end()) {
            can_spell = false;
            break; 
        }
    }
    vector<int> letter_from_dice_number; 

    // If we can spell the word
    if (can_spell) {
        int value; 

        // We look for all the paths from sink to source to find from which dice number we are getting each letter. 
        for (adjacent_edges_it = residual_graph.at(residual_graph.size()-1).begin(); adjacent_edges_it != residual_graph.at(residual_graph.size()-1).end(); adjacent_edges_it++) {
            backtrack_it = adjacent_edges_it;

            // We break out of this loop when the incident vertex is different to 0, and we store the value of the vertex connected to 0 in the 
            // 'value' variable.
            while (true) {
                backtrack_it = residual_graph.at(backtrack_it->first).begin();
                if (backtrack_it->first == 0) break;
                value = backtrack_it->first;
            }

            // We subtract one since we added an extra vertex (source) and then store this value on the vector 'letter_from_dice_number'
            value -= 1;
            letter_from_dice_number.push_back(value);
        }

        // Then, we print from which dice number we got each letter and then the word. 
        for (size_t i = 0; i < (letter_from_dice_number.size()-1); i++) {
            cout << letter_from_dice_number.at(i) << ",";
        }
        cout << letter_from_dice_number.at(letter_from_dice_number.size()-1) << ": " << word << endl;
    }

    // Otherwise, we output that we cannot print the message. 
    else { 
        cout << "Cannot spell " << word << endl;
    }
}

// bfs() function that finds an augmenting path from source to sink.
bool bfs(vector<map<int, int> > &residual_graph, map<int, int> &visited, int &src, int &dest) {

    /*
        Declare the vertices_queue variable where we will push all vertices in a graph that have not yet been visited. 
        Then, we push the source vertex and mark it as visited (we add the vertex and from where we visited (source in this case)).
    */
    queue<int> vertices_queue; 
    vertices_queue.push(src); 
    visited.insert(make_pair(src, src));
    map<int, int>::iterator visited_it; 
    map<int, int>::iterator residual_graph_it;
    int new_front; 

    // This while-loop will loop as long as the queue is not empty.
    while (!vertices_queue.empty()) {

        // First, store the first element of the queue into the new_front variable and pop it off the queue.
        new_front = vertices_queue.front();
        vertices_queue.pop();

        // Loop through the vertices adjacent to new_front. 
        for (residual_graph_it = residual_graph.at(new_front).begin(); residual_graph_it != residual_graph.at(new_front).end(); residual_graph_it++) {
                
            // If the vertex has not yet been visited, we add it to the queue and insert it into the visited map.
            visited_it = visited.find(residual_graph_it->first);
            if (visited_it == visited.end()) {
                visited.insert(make_pair(residual_graph_it->first, new_front));
                vertices_queue.push(residual_graph_it->first); 
            }

            // We return if we find the sink.
            if (residual_graph_it->first == dest) return true; 
        }
    }
    return false; 
}

// Function that implements the Edmonds Karp's algorithm to find the maximum flow.
void edmonds_karp(vector<map<int, int> > residual_graph, vector<int> &vertices_to_sink, string &word) {
        
    /*
        Declaring and initializing the variables we will pass to the bfs function which finds an augmenting path from source to sink, 
        and the ones we use to store the retrived path from source to sink. The visited map is a set of pairs that will contain each 
        vertex visited and from which vertex it was visited (dest src) so that we can retrieve the path from here.
    */
    int src = 0; 
    int dest = residual_graph.size()-1;
    vector<int> backward_path; 
    map<int, int> visited; 
    map<int, int>::iterator adjacent_edges_it; 
    map<int, int>::iterator visited_it; 
    map<int, int>::iterator residual_graph_it;

    // The following while-loop will loop as long there is an augmenting path between source and sink.
    while (bfs(residual_graph, visited, src, dest)) {

        // Path recovery 
        visited_it = visited.find(dest); 
        backward_path.push_back(dest);
        while (visited_it->first != 0) {
            backward_path.push_back(visited_it->second);
            visited_it = visited.find(visited_it->second);
        }

        /* 
            Erase/add links in the residual graph:
            First, remove forward links from our network flow since now their capacity is 0. 
        */
        for (int i = backward_path.size()-1; i > 0; i--) {
            residual_graph_it = residual_graph.at(backward_path.at(i)).find(backward_path.at(i-1));
            residual_graph.at(backward_path.at(i)).erase(residual_graph_it);
        }

        // Add backward links (from sink to source)
        for (size_t i = 0; i < (backward_path.size()-1); i++) {
            residual_graph.at(backward_path.at(i)).insert(make_pair(backward_path.at(i+1), 1));
        }

        // Reset containers to start a new path search in the next iteration.
        visited.clear();
        backward_path.clear(); 
    }

    // Call print_results() function to print whether there exists a matching or not. 
    print_results(residual_graph, vertices_to_sink, word);
}

int main(int argc, char* argv[]) {

    // Check if the correct number of arguments is provided
    if (argc != 3) {
        cerr << "You need to provide only three command line arguments" << endl;
        return 1;
    }

    // Declare the variable 'line' that will store each line from the input files, the vector 'dice' that contains each of the die, and 'number_of_dice'
    string line;
    int number_of_dice = 0; 
    vector <string> dice; 

    // Open the first file for reading
    ifstream dice_file(argv[1]);
    if (!dice_file.is_open()) {
        cerr << "Failed to open " << argv[1] << endl;
        return 1;
    }

    // Push each of the lines extracted from the first file - representing each die - into the 'dice' vector
    while (getline(dice_file, line)) {
        dice.push_back(line);
        number_of_dice++;
    }
    dice_file.close();

    // Declare the variables 'vector_size' - initialize it to the number of dice plus 2 (source and sink), and 'adjacency_list' which represents the 
    // network flow. In this graph, each index of the vector represents a vertex and then the pairs at each index represent the adjacent vertices to the 
    // current vertex (key) and the respective weight between vertices (values)
    int vector_size = number_of_dice+2; 
    vector<map<int, int> > adjacency_list (vector_size, map<int, int>()); 

    // Create the edges between the source vertex (first element in the network flow) and each of the dice  
    for (size_t i = 0; i < dice.size(); i++) {
        adjacency_list.at(0).insert(make_pair((i+1), 1));
    }

    // Open the second file for reading
    ifstream words_file(argv[2]);
    if (!words_file.is_open()) {
        cerr << "Failed to open " << argv[2] << endl;
        return 1;
    }

    // The following while-loop will loop as long as there is a word to read from the second input file
    while (getline(words_file, line)) {

        // Resize the 'adjacency_list' vector to account for the length of the word extracted
        int new_vector_size = vector_size + line.size();
        adjacency_list.resize(new_vector_size);

        // The external for-loop loops through each element of the 'dice' vector while the internal for-loop loops through each character of 
        // the word extracted. Then, checking whether the current dice contains that specific character
        for (size_t i = 0; i < dice.size(); i++) {
            for (size_t j = 0; j < line.size(); j++) {

                // If the dice contains that character, we create an edge between this dice and the character. 
                if (dice.at(i).find(line.at(j)) != string::npos) {
                    adjacency_list.at(i+1).insert(make_pair((j+dice.size()+1), 1));
                }
            }
        }

        // Create an edge between each letter of the word and the sink, then push in the 'vertices_to_sink' vector the vertex number of these vertices 
        // pointing to the sink.
        vector<int> vertices_to_sink; 
        for (size_t i = dice.size()+1; i < (adjacency_list.size()-1); i++) {
            adjacency_list.at(i).insert(make_pair((adjacency_list.size()-1), 1));
            vertices_to_sink.push_back(i);
        }

        // Call the edmonds_karp function which performs the Edmonds-Karp algorithm.
        edmonds_karp(adjacency_list, vertices_to_sink, line);

        // Clear all the network flow except the edges from source to dice since these are going to be the same for all the words contained 
        // in the word.txt file. 
        for (size_t i = 1; i < adjacency_list.size(); i++) {
            adjacency_list.at(i).clear();
        }
    }

    // Close the second file
    words_file.close();
    return 0;
}