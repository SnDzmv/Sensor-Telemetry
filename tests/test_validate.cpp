
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>
#include <vector>
#include <chrono>

#include "json.hpp"

using namespace std;
using json = nlohmann::json;

void validateData(const json& data) {
    set<string> timestamps;

    for (const auto& item : data) {
        if (!item.contains("ts")) {
            cout << "отсутсвует ts" << endl;
            continue;
        }

        string ts = item["ts"];
        if (!timestamps.insert(ts).second)
            cout << "Дубликат ts: " << ts << endl;

        if (!item.contains("value") || !item["value"].is_number()) {
            cout << "Неверный тип value" << endl;
        }
        else {
            double value = item["value"];
            if (value < 0 || value > 100)
                cout << "Невозможное значение value: " << value << endl;
        }
    }
}


void test_validate_correct() {
    json data = json::array({
        {{"sensor", "s1"}, {"ts", "2025-06-01T00:00:00Z"}, {"value", 10.5}},
        {{"sensor", "s1"}, {"ts", "2025-06-01T00:01:00Z"}, {"value", 12.0}}
        });
    std::cout << "Test: correct data\n";
    validateData(data);
}

void test_validate_errors() {
    json data = json::array({
        {{"sensor", "s1"}, {"ts", "2025-06-01T00:00:00Z"}, {"value", -5}},
        {{"sensor", "s1"}, {"ts", "2025-06-01T00:00:00Z"}, {"value", 200}},
        {{"sensor", "s1"}}
        });
    std::cout << "Test: data with errors\n";
    validateData(data);
}

int main() {
    setlocale(LC_ALL, "");
    test_validate_correct();
    test_validate_errors();
    return 0;
}


