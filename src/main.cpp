
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


json readJsonFile(const string& filename) {
    ifstream file(filename);
    json data;
    if (!file.is_open()) {
        cout << "missing sensor JSON report";
    }
    else {
        file >> data;
        return data;
    }
    
}

void generateJsonFiles(int fileCount) {
    mt19937 gen(time(nullptr));
    uniform_real_distribution<> valueDist(0.0, 100.0);
    uniform_int_distribution<> errorDist(0, 20);

    for (int f = 0; f < fileCount; f++) {
        json data = json::array();

        for (int i = 0; i < 60; i++) {
            json record;
            record["sensor"] = "s1";

            ostringstream ts;
            ts << "2025-06-01T00:"
                << setw(2) << setfill('0') << i
                << ":00Z";

            if (errorDist(gen) != 0)
                record["ts"] = ts.str();

            double value = valueDist(gen);
            if (errorDist(gen) == 1)
                value = -50;

            record["value"] = value;
            data.push_back(record);
        }

        ofstream file("data_" + to_string(f) + ".json");
        file << data.dump(4);
    }
}
void generateCorrectJsonFiles(int fileCount) {
    mt19937 gen(time(nullptr));
    uniform_real_distribution<> valueDist(0.0, 100.0);

    for (int f = 0; f < fileCount; f++) {
        json data = json::array();

        for (int i = 0; i < 60; i++) {
            json record;
            record["sensor"] = "s1";

            ostringstream ts;
            ts << "2025-06-01T00:"
                << setw(2) << setfill('0') << i
                << ":00Z";


            record["ts"] = ts.str();

            double value = valueDist(gen);

            record["value"] = value;
            data.push_back(record);
        }

        ofstream file("data_" + to_string(f) + ".json");
        file << data.dump(4);
    }
}






void validateData(const json& data) {
    set<string> timestamps;

    for (const auto& item : data) {
        if (!item.contains("ts")) {
            cout << "Missing ts\n";
            continue;
        }

        string ts = item["ts"];
        if (!timestamps.insert(ts).second)
            cout << "Duplicate ts: " << ts << "\n";

        if (!item.contains("value") || !item["value"].is_number()) {
            cout << "Invalid value type\n";
        }
        else {
            double value = item["value"];
            if (value < 0 || value > 100)
                cout << "Invalid value range: " << value << "\n";
        }
    }
}

void analyzeSensorData(const json& data, const string& sensorFilter, int windowSize, bool timecheck) {
    auto startMedian = chrono::high_resolution_clock::now();
    vector<double> values;


    for (const auto& item : data) {
        if (item.contains("sensor") && item["sensor"] == sensorFilter &&
            item.contains("value") && item["value"].is_number()) {
            values.push_back(item["value"]);
        }
    }

    if (values.empty()) {
        cout << "No data for sensor " << sensorFilter << endl;
        return;
    }


    double sum = 0;
    double minVal = values[0];
    double maxVal = values[0];
    for (double v : values) {
        sum += v;
        if (v < minVal) minVal = v;
        if (v > maxVal) maxVal = v;
    }
    double avg = sum / values.size();

    vector<double> sorted = values;
    sort(sorted.begin(), sorted.end());
    double median;
    int n = sorted.size();
    if (n % 2 == 1)
        median = sorted[n / 2];
    else
        median = (sorted[n / 2 - 1] + sorted[n / 2]) / 2.0;

    auto endMedian = chrono::high_resolution_clock::now();
    auto medianDuration = chrono::duration_cast<chrono::microseconds>(endMedian - startMedian).count();

    auto startWindow = chrono::high_resolution_clock::now();

    vector<int> anomalies;
    for (int i = 0; i + windowSize <= values.size(); i++) {
        double windowSum = 0;
        for (int j = 0; j < windowSize; j++)
            windowSum += values[i + j];
        double windowAvg = windowSum / windowSize;

        double current = values[i + windowSize - 1];
        if (current > windowAvg * 1.5 || current < windowAvg * 0.5)
            anomalies.push_back(i + windowSize - 1);

    }

    auto endWindow = chrono::high_resolution_clock::now();
    auto windowDuration = chrono::duration_cast<chrono::microseconds>(endWindow - startWindow).count();

    cout << "\n===== SUMMARY =====\n";
    cout << "Sensor: " << sensorFilter << endl;
    cout << "Count: " << values.size() << endl;
    cout << "Min: " << minVal << endl;
    cout << "Max: " << maxVal << endl;
    cout << "Average: " << avg << endl;
    cout << "Median: " << median << endl;
    cout << "Anomalies found: " << anomalies.size() << endl;

    if (!anomalies.empty()) {
        cout << "\nAnomaly list:\n";
        for (int idx : anomalies)
            cout << "Index " << idx << ", value = " << values[idx] << endl;
    }
    cout << "----------------------\n";



    if (timecheck == 1) {
        cout << "Median calculation time: " << medianDuration << " microseconds" << endl;
        cout << "Sliding window calculation time: " << windowDuration << " microseconds" << endl;
    }
}


int main() {



    cout << "------------------------------------------------------------------" << endl;

    cout << "                             _       _               _            " << endl;
    cout << R"(  ___ ___ _ _  ___ ___ _ _  | |_ ___| |___ _ __  ___| |_ _ _ _  _ )" << endl;
    cout << R"( (_-</ -_) ' \(_-</ _ \ '_| |  _/ -_) / -_) '  \/ -_)  _| '_| || |)" << endl;
    cout << R"( /__/\___|_||_/__/\___/_|    \__\___|_\___|_|_|_\___|\__|_|  \_, |)" << endl;
    cout << R"(                                                             |__/ )" << endl;

    cout << "------------------------------------------------------------------" << endl;
    cout << "Create JSON files named: data_{number}.json" << endl << "file number start from 0" << endl;

    while (true) {
        cout << "Choose option" << endl;
        cout << "1) Functions" << endl;
        cout << "2) Debug mode" << endl;
        cout << "3) Exit programm" << endl;
        string choose;
        cin >> choose;
        if (choose == "2") {
            cout << "Choose option" << endl;
            cout << "1) Generate JSON examples with errors" << endl;
            cout << "2) Generate JSON examples without errors" << endl;
            cout << "3) Check calculation time" << endl;
            cin >> choose;
            if (choose == "1") {
                cout << "enter amount of files" << endl;
                int amount;
                cin >> amount;
                generateJsonFiles(amount);
                cout << amount << "files generated" << endl;
            }
            else if (choose == "2") {
                cout << "enter amount of files" << endl;
                int amount;
                cin >> amount;
                generateCorrectJsonFiles(amount);
                cout << amount << "files generated" << endl;
            }
            else if (choose == "3") {
                cout << "Enter amount of files" << endl;
                int amount2;
                cin >> amount2;
                string sensorFilter;
                cout << "Enter sensor name (example s1): ";
                cin >> sensorFilter;

                int windowSize;
                cout << "Enter window size (number of points): ";
                cin >> windowSize;

                for (int i = 0; i < amount2; i++) {
                    string filename = "data_" + to_string(i) + ".json";
                    ifstream test(filename);
                    if (!test.is_open()) continue;

                    json data = readJsonFile(filename);
                    cout << "Analyzing " << filename << "\n";
                    analyzeSensorData(data, sensorFilter, windowSize, 1);
                }
            }
        }
        else if (choose == "1") {
            cout << "Valid checking files" << endl << "Enter amount of files" << endl;
            int amount2;
            cin >> amount2;
            for (int i = 0; i < amount2; i++) {

                string filename = "data_" + to_string(i) + ".json";
                json data = readJsonFile(filename);

                cout << "Checking " << filename << "\n";
                validateData(data);
                cout << "----------------\n";
            }
            string sensorFilter;
            cout << "Enter sensor name (example s1): ";
            cin >> sensorFilter;

            int windowSize;
            cout << "Enter window size (number of points): ";
            cin >> windowSize;

            for (int i = 0; i < amount2; i++) {
                string filename = "data_" + to_string(i) + ".json";
                ifstream test(filename);
                if (!test.is_open()) continue;

                json data = readJsonFile(filename);
                cout << "Analyzing " << filename << "\n";
                analyzeSensorData(data, sensorFilter, windowSize, 0);
            }

        }
        else if (choose == "3") {
            return 0;
        }
        else {
            cout << "unknown option(1-2!)";
        }

    }
    
}

