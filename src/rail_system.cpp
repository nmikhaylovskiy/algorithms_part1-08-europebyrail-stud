#pragma warning (disable:4786)
#pragma warning (disable:4503)




#include "rail_system.h"
#include <vector>
#include <algorithm>
#include <climits>
#include "service.h"

using namespace std;

// TODO: Implement all methods


void RailSystem::load_services(const std::string &filename) {

    ifstream fail(filename.c_str());
    string fr;
    string to;
    int fee;
    int dist;
    if (fail.is_open()) {
        while (fail.good()) {
            fail >> fr >> to >> fee >> dist;

            if (fail.good()) {
                if (cities.find(fr) == cities.end())
                    cities[fr] = new City(fr);
                if (cities.find(to) == cities.end())
                    cities[to] = new City(to);
                outgoing_services[fr].push_back(new Service(to, fee, dist));
            }
        }

        fail.close();
    } else {
        throw logic_error("no file");
    }


}

void RailSystem::reset() {
    for (std::pair<std::string, City *> res : cities) {
        res.second->visited = false;
        res.second->total_fee = INT_MAX;
        res.second->total_distance = INT_MAX;
        res.second->from_city = "";
    }
}

std::vector<string> RailSystem::recover_route(const std::string &city) {
    vector<string> res;
    string City = cities[city]->from_city;
    if (City != "")
        res.push_back(city);
    while (City != "") {
        res.push_back(City);
        City = cities[City]->from_city;
    }

    reverse(begin(res), end(res));

    return res;
}

std::pair<int, int> RailSystem::calc_route(std::string from, std::string to) {
    if (!is_valid_city(to) || !is_valid_city(from))
        throw invalid_argument("no city");
    reset();
    std::priority_queue<City *, vector<City *>, Cheapest> whoisit;
    cities[from]->visited = true;
    cities[from]->total_distance = 0;
    cities[from]->total_fee = 0;
    whoisit.push(cities[from]);

    while (whoisit.empty() == false) {
        City *wherewearenow = whoisit.top();
        whoisit.pop();
        if (wherewearenow->name == to)
            break;
        for (Service *service : outgoing_services[wherewearenow->name]) {
            if (cities[service->destination]->total_fee > wherewearenow->total_fee + service->fee) {
                cities[service->destination]->visited = true;
                cities[service->destination]->from_city = wherewearenow->name;
                cities[service->destination]->total_fee = wherewearenow->total_fee + service->fee;
                cities[service->destination]->total_distance = wherewearenow->total_distance + service->distance;
                whoisit.push(cities[service->destination]);
            }
        }
    }

    if (cities[to]->visited)
        return std::pair<int, int>(cities[to]->total_fee, cities[to]->total_distance);
    return pair<int, int>(INT_MAX, INT_MAX);
}

RailSystem::RailSystem(const std::string &filename) {
    load_services(filename);
}

RailSystem::~RailSystem() {
    outgoing_services.clear();
    cities.clear();
}

void RailSystem::output_cheapest_route(const std::string &from, const std::string &to) {
    reset();
    std::pair<int, int> totals = calc_route(from, to);
    if (totals.first == INT_MAX) {
        cout << "No route from " << from << " to " << to << "\n";
        return;
    }
    cout << "The cheapest route from " << from << " to " << to << "\n";
    cout << "costs " << totals.first << "eu and  spans " << totals.second << "km";
}

bool RailSystem::is_valid_city(const std::string &name) {
    return cities.count(name) == 1;
}

Route RailSystem::getCheapestRoute(const std::string &from, const std::string &to) {
    reset();
    pair<int, int> p = calc_route(from, to);
    return Route(from, to, p.first, p.second, recover_route(to));
}
