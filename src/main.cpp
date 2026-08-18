#include "crow.h"
#include "combi_data.h"
#include <iostream>

int main()
{
    crow::SimpleApp app;

    // Temporary sanity check for Phase 2 — confirms the hub/route data
    // loaded and the adjacency list built correctly. Phase 3 replaces
    // this with real API endpoints.
    auto adjacency = buildAdjacencyList();
    std::cout << "Loaded " << hubRoutes.size() << " hubs.\n";
    std::cout << "Connections from Main Mall:\n";
    for (const auto& conn : adjacency["Main Mall"]) {
        std::cout << "  -> " << conn.toHub << " (via " << conn.viaRoute << ")\n";
    }

    CROW_STATIC_FILE(app, "/", "public/index.html");

    CROW_ROUTE(app, "/api/ping")([](){
        crow::json::wvalue result;
        result["status"] = "ok";
        return result;
    });

    app.port(8080).multithreaded().run();
}