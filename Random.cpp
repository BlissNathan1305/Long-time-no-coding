#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <memory>
#include <map>
#include <ctime>

// Data structure for stock data
struct StockData {
    double open;
    double high;
    double low;
    double close;
    double volume;
    double next_day_return; // Target: 1 for positive return, 0 for negative
};

// Decision Tree Node
class TreeNode {
public:
    int feature_index;
    double threshold;
    double value; // For leaf nodes
    std::shared_ptr<TreeNode> left;
    std::shared_ptr<TreeNode> right;

    TreeNode() : feature_index(-1), threshold(0), value(-1) {}
};

// Decision Tree Class
class DecisionTree {
private:
    std::shared_ptr<TreeNode> root;
    int max_depth;
    int min_samples_split;

    double calculate_gini(const std::vector<StockData>& data) {
        if (data.empty()) return 0.0;
        
        double p = 0.0;
        for (const auto& d : data) {
            p += d.next_day_return;
        }
        p /= data.size();
        
        return 2 * p * (1 - p);
    }

    std::shared_ptr<TreeNode> build_tree(const std::vector<StockData>& data, int depth) {
        int num_samples = data.size();
        
        // Stopping conditions
        if (depth >= max_depth || num_samples < min_samples_split) {
            auto node = std::make_shared<TreeNode>();
            double sum = 0.0;
            for (const auto& d : data) {
                sum += d.next_day_return;
            }
            node->value = sum / num_samples > 0.5 ? 1.0 : 0.0;
            return node;
        }
        
        // Find best split
        double best_gini = 1.0;
        int best_feature = -1;
        double best_threshold = 0.0;
        std::vector<StockData> best_left, best_right;
        
        // Try all features
        for (int feature = 0; feature < 5; feature++) { // 5 features: open, high, low, close, volume
            // Try different thresholds
            for (const auto& d : data) {
                double threshold = 0.0;
                switch (feature) {
                    case 0: threshold = d.open; break;
                    case 1: threshold = d.high; break;
                    case 2: threshold = d.low; break;
                    case 3: threshold = d.close; break;
                    case 4: threshold = d.volume; break;
                }
                
                std::vector<StockData> left, right;
                for (const auto& sample : data) {
                    double value = 0.0;
                    switch (feature) {
                        case 0: value = sample.open; break;
                        case 1: value = sample.high; break;
                        case 2: value = sample.low; break;
                        case 3: value = sample.close; break;
                        case 4: value = sample.volume; break;
                    }
                    
                    if (value <= threshold) {
                        left.push_back(sample);
                    } else {
                        right.push_back(sample);
                    }
                }
                
                if (left.empty() || right.empty()) continue;
                
                double gini_left = calculate_gini(left);
                double gini_right = calculate_gini(right);
                double weighted_gini = (left.size() * gini_left + right.size() * gini_right) / num_samples;
                
                if (weighted_gini < best_gini) {
                    best_gini = weighted_gini;
                    best_feature = feature;
                    best_threshold = threshold;
                    best_left = left;
                    best_right = right;
                }
            }
        }
        
        if (best_gini == 1.0) { // No improvement
            auto node = std::make_shared<TreeNode>();
            double sum = 0.0;
            for (const auto& d : data) {
                sum += d.next_day_return;
            }
            node->value = sum / num_samples > 0.5 ? 1.0 : 0.0;
            return node;
        }
        
        auto node = std::make_shared<TreeNode>();
        node->feature_index = best_feature;
        node->threshold = best_threshold;
        node->left = build_tree(best_left, depth + 1);
        node->right = build_tree(best_right, depth + 1);
        
        return node;
    }

public:
    DecisionTree(int max_depth = 5, int min_samples_split = 10) 
        : max_depth(max_depth), min_samples_split(min_samples_split) {}
    
    void train(const std::vector<StockData>& data) {
        root = build_tree(data, 0);
    }
    
    double predict(const StockData& sample) const {
        auto node = root;
        while (node->value == -1) {
            double value = 0.0;
            switch (node->feature_index) {
                case 0: value = sample.open; break;
                case 1: value = sample.high; break;
                case 2: value = sample.low; break;
                case 3: value = sample.close; break;
                case 4: value = sample.volume; break;
            }
            
            if (value <= node->threshold) {
                node = node->left;
            } else {
                node = node->right;
            }
        }
        return node->value;
    }
};

// Random Forest Class
class RandomForest {
private:
    std::vector<DecisionTree> trees;
    int n_trees;
    int max_depth;
    int min_samples_split;

    std::vector<StockData> bootstrap_sample(const std::vector<StockData>& data) {
        std::vector<StockData> sample;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, data.size() - 1);
        
        for (size_t i = 0; i < data.size(); i++) {
            sample.push_back(data[dist(gen)]);
        }
        
        return sample;
    }

public:
    RandomForest(int n_trees = 100, int max_depth = 5, int min_samples_split = 10)
        : n_trees(n_trees), max_depth(max_depth), min_samples_split(min_samples_split) {}
    
    void train(const std::vector<StockData>& data) {
        trees.clear();
        for (int i = 0; i < n_trees; i++) {
            auto sample = bootstrap_sample(data);
            DecisionTree tree(max_depth, min_samples_split);
            tree.train(sample);
            trees.push_back(tree);
        }
    }
    
    double predict(const StockData& sample) const {
        double sum = 0.0;
        for (const auto& tree : trees) {
            sum += tree.predict(sample);
        }
        return sum / trees.size();
    }
};

// Helper function to generate synthetic stock data
std::vector<StockData> generate_stock_data(int n_samples) {
    std::vector<StockData> data;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> price_dist(100, 200);
    std::uniform_real_distribution<> volume_dist(100000, 1000000);
    std::uniform_real_distribution<> return_dist(-0.05, 0.05);
    
    for (int i = 0; i < n_samples; i++) {
        StockData d;
        d.open = price_dist(gen);
        d.high = d.open * (1 + return_dist(gen));
        d.low = d.open * (1 - return_dist(gen));
        d.close = d.open * (1 + return_dist(gen));
        d.volume = volume_dist(gen);
        
        // Next day return (1 if positive, 0 if negative)
        d.next_day_return = (return_dist(gen) > 0) ? 1.0 : 0.0;
        
        data.push_back(d);
    }
    
    return data;
}

int main() {
    // Generate synthetic data (in practice, load real stock data)
    std::vector<StockData> stock_data = generate_stock_data(1000);
    
    // Split into training and test sets
    std::random_shuffle(stock_data.begin(), stock_data.end());
    size_t split = stock_data.size() * 0.8;
    std::vector<StockData> train_data(stock_data.begin(), stock_data.begin() + split);
    std::vector<StockData> test_data(stock_data.begin() + split, stock_data.end());
    
    // Train Random Forest
    RandomForest forest(100, 5, 10);
    forest.train(train_data);
    
    // Test the model
    int correct = 0;
    for (const auto& sample : test_data) {
        double prediction = forest.predict(sample);
        if ((prediction > 0.5 && sample.next_day_return == 1) || 
            (prediction <= 0.5 && sample.next_day_return == 0)) {
            correct++;
        }
    }
    
    double accuracy = static_cast<double>(correct) / test_data.size();
    std::cout << "Model Accuracy: " << accuracy * 100 << "%" << std::endl;
    
    // Example prediction for a new stock day
    StockData new_day;
    new_day.open = 150.25;
    new_day.high = 152.30;
    new_day.low = 149.80;
    new_day.close = 151.75;
    new_day.volume = 750000;
    
    double buy_signal = forest.predict(new_day);
    std::cout << "Buy Signal (1 = buy, 0 = don't buy): " << (buy_signal > 0.5 ? 1 : 0) << std::endl;
    std::cout << "Confidence: " << buy_signal * 100 << "%" << std::endl;
    
    return 0;
}
