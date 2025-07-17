#include <iostream>
#include <fstream>
#include <string>
using namespace std;

const int MAX_SAMPLES = 150;
const int MAX_FEATURES = 4;

struct DataPoint {
    double features[MAX_FEATURES];
    int label;
};

double distance(const DataPoint &a, const DataPoint &b, int numFeatures) {
    double sum = 0.0;
    for (int i = 0; i < numFeatures; ++i) {
        double diff = a.features[i] - b.features[i];
        sum += diff * diff;
    }
    return sum;
}

int predictLabel(DataPoint trainingSet[], int trainSize, const DataPoint &testPoint, int k, int numFeatures) {
    double distances[MAX_SAMPLES];
    int labels[MAX_SAMPLES];

    for (int i = 0; i < trainSize; ++i) {
        distances[i] = distance(trainingSet[i], testPoint, numFeatures);
        labels[i] = trainingSet[i].label;
    }

    for (int i = 0; i < trainSize - 1; ++i) {
        for (int j = 0; j < trainSize - i - 1; ++j) {
            if (distances[j] > distances[j + 1]) {
                swap(distances[j], distances[j + 1]);
                swap(labels[j], labels[j + 1]);
            }
        }
    }

    int count[3] = {0, 0, 0}; 
    for (int i = 0; i < k; ++i) {
        count[labels[i]]++;
    }

    int maxLabel = 0;
    for (int i = 1; i < 3; ++i) {
        if (count[i] > count[maxLabel]) {
            maxLabel = i;
        }
    }
    return maxLabel;
}

int readDataset(const string &filename, DataPoint dataset[], int numFeatures) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return 0;
    }

    int count = 0;
    while (count < MAX_SAMPLES && !file.eof()) {
        for (int i = 0; i < numFeatures; ++i) {
            file >> dataset[count].features[i];
        }
        file >> dataset[count].label;
        if (file.fail()) break;
        count++;
    }

    file.close();
    return count;
}

void writePredictions(const string &filename, DataPoint testSet[], int testSize, int predictions[], int correct, int numFeatures) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    for (int i = 0; i < testSize; ++i) {
        for (int j = 0; j < numFeatures; ++j) {
            file << testSet[i].features[j] << " ";
        }
        file << "Actual: " << testSet[i].label << " Predicted: " << predictions[i] << endl;
    }

    double accuracy = (double(correct) / testSize) * 100.0;
    file << "Accuracy: " << accuracy << "%" << endl;

    file.close();
}

int main() {
    string trainFile = "iris_2features_50samples_train.txt";   // Path to the training dataset.You can change this to the path of your training dataset
    string testFile = "iris_2features_50samples_test.txt";  // Path to the test dataset. You can change this to the path of your test dataset
    string predictionFile = "t3.txt";

    int k = 3;
    int numFeatures = 2;// Number of features in the dataset 

    DataPoint trainingSet[MAX_SAMPLES];
    DataPoint testSet[MAX_SAMPLES];

    int trainSize = readDataset(trainFile, trainingSet, numFeatures);
    int testSize = readDataset(testFile, testSet, numFeatures);

    if (trainSize == 0 || testSize == 0) return 1;

    int predictions[MAX_SAMPLES];
    int correct = 0;

    for (int i = 0; i < testSize; ++i) {
        predictions[i] = predictLabel(trainingSet, trainSize, testSet[i], k, numFeatures);
        if (predictions[i] == testSet[i].label) {
            correct++;
        }
    }

    writePredictions(predictionFile, testSet, testSize, predictions, correct, numFeatures);
    cout << "Predictions written to " << predictionFile << endl;

    return 0;
}
