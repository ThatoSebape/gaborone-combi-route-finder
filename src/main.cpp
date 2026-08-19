#include "crow.h"
#include "combi_data.h"
#include "route_finder.h"
#include <iostream>

int main()
{
    crow::SimpleApp app;

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

    CROW_ROUTE(app, "/api/hubs")([](){
        crow::json::wvalue result;
        int i = 0;
        for (const auto& [hub, routes] : hubRoutes) {
            result[i++] = hub;
        }
        return result;
    });

    CROW_ROUTE(app, "/api/hub")([](const crow::request& req){
        crow::response res;

        const char* name = req.url_params.get("name");
        if (name == nullptr) {
            res.code = 400;
            res.body = "Missing 'name' query parameter";
            return res;
        }

        std::string hubName(name);
        auto it = hubRoutes.find(hubName);
        if (it == hubRoutes.end()) {
            res.code = 404;
            res.body = "Hub not found: " + hubName;
            return res;
        }

        crow::json::wvalue json;
        json["name"] = hubName;

        int i = 0;
        for (const auto& route : it->second) {
            json["routes"][i++] = route;
        }

        json["landmarks"] = crow::json::wvalue::list();
        if (hubLandmarks.count(hubName)) {
            int j = 0;
            for (const auto& landmark : hubLandmarks[hubName]) {
                json["landmarks"][j++] = landmark;
            }
        }

        res.set_header("Content-Type", "application/json");
        res.body = json.dump();
        return res;
    });

    CROW_ROUTE(app, "/api/route")([&adjacency](const crow::request& req){
        crow::response res;

        const char* fromParam = req.url_params.get("from");
        const char* toParam = req.url_params.get("to");

        if (fromParam == nullptr || toParam == nullptr) {
            res.code = 400;
            res.body = "Both 'from' and 'to' query parameters are required";
            return res;
        }

        std::string from(fromParam);
        std::string to(toParam);

        if (hubRoutes.find(from) == hubRoutes.end()) {
            res.code = 404;
            res.body = "Unknown 'from' hub: " + from;
            return res;
        }
        if (hubRoutes.find(to) == hubRoutes.end()) {
            res.code = 404;
            res.body = "Unknown 'to' hub: " + to;
            return res;
        }

        crow::json::wvalue json;
        json["from"] = from;
        json["to"] = to;

        if (from == to) {
            json["steps"] = crow::json::wvalue::list();
            json["message"] = "You're already there.";
            res.set_header("Content-Type", "application/json");
            res.body = json.dump();
            return res;
        }

        auto path = findRoute(from, to, adjacency);

        if (path.empty()) {
            json["steps"] = crow::json::wvalue::list();
            json["message"] = "No route found between these two places.";
        } else {
            int i = 0;
            for (const auto& step : path) {
                crow::json::wvalue stepJson;
                stepJson["from"] = step.fromHub;
                stepJson["to"] = step.toHub;
                stepJson["route"] = step.viaRoute;
                json["steps"][i++] = std::move(stepJson);
            }
        }

        res.set_header("Content-Type", "application/json");
        res.body = json.dump();
        return res;
    });

    app.port(8080).multithreaded().run();
}