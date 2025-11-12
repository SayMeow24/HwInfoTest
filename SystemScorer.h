#ifndef SYSTEMSCORER_H
#define SYSTEMSCORER_H

#include "HardwareInfoProvider.h"
#include <string>
#include <vector>
#include <map>
#include <optional>

// ========================================
// Структура для GPU benchmark
// ========================================
struct GPUBenchmark {
    std::string model_pattern;  // "RX 6600", "RTX 3080"
    double hashrate;            // MH/s для Ethereum (приклад)
    double power_efficiency;    // MH/W
    int tier;                   // 1-5 (1 = найкраща)
    
    GPUBenchmark(const std::string& pattern, double hash, double efficiency, int t)
        : model_pattern(pattern), hashrate(hash), power_efficiency(efficiency), tier(t) {}
};

// ========================================
// Ваги для різних завдань
// ========================================
struct TaskWeights {
    double gpu_weight;
    double vram_weight;
    double cpu_weight;
    double ram_weight;
    double disk_weight;
    
    TaskWeights(double gpu = 0.5, double vram = 0.25, double cpu = 0.1, 
                double ram = 0.1, double disk = 0.05)
        : gpu_weight(gpu), vram_weight(vram), cpu_weight(cpu), 
          ram_weight(ram), disk_weight(disk) {}
};

// ========================================
// Результат оцінки системи
// ========================================
struct SystemScore {
    double total_score;         // 0-100
    double gpu_score;           // 0-100
    double vram_score;          // 0-100
    double cpu_score;           // 0-100
    double ram_score;           // 0-100
    double disk_score;          // 0-100
    
    std::string rating;         // "Excellent", "Good", "Average", "Poor"
    std::string recommendation; // Текстова рекомендація
    
    // Детальна інформація
    std::string gpu_details;
    std::string vram_details;
    std::string cpu_details;
    std::string ram_details;
    std::string disk_details;
    
    SystemScore() 
        : total_score(0), gpu_score(0), vram_score(0), 
          cpu_score(0), ram_score(0), disk_score(0) {}
};

// ========================================
// Клас для оцінювання системи
// ========================================
class SystemScorer {
public:
    SystemScorer();
    ~SystemScorer();
    
    // Головний метод оцінювання
    SystemScore scoreSystem(
        const ArgentumDevice& device,
        const TaskWeights& weights,
        const std::string& task_type = "mining"
    ) const;
    
    // Преднастроєні ваги для різних завдань
    static TaskWeights getMiningWeights();
    static TaskWeights getAITrainingWeights();
    static TaskWeights getVideoRenderingWeights();
    static TaskWeights getGamingWeights();
    static TaskWeights getGeneralWeights();
    
    // Вивід результату
    static void printScore(const SystemScore& score);
    static void printDetailedScore(const SystemScore& score);
    static void printComparison(const SystemScore& score1, const SystemScore& score2,
                               const std::string& name1 = "System 1", 
                               const std::string& name2 = "System 2");
    
    // Отримати інформацію про GPU з бази
    std::optional<GPUBenchmark> findGPU(const std::string& model) const;
    
private:
    // База даних GPU
    std::vector<GPUBenchmark> gpu_database_;
    
    // Методи оцінювання окремих компонентів
    double calculateGPUScore(const std::string& gpu_model, std::string& details) const;
    double calculateVRAMScore(uint64_t vram_mb, const std::string& task_type, std::string& details) const;
    double calculateCPUScore(uint32_t cores, uint32_t frequency_mhz, std::string& details) const;
    double calculateRAMScore(uint64_t ram_mb, const std::string& task_type, std::string& details) const;
    double calculateDiskScore(DiskType primary_type, uint64_t free_mb, std::string& details) const;
    
    // Допоміжні методи
    std::string getRatingFromScore(double score) const;
    std::string getRecommendationFromScore(double score, const std::string& task_type) const;
    void initializeGPUDatabase();
};

#endif // SYSTEMSCORER_H