#pragma once

#include <string>
#include <vector>
#include <unordered_map>

// A physical location: a mall, or in a few cases a residential/outlying area.
struct Hub {
    std::string name;
    std::vector<std::string> landmarks; // may be empty
};

// An edge from one hub to another, labeled with which combi route to catch.
struct Connection {
    std::string toHub;
    std::string viaRoute; // e.g. "Broadhurst Route 1"
};

// hub name -> list of route names that serve it (the "source of truth")
inline std::unordered_map<std::string, std::vector<std::string>> hubRoutes = {

    {"Airport Junction Mall", {
        "Block 8 Route 4", "Phakalane Phase 1", "Phakalane Phase 2",
        "Modipane", "Oodi Matebele", "Gaborone North"
    }},

    {"BBS Mall", {
        "Broadhurst Route 1", "Tlokweng Route 5", "Block 8 Route 3", "Block 10 Bus Rank"
    }},

    {"African Mall", {
        "Tlokweng Route 1", "Tlokweng Route 2", "Taxi"
    }},

    {"Riverwalk Mall", {
        "Tlokweng Route 1", "Tlokweng Route 2", "Tlokweng Route 6"
    }},

    {"Sebele Centre/Northgate Crossing", {
        "Phakalane Phase 1", "Phakalane Phase 2", "Gaborone North", "Oodi Matebele"
    }},

    {"Westgate Mall", {
        "Block 6/Mogoditshane Route 8", "Broadhurst Route 6"
    }},

    {"Game City", {
        "Kgaleview Route 2", "Broadhurst Route 6", "Mogoditshane Route 11 Gamecity", "Tlokweng Route 6"
    }},

    {"MiddleStar", {
        "Broadhurst Route 1"
    }},

    {"Fairgrounds Mall", {
        "Broadhurst Route 5", "Tlokweng Route 6"
    }},

    {"Southring Mall", {
        "Tlokweng Route 1", "Tlokweng Route 2", "Tlokweng Route 3"
    }},

    {"Main Mall", {
        "Broadhurst Route 1", "Broadhurst Route 2", "Block 8 Route 3",
        "Block 10 Bus Rank", "Block 8 Route 1", "Block 8 Route 2"
    }},

    {"Molapo Crossing", {
        "Broadhurst Route 6", "Block 6/Mogoditshane Route 8"
    }},

    {"Acacia Mall", {
        "Phakalane Phase 2", "Oodi"
    }},

    {"Square Mart", {
        "Block 8 Route 1", "Block 8 Route 2", "Taxi"
    }},
};

// hub name -> landmarks found there
inline std::unordered_map<std::string, std::vector<std::string>> hubLandmarks = {
    {"BBS Mall", {"Flea market", "BBS Bank", "BSB Bank", "DRTS"}},
    {"Westgate Mall", {"BEC", "BTV"}},
    {"Game City", {"Kgale Mews", "Kgale Hill"}},
    {"MiddleStar", {"Diagnofirm", "Avicenna Clinic"}},
    {"Fairgrounds Mall", {"DRTS"}},
    {"Main Mall", {"British Council", "Blood Donation Centre", "Gaborone City Council", "Marketplace"}},
    {"Molapo Crossing", {"Stanbic Piazza"}},
    {"Square Mart", {"Three Dikgosi Monument", "Industrial Court", "SADC"}},
};

// Direct hub-to-hub links noted in the source data without a specific
// named/numbered route attached — i.e. "you can get there directly,"
// but there's no route name to tell someone to look for.
inline std::vector<std::pair<std::string, std::string>> unnamedDirectLinks = {
    {"Southring Mall", "Main Mall"},
};

// Derive hub-to-hub adjacency: two hubs are connected if they share a
// named route, OR if they're listed as an unnamed direct link.
inline std::unordered_map<std::string, std::vector<Connection>> buildAdjacencyList() {
    std::unordered_map<std::string, std::vector<Connection>> adjacency;

    std::unordered_map<std::string, std::vector<std::string>> routeHubs;
    for (const auto& [hub, routes] : hubRoutes) {
        for (const auto& route : routes) {
            routeHubs[route].push_back(hub);
        }
    }

    for (const auto& [route, hubs] : routeHubs) {
        for (size_t i = 0; i < hubs.size(); ++i) {
            for (size_t j = 0; j < hubs.size(); ++j) {
                if (i == j) continue;
                adjacency[hubs[i]].push_back(Connection{hubs[j], route});
            }
        }
    }

    const std::string unnamedLabel = "Direct connection (specific route not recorded)";
    for (const auto& [hubA, hubB] : unnamedDirectLinks) {
        adjacency[hubA].push_back(Connection{hubB, unnamedLabel});
        adjacency[hubB].push_back(Connection{hubA, unnamedLabel});
    }

    return adjacency;
}