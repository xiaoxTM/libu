# libu
a minimal tools for linux based (g++, c++11) 

# example [u-options.hpp] of usage
```cpp
#include <libu/u-options.hpp>

int main(int argc, char *argv[]) {
    char *source = NULL;
    char *dest = NULL;

    int delta = 5;
    int min_area = 60;
    int max_area = 0;
    double max_variation = 0.25;
    double min_diversity = 0.2;
    int max_evolution = 200;
    double area_thresh = 1.01;
    double min_margin = 0.003;
    int edge_blur_size = 5;

    int level_thresh = 2;

    std::vector<u::options::entry*> options;
    options.push_back(u::options::entry::create<char*>(&source, "--source", "-s", NULL, "source image file for mser"));
    options.push_back(u::options::entry::create<char*>(&dest, "--destinate", "-d", NULL, "destinate image file for connected components"));
    options.push_back(u::options::entry::create<int>(&level_thresh, "--tree-level-thresh", "-t", 2, "level thresh for threshold components that higher than thresh"));
    options.push_back(u::options::entry::create<int>(&delta, "--delta", NULL, 5, "delta for MSER"));
    options.push_back(u::options::entry::create<int>(&max_evolution, "--max-evolution", NULL, 200, "max-evolution for MSER"));
    options.push_back(u::options::entry::create<int>(&edge_blur_size, "--edge-blur-size", NULL, 5, "edge-blur-size for MSER"));
    options.push_back(u::options::entry::create<int>(&min_area, "--min-area", NULL, 60, "min-area for MSER"));
    options.push_back(u::options::entry::create<int>(&max_area, "--max-area", NULL, 0, "max-area for MSER, 0 indicate max of int"));
    options.push_back(u::options::entry::create<double>(&max_variation, "--max-variation", NULL, 0.25, "max-variation for MSER"));
    options.push_back(u::options::entry::create<double>(&min_diversity, "--min-diversity", NULL, 0.2, "min-diversity for MSER"));
    options.push_back(u::options::entry::create<double>(&area_thresh, "--area-thresh", NULL, 1.01, "area-thresh for MSER"));
    options.push_back(u::options::entry::create<double>(&min_margin, "--min-margin", NULL, 0.003, "min-margin for MSER"));

    if (argc == 1) {
        u::options::help("options usage example", options);
    } else {
        u::options::parse(argc, argv, options);
        assert(source != NULL && dest != NULL);
        /*other source code here for further processing*/
    }
    return 0;
}
```
