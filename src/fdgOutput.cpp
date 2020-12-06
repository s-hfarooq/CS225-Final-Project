#include "fdgOutput.h"
#include "../structures/PNG.h"
#include <math.h>

fdgOutput::fdgOutput(Graph graph, int iterations) { 
    defineLocations(graph, iterations); 
}

fdgOutput::~fdgOutput() {}

// Find best suited locations for each node - save location values into vectors
void fdgOutput::defineLocations(Graph graph, int iterations) {
    v = graph.getVertices();
    e = graph.getEdges();
    disp.clear();
    pos.clear();
    disp.resize(v.size(), {0, 0});
    pos.resize(v.size(), {0, 0});

    width = v.size() * 5;
    area = width * width;
    float thr = area, K = std::sqrt(area / v.size());

    // Assign random positions
    for(unsigned i = 0; i < pos.size(); i++) {
        pos[i].first = std::rand() % width;
        pos[i].second = std::rand() % width;
    }

    for(unsigned i = 0; i < iterations; i++) {
        // Repulsion forces
        for(unsigned j = 0; j < v.size(); j++) {
            disp[j] = {0, 0};

            for(unsigned k = 0; k < v.size(); k++) {
                float d = pos[j].first - pos[k].first;
                float d2 = pos[j].second - pos[k].second;
                float dist = std::max((float)0.001, std::sqrt(d * d + d2 * d2));
                float aForce = K * K / dist / v.size() / 100.0;
                disp[j].first += d / dist * aForce;
                disp[j].second += d2 / dist * aForce;
            }
        }

        // Attractive forces
        for(unsigned j = 0; j < e.size(); j++) {
            float loc1 = find(v.begin(), v.end(), e[j].source) - v.begin();
            float loc2 = find(v.begin(), v.end(), e[j].dest) - v.begin();

            float x = pos[i].first - pos[loc2].first;
            float y = pos[i].second - pos[loc2].second;

            float dist = std::max((float)0.001, std::sqrt(x * x + y * y));
            float aForce = dist * dist / K / v.size();

            disp[i].first -= x / dist * aForce;
            disp[i].second += y / dist * aForce;
        }

        // Change position values based on displacement
        for(unsigned j = 0; j < v.size(); j++) {
            float d = std::sqrt(disp[j].first * disp[j].first + disp[j].second * disp[j].second);
            pos[j].first += d > thr ? disp[j].first / d * thr : disp[j].first;
            pos[j].second += d > thr ? disp[j].second / d * thr : disp[j].second;
            // pos[j].first = (int)pos[j].first % width;
            // pos[j].second = (int)pos[j].second % width;

            // // pos[j].first = std::min((float)width - 1, pos[j].first);
            // // pos[j].second = std::min((float)width - 1, pos[j].second);

            // while(pos[j].first > width)
            //     pos[j].first = (int)pos[j].first % width;
            // while(pos[j].second > width)
            //     pos[j].second = (int)pos[j].second % width;
            // pos[j].first = std::max((float)1, pos[j].first);
            // pos[j].second = std::max((float)1, pos[j].second);

            pos[j].first = std::min((float)width, std::max((float)0, pos[j].first));
            pos[j].second = std::min((float)width, std::max((float)0, pos[j].second));
        }

        thr *= 0.99;
    }

    return;
}

// Uses new locations to create output PNG using cs225's PNG class
cs225::PNG fdgOutput::createOutputImage() {
    cs225::PNG out(width + 2, width + 2);

    // Draw verticies
    for(int i = 0; i < v.size(); i++) {
        // Square of size 3x3 instead of single pixel
        for(int j = -1; j < 2; j++) {
            for(int k = -1; k < 2; k++) {
                int x = std::max(0, (int)pos[i].first + j);
                int y = std::max(0, (int)pos[i].second + k);
                out.getPixel(x, y).l = 0;
            }
        }

        //out->getPixel(pos[i].first, pos[i].second).l = 0;
    }

    // Draw edges
    for(int i = 0; i < e.size(); i++) {
        std::pair<float, float> pt1 = pos[find(v.begin(), v.end(), e[i].source) - v.begin()];
        std::pair<float, float> pt2 = pos[find(v.begin(), v.end(), e[i].dest) - v.begin()];

        float slope = (pt1.second - pt2.second) / (pt1.first - pt2.first);
        float yIntercept = pt1.second - (slope * pt1.first);

        int end = std::max(pt1.first, pt2.first);
        for(int j = std::min(pt1.first, pt2.first); j < end; j++)
            out.getPixel(j, (slope * j) + yIntercept).l = 0.6;
    }

    return out;
}

// Debugging function used to print values within class vectors
void fdgOutput::printLocations() {
    for(unsigned i = 0; i < v.size(); i++)
        std::cout << "Vertex: " << v[i] << ", loc: (" << pos[i].first << ", " << pos[i].second << ")" << std::endl;
    
    // for(unsigned i = 0; i < e.size(); i++)
    //     std::cout << "source: " << e[i].source << ", dest: " << e[i].dest << ", label: " << e[i].getLabel() << std::endl;

    std::cout << "width: " << width << " (size : " << area << ")" << std::endl;

    return;
}